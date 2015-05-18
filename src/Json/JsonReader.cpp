
#include "../Internal.h"
#include <Lumino/Base/StringUtils.h>
#include <Lumino/IO/MemoryStream.h>
#include <Lumino/Json/JsonReader.h>
#include "JsonInputStream.h"

namespace Lumino
{
namespace Json
{

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void JsonReader<TChar>::Parse(const TChar* text, int len = -1, Text::Encoding* encoding)
{
	LN_VERIFY(text != NULL) { return; }
	LN_VERIFY(encoding != NULL) { return; }

	MemoryStream stream;
	stream.Create(text, (len == -1) ? StringUtils::StrLen(text) : len);
	Parse(&stream, encoding);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void JsonReader<TChar>::Parse(Stream* inputStream, Text::Encoding* encoding)
{
	LN_VERIFY(inputStream != NULL) { return; }
	LN_VERIFY(encoding != NULL) { return; }

	// �ꎞ�o�b�t�@�͓��͂Ɠ����T�C�Y�m�ۂ��Ă����Ώ\���B
	// ��������Ȃ���Ίg�������B
	m_tmpStream.Create(inputStream->GetLength());

	InputStream<TChar> is(inputStream, encoding);

	// �o�b�t�@�擪�̋󔒂� BOM ��ǂݔ�΂�
	is.SkipWhitespace();

	if (is.IsEOF())
	{
		// Error: �o�b�t�@���󂾂���
		m_error.SetError(ParseError_DocumentEmpty, is.GetReadCharCount());
		return;
	}

	// ���[�g�v�f�̉��
	ParseValue(&is);

	// �o�b�t�@�I�[�̋󔒂�ǂݔ�΂�
	is.SkipWhitespace();

	if (is.IsEOF())
	{
		// Error: �����̃��[�g�v�f����������
		m_error.SetError(ParseError_DocumentRootNotSingular, is.GetReadCharCount());
		return;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void JsonReader<TChar>::ParseValue(InputStream* is)
{
	switch (is.Peek())
	{
		case 'n': ParseNull(is); break;				// null ��������Ȃ�
		case 't': ParseTrue(is); break;				// true ��������Ȃ�
		case 'f': ParseFalse(is); break;			// false ��������Ȃ�
		case '"': ParseString(is, false); break;	// �����񂩂�����Ȃ�
		case '[': ParseArray(is); break;			// �z�񂩂�����Ȃ�
		case '{': ParseObject(is); break;			// �I�u�W�F�N�g��������Ȃ�
		default: ParseNumber(is); break;			// ���l��������Ȃ�
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void JsonReader<TChar>::ParseNull(InputStream* is)
{
	is->Take();
	if (is.Take() == 'u' && is.Take() == 'l' && is.Take() == 'l') {
		if (!handler.OnNull()) {
			m_error.SetError(ParseError_Termination, is.GetReadCharCount());
		}
	}
	else {
		m_error.SetError(ParseError_ValueInvalid, is.GetReadCharCount() - 1);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void JsonReader<TChar>::ParseTrue(InputStream* is)
{
	is->Take();
	if (is.Take() == 'r' && is.Take() == 'u' && is.Take() == 'e') {
		if (!handler.OnBool(true)) {
			m_error.SetError(ParseError_Termination, is.GetReadCharCount());
		}
	}
	else {
		m_error.SetError(ParseError_ValueInvalid, is.GetReadCharCount() - 1);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void JsonReader<TChar>::ParseFalse(InputStream* is)
{
	is->Take();
	if (is.Take() == 'a' && is.Take() == 'l' && is.Take() == 's' && is.Take() == 'e') {
		if (!handler.OnBool(false)) {
			m_error.SetError(ParseError_Termination, is.GetReadCharCount());
		}
	}
	else {
		m_error.SetError(ParseError_ValueInvalid, is.GetReadCharCount() - 1);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void JsonReader<TChar>::ParseString(InputStream* is, bool isKey)
{
	// http://json.org/json-ja.html
	static const TChar escapeTable[256] =
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

	m_tmpStream.Seek(0, SeekOrigin_Begin);
	is->Take();	// skip "

	while (true)
	{
		TChar c = is.Peek();
		if (c == '\\')
		{
			is.Take();	// skip '\'
			TChar e = is.Take();
			// ��{�I�ȃG�X�P�[�v
			if ((sizeof(Ch) == 1 || unsigned(e) < 256) && escape[(unsigned char)e])
			{
				m_tmpStream.Write(&escapeTable[(unsigned char)e], sizeof(TChar));
			}
			// Unicode �G�X�P�[�v
			else if (e == 'u')
			{
				// ������
				m_error.SetError(ParseError_StringEscapeInvalid, is->GetReadCharCount() - 1);
				return;
			}
			else
			{
				// Error: �����ȃG�X�P�[�v
				m_error.SetError(ParseError_StringEscapeInvalid, is->GetReadCharCount() - 1);
				return;
			}
		}
		else if (c == '"') {    // Closing double quote
			is.Take();
			break;
		}
		else if (c == '\0') {
			// Error: " ����v���Ȃ�����
			m_error.SetError(ParseError_StringMissQuotationMark, is->GetReadCharCount() - 1);
			return;
		}
		else if ((unsigned)c < 0x20) {	// 0x20 �����̐��䕶���͎g���Ȃ�
			// RFC 4627: unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
			m_error.SetError(ParseError_StringEscapeInvalid, is->GetReadCharCount() - 1);
		}
		else
		{
			// ���ʂ̕���
			m_tmpStream.Write(&c, sizeof(TChar));
			is.Take();
		}
	}

	// �s�������`�F�b�N
	if (m_inputEncoding->Validate(m_tmpStream.GetBuffer(), m_tmpStream.GetPosition()))
	{
		m_handler->OnString((TChar*)m_tmpStream.GetBuffer(), m_tmpStream.GetPosition() / sizeof(TChar));
	}
	else
	{
		// Error: �ϊ��ł��Ȃ�����
		m_error.SetError(ParseError_StringInvalidEncoding, is->GetReadCharCount() - 1);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void JsonReader<TChar>::ParseArray(InputStream* is)
{
	is.Take();  // skip '['

	if (!m_handler->OnStartArray()) {
		// ���f
		m_error.SetError(ParseError_Termination, is->GetReadCharCount());
	}

	SkipWhitespace(is);

	if (is.Peek() == ']')
	{
		is.Take();	// ']' �̎����w���Ă���
		// ��z�񂾂���
		if (!m_handler->OnEndArray(0)) {
			// ���f
			m_error.SetError(ParseError_Termination, is->GetReadCharCount());
		}
		return;
	}

	size_t elementCount = 0;
	while (true)
	{
		if (!ParseValue(is)) {
			return false;	// �G���[�͏����ς�
		}

		++elementCount;
		SkipWhitespace(is);

		switch (is->Take())
		{
		case ',':
			SkipWhitespace(is);
			break;
		case ']':
			if (!m_handler->OnEndArray(elementCount)) {
				// ���f
				m_error.SetError(ParseError_Termination, is->GetReadCharCount());
				return false;
			}
			return true;
		default:
			m_error.SetError(ParseError_ArrayMissCommaOrSquareBracket, is->GetReadCharCount());
			return false;
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void JsonReader<TChar>::ParseObject(InputStream* is)
{
	is.Take();  // Skip '{'

	// �I�u�W�F�N�g��`�̊J�n
	if (!m_handler->StartObject())
	{
		// ���f
		m_error.SetError(ParseError_Termination, is->GetReadCharCount());
		return false;
	}

	SkipWhitespace(is);

	if (is->Peek() == '}')
	{
		is.Take();	// '}' �̎����w���Ă���
		// ��z�񂾂���
		if (!m_handler->OnEndObject(0))
		{
			// ���f
			m_error.SetError(ParseError_Termination, is->GetReadCharCount());
			return false;
		}
		return;
	}

	SizeType memberCount = 0;
	while (true)
	{
		// �ŏ��̓����o��
		if (is.Peek() != '"')
		{
			// Error: �����o���̊J�n��������Ȃ�����
			m_error.SetError(ParseError_ObjectMissKeyStart, is->GetReadCharCount());
		}
		if (!ParseString(is, true) return false;
		SkipWhitespace(is);

		// ������ ':'
		if (is.Take() != ':')
		{
			// Error: ':' ��������Ȃ�����
			m_error.SetError(kParseErrorObjectMissColon, is->GetReadCharCount());
		}
		SkipWhitespace(is);

		// �Ō�ɒl
		if (!ParseValue(is, handler)) return false;
		SkipWhitespace(is);

		// TODO: Handler �ɒʒm���Ȃ��́H

		++memberCount;

		switch (is->Take())
		{
		case ',':
			SkipWhitespace(is);
			break;
		case '}':
			// �I�u�W�F�N�g��`�I�[
			if (!handler.EndObject(memberCount))
			{
				// ���f
				m_error.SetError(ParseError_Termination, is->GetReadCharCount());
				return false;
			}
			return true;
		default:
			m_error.SetError(ParseError_ObjectMissCommaOrCurlyBracket, is->GetReadCharCount());
			return false;
		}
	}

	// �����ɂ͗��Ȃ��͂�
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void JsonReader<TChar>::ParseObject(InputStream* is)
{
}

} // namespace Json
} // namespace Lumino
