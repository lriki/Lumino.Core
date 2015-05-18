
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

	// 一時バッファは入力と同じサイズ確保しておけば十分。
	// もし足りなければ拡張される。
	m_tmpStream.Create(inputStream->GetLength());

	InputStream<TChar> is(inputStream, encoding);

	// バッファ先頭の空白と BOM を読み飛ばす
	is.SkipWhitespace();

	if (is.IsEOF())
	{
		// Error: バッファが空だった
		m_error.SetError(ParseError_DocumentEmpty, is.GetReadCharCount());
		return;
	}

	// ルート要素の解析
	ParseValue(&is);

	// バッファ終端の空白を読み飛ばす
	is.SkipWhitespace();

	if (is.IsEOF())
	{
		// Error: 複数のルート要素が見つかった
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
		case 'n': ParseNull(is); break;				// null かもしれない
		case 't': ParseTrue(is); break;				// true かもしれない
		case 'f': ParseFalse(is); break;			// false かもしれない
		case '"': ParseString(is, false); break;	// 文字列かもしれない
		case '[': ParseArray(is); break;			// 配列かもしれない
		case '{': ParseObject(is); break;			// オブジェクトかもしれない
		default: ParseNumber(is); break;			// 数値かもしれない
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
			// 基本的なエスケープ
			if ((sizeof(Ch) == 1 || unsigned(e) < 256) && escape[(unsigned char)e])
			{
				m_tmpStream.Write(&escapeTable[(unsigned char)e], sizeof(TChar));
			}
			// Unicode エスケープ
			else if (e == 'u')
			{
				// 未実装
				m_error.SetError(ParseError_StringEscapeInvalid, is->GetReadCharCount() - 1);
				return;
			}
			else
			{
				// Error: 無効なエスケープ
				m_error.SetError(ParseError_StringEscapeInvalid, is->GetReadCharCount() - 1);
				return;
			}
		}
		else if (c == '"') {    // Closing double quote
			is.Take();
			break;
		}
		else if (c == '\0') {
			// Error: " が一致しなかった
			m_error.SetError(ParseError_StringMissQuotationMark, is->GetReadCharCount() - 1);
			return;
		}
		else if ((unsigned)c < 0x20) {	// 0x20 未満の制御文字は使えない
			// RFC 4627: unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
			m_error.SetError(ParseError_StringEscapeInvalid, is->GetReadCharCount() - 1);
		}
		else
		{
			// 普通の文字
			m_tmpStream.Write(&c, sizeof(TChar));
			is.Take();
		}
	}

	// 不正文字チェック
	if (m_inputEncoding->Validate(m_tmpStream.GetBuffer(), m_tmpStream.GetPosition()))
	{
		m_handler->OnString((TChar*)m_tmpStream.GetBuffer(), m_tmpStream.GetPosition() / sizeof(TChar));
	}
	else
	{
		// Error: 変換できなかった
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
		// 中断
		m_error.SetError(ParseError_Termination, is->GetReadCharCount());
	}

	SkipWhitespace(is);

	if (is.Peek() == ']')
	{
		is.Take();	// ']' の次を指しておく
		// 空配列だった
		if (!m_handler->OnEndArray(0)) {
			// 中断
			m_error.SetError(ParseError_Termination, is->GetReadCharCount());
		}
		return;
	}

	size_t elementCount = 0;
	while (true)
	{
		if (!ParseValue(is)) {
			return false;	// エラーは処理済み
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
				// 中断
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

	// オブジェクト定義の開始
	if (!m_handler->StartObject())
	{
		// 中断
		m_error.SetError(ParseError_Termination, is->GetReadCharCount());
		return false;
	}

	SkipWhitespace(is);

	if (is->Peek() == '}')
	{
		is.Take();	// '}' の次を指しておく
		// 空配列だった
		if (!m_handler->OnEndObject(0))
		{
			// 中断
			m_error.SetError(ParseError_Termination, is->GetReadCharCount());
			return false;
		}
		return;
	}

	SizeType memberCount = 0;
	while (true)
	{
		// 最初はメンバ名
		if (is.Peek() != '"')
		{
			// Error: メンバ名の開始が見つからなかった
			m_error.SetError(ParseError_ObjectMissKeyStart, is->GetReadCharCount());
		}
		if (!ParseString(is, true) return false;
		SkipWhitespace(is);

		// 続いて ':'
		if (is.Take() != ':')
		{
			// Error: ':' が見つからなかった
			m_error.SetError(kParseErrorObjectMissColon, is->GetReadCharCount());
		}
		SkipWhitespace(is);

		// 最後に値
		if (!ParseValue(is, handler)) return false;
		SkipWhitespace(is);

		// TODO: Handler に通知しないの？

		++memberCount;

		switch (is->Take())
		{
		case ',':
			SkipWhitespace(is);
			break;
		case '}':
			// オブジェクト定義終端
			if (!handler.EndObject(memberCount))
			{
				// 中断
				m_error.SetError(ParseError_Termination, is->GetReadCharCount());
				return false;
			}
			return true;
		default:
			m_error.SetError(ParseError_ObjectMissCommaOrCurlyBracket, is->GetReadCharCount());
			return false;
		}
	}

	// ここには来ないはず
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
