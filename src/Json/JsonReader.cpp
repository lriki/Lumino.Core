
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
	static const char escapeTale[256] =
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
	}
}



} // namespace Json
} // namespace Lumino
