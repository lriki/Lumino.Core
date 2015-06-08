
#include "../Internal.h"
#include <Lumino/Base/StringUtils.h>
#include <Lumino/IO/StringReader.h>
#include <Lumino/Json/JsonReader.h>

namespace Lumino
{
namespace Json
{

//=============================================================================
// JsonReader
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JsonReader::JsonReader(JsonHandler* handler)
	: m_error()
	, m_handler(handler)
	, m_reader(NULL)
	, m_tmpStream()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JsonReader::~JsonReader()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonReader::Parse(const TCHAR* text, int len)
{
	if (LN_VERIFY_ASSERT_ARG(text != NULL)) { return; }

	StringReader textReader(String(text, len));
	Parse(&textReader);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonReader::Parse(TextReader* textReader)
{
	if (LN_VERIFY_ASSERT_ARG(textReader != NULL)) { return; }
	m_reader = textReader;

	// �ꎞ�o�b�t�@�B��������Ȃ���Ίg�������
	m_tmpStream.Create(512);

	// �o�b�t�@�擪�̋󔒂�ǂݔ�΂�
	if (!SkipWhitespace())
	{
		// Error: �o�b�t�@���󂾂���
		m_error.SetError(ParseError_DocumentEmpty, m_reader->GetPosition());
		return;
	}

	// ���[�g�v�f�̉��
	if (!ParseValue()) {
		return;
	}

	// �o�b�t�@�I�[�̋󔒂�ǂݔ�΂�
	if (SkipWhitespace())
	{
		// Error: �����̃��[�g�v�f����������
		m_error.SetError(ParseError_DocumentRootNotSingular, m_reader->GetPosition());
		return;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonReader::SkipWhitespace()
{
	while (m_reader->Peek() == ' ' || m_reader->Peek() == '\n' || m_reader->Peek() == '\r' || m_reader->Peek() == '\t') {
		m_reader->Read();
	}
	return !m_reader->IsEOF();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonReader::ParseValue()
{
	switch (m_reader->Peek())
	{
		case 'n': return ParseNull();			// null ��������Ȃ�
		case 't': return ParseTrue();			// true ��������Ȃ�
		case 'f': return ParseFalse();			// false ��������Ȃ�
		case '"': return ParseString(false);	// �����񂩂�����Ȃ�
		case '[': return ParseArray();			// �z�񂩂�����Ȃ�
		case '{': return ParseObject();			// �I�u�W�F�N�g��������Ȃ�
		default: return ParseNumber();			// ���l��������Ȃ�
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonReader::ParseNull()
{
	m_reader->Read();	// skip 'n'
	if (m_reader->Read() == 'u' &&
		m_reader->Read() == 'l' &&
		m_reader->Read() == 'l')
	{
		if (!m_handler->OnNull())
		{
			// ���f
			m_error.SetError(ParseError_Termination, m_reader->GetPosition());
			return false;
		}
	}
	else
	{
		// Error: "null" �ł͂Ȃ�����
		m_error.SetError(ParseError_ValueInvalid, m_reader->GetPosition());
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonReader::ParseTrue()
{
	m_reader->Read();	// skip 't'
	if (m_reader->Read() == 'r' &&
		m_reader->Read() == 'u' &&
		m_reader->Read() == 'e')
	{
		if (!m_handler->OnBool(true))
		{
			// ���f
			m_error.SetError(ParseError_Termination, m_reader->GetPosition());
			return false;
		}
	}
	else
	{
		// Error: "true" �ł͂Ȃ�����
		m_error.SetError(ParseError_ValueInvalid, m_reader->GetPosition());
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonReader::ParseFalse()
{
	m_reader->Read();	// skip 'f'
	if (m_reader->Read() == 'a' &&
		m_reader->Read() == 'l' &&
		m_reader->Read() == 's' &&
		m_reader->Read() == 'e')
	{
		if (!m_handler->OnBool(false))
		{
			// ���f
			m_error.SetError(ParseError_Termination, m_reader->GetPosition());
			return false;
		}
	}
	else
	{
		// Error: "false" �ł͂Ȃ�����
		m_error.SetError(ParseError_ValueInvalid, m_reader->GetPosition());
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonReader::ParseNumber()
{
	// ���l�ւ̕ϊ��ɂ� strtod ���g�p����B���̂��߁A�܂��͐��l�����ł��镶����S�ēǂݎ��
	m_tmpStream.Seek(0, SeekOrigin_Begin);
	int len = 0;
	TCHAR ch;
	while (true)
	{
		ch = m_reader->Peek();	// �ǂނ����B�|�C���^�͐i�߂Ȃ�
		if (ch == '.' ||
			('0' <= ch && ch <= '9') ||
			(ch == 'e' || ch == 'E') ||
			(ch == '+' || ch == '-'))
		{
			m_tmpStream.Write(&ch, sizeof(TCHAR));
			++len;
			m_reader->Read();	// ������1�i�߂�
		}
		else {
			break;				// ��v���Ȃ���΃|�C���^�͐i�߂Ȃ�
		}
	}
	if (len == 0)
	{
		// Error: ���l���ۂ�������������Ȃ�����
		m_error.SetError(ParseError_NumberInvalid, m_reader->GetPosition());
		return false;
	}
	ch = '\0';
	m_tmpStream.Write(&ch, sizeof(TCHAR));	// �I�[ \0

	// double �֕ϊ�����
	TCHAR* str = (TCHAR*)m_tmpStream.GetBuffer();
	TCHAR* endptr = NULL;
	NumberConversionResult result;
	double value = StringUtils::ToDouble(str, len, &endptr, &result);
	if ((endptr - str) != len)	// ����ɕϊ��ł��Ă���΁A�ǂݎ�������������S�ď�����͂�
	{
		// Error: �\�����������Ȃ�
		m_error.SetError(ParseError_NumberInvalid, m_reader->GetPosition());
		return false;
	}
	if (result == NumberConversionResult_Overflow) {
		// Error: �I�[�o�[�t���[����������
		m_error.SetError(ParseError_NumberOverflow, m_reader->GetPosition());
		return false;
	}

	// Handler �ɒʒm����
	if (!m_handler->OnDouble(value))
	{
		// ���f
		m_error.SetError(ParseError_Termination, m_reader->GetPosition());
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonReader::ParseString(bool isKey)
{
	// http://json.org/json-ja.html
	static const TCHAR escapeTable[256] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, '\"', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '/',
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '\\', 0, 0, 0,
		0, 0, '\b', 0, 0, 0, '\f', 0, 0, 0, 0, 0, 0, 0, '\n', 0,
		0, 0, '\r', 0, '\t', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};

	// 1���ǂ񂾕������i�[���Ă����ꎞ�o�b�t�@�B�V�[�N�ʒu��擪�ɖ߂��Ă���
	m_tmpStream.Seek(0, SeekOrigin_Begin);

	m_reader->Read();	// skip '"'
	while (true)
	{
		TCHAR c = m_reader->Peek();

		// �G�X�P�[�v�V�[�P���X
		if (c == '\\')
		{
			m_reader->Read();	// skip '\'
			TCHAR esc = m_reader->Read();
			// ��{�I�ȃG�X�P�[�v
			if (unsigned(esc) < 256 && escapeTable[(unsigned char)esc])
			{
				m_tmpStream.Write(&escapeTable[(unsigned char)esc], sizeof(TCHAR));
			}
			// Unicode �G�X�P�[�v
			else if (esc == 'u')
			{
				// ������
				m_error.SetError(ParseError_StringEscapeInvalid, m_reader->GetPosition());
				return false;
			}
			else
			{
				// Error: �����ȃG�X�P�[�v
				m_error.SetError(ParseError_StringEscapeInvalid, m_reader->GetPosition());
				return false;
			}
		}
		// ������I�[
		else if (c == '"')
		{
			m_reader->Read();	// skip '"'
			break;
		}
		// ������̓r���Ńo�b�t�@���؂ꂽ
		else if (m_reader->IsEOF() || c == '\0')
		{
			// Error: " ����v���Ȃ�����
			m_error.SetError(ParseError_StringMissQuotationMark, m_reader->GetPosition());
			return false;
		}
		// 0x20 �����̐��䕶���͎g���Ȃ�
		else if ((unsigned)c < 0x20) {
			// RFC 4627: unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
			m_error.SetError(ParseError_StringEscapeInvalid, m_reader->GetPosition());
			return false;
		}
		// ���ʂ̕���
		else
		{
			m_tmpStream.Write(&c, sizeof(TCHAR));
			m_reader->Read();
		}
	}

	// Handler �ɒʒm
	bool cont = false;
	if (isKey) {
		cont = m_handler->OnKey((TCHAR*)m_tmpStream.GetBuffer(), ((int)m_tmpStream.GetPosition()) / sizeof(TCHAR));
	}
	else {
		cont = m_handler->OnString((TCHAR*)m_tmpStream.GetBuffer(), ((int)m_tmpStream.GetPosition()) / sizeof(TCHAR));
	}
	if (!cont)
	{
		// ���f
		m_error.SetError(ParseError_Termination, m_reader->GetPosition());
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonReader::ParseArray()
{
	m_reader->Read();  // skip '['

	// Handler �� Array �̊J�n��ʒm
	if (!m_handler->OnStartArray())
	{
		// ���f
		m_error.SetError(ParseError_Termination, m_reader->GetPosition());
		return false;
	}

	SkipWhitespace();
	if (m_reader->Peek() == ']')
	{
		// ��z�񂾂����BArray �̏I����ʒm����
		m_reader->Read(); 	// ']' �̎����w���Ă���
		if (!m_handler->OnEndArray(0))
		{
			// ���f
			m_error.SetError(ParseError_Termination, m_reader->GetPosition());
			return false;
		}
		return true;
	}

	SkipWhitespace();
	size_t elementCount = 0;
	while (true)
	{
		if (!ParseValue()) {
			return false;	// �G���[�͏����ς�
		}

		++elementCount;
		SkipWhitespace();

		switch (m_reader->Read())
		{
		case ',':
			SkipWhitespace();
			break;
		case ']':
			// �z��̏I�[
			if (!m_handler->OnEndArray(elementCount))
			{
				// ���f
				m_error.SetError(ParseError_Termination, m_reader->GetPosition());
				return false;
			}
			SkipWhitespace();
			return true;
		default:
			m_error.SetError(ParseError_ArrayMissCommaOrSquareBracket, m_reader->GetPosition());
			return false;
		}
	}

	// �����ɗ��邱�Ƃ͂Ȃ�
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonReader::ParseObject()
{
	m_reader->Read();  // Skip '{'

	// �I�u�W�F�N�g��`�̊J�n
	if (!m_handler->OnStartObject())
	{
		// ���f
		m_error.SetError(ParseError_Termination, m_reader->GetPosition());
		return false;
	}

	SkipWhitespace();
	if (m_reader->Peek() == '}')
	{
		// �����o��1����������
		m_reader->Read();	// '}' �̎����w���Ă���
		if (!m_handler->OnEndObject(0))
		{
			// ���f
			m_error.SetError(ParseError_Termination, m_reader->GetPosition());
			return false;
		}
		return true;
	}

	int memberCount = 0;
	while (true)
	{
		// �ŏ��̓����o��
		if (m_reader->Peek() != '"')
		{
			// Error: �����o���̊J�n��������Ȃ�����
			m_error.SetError(ParseError_ObjectMissKeyStart, m_reader->GetPosition());
			return false;
		}
		if (!ParseString(true)) return false;
		SkipWhitespace();

		// ������ ':'
		if (m_reader->Read() != ':')
		{
			// Error: ':' ��������Ȃ�����
			m_error.SetError(ParseError_ObjectMissColon, m_reader->GetPosition());
			return false;
		}
		SkipWhitespace();

		// �Ō�ɒl
		if (!ParseValue()) return false;
		SkipWhitespace();

		++memberCount;

		switch (m_reader->Read())
		{
		case ',':
			SkipWhitespace();
			break;
		case '}':
			// �I�u�W�F�N�g��`�I�[
			if (!m_handler->OnEndObject(memberCount))
			{
				// ���f
				m_error.SetError(ParseError_Termination, m_reader->GetPosition());
				return false;
			}
			SkipWhitespace();
			return true;
		default:
			m_error.SetError(ParseError_ObjectMissCommaOrCurlyBracket, m_reader->GetPosition());
			return false;
		}
	}

	// �����ɂ͗��Ȃ��͂�
}

} // namespace Json
} // namespace Lumino
