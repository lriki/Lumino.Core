
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

	// 一時バッファ。もし足りなければ拡張される
	m_tmpStream.Create(512);

	// バッファ先頭の空白を読み飛ばす
	if (!SkipWhitespace())
	{
		// Error: バッファが空だった
		m_error.SetError(ParseError_DocumentEmpty, m_reader->GetPosition());
		return;
	}

	// ルート要素の解析
	if (!ParseValue()) {
		return;
	}

	// バッファ終端の空白を読み飛ばす
	if (SkipWhitespace())
	{
		// Error: 複数のルート要素が見つかった
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
		case 'n': return ParseNull();			// null かもしれない
		case 't': return ParseTrue();			// true かもしれない
		case 'f': return ParseFalse();			// false かもしれない
		case '"': return ParseString(false);	// 文字列かもしれない
		case '[': return ParseArray();			// 配列かもしれない
		case '{': return ParseObject();			// オブジェクトかもしれない
		default: return ParseNumber();			// 数値かもしれない
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
			// 中断
			m_error.SetError(ParseError_Termination, m_reader->GetPosition());
			return false;
		}
	}
	else
	{
		// Error: "null" ではなかった
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
			// 中断
			m_error.SetError(ParseError_Termination, m_reader->GetPosition());
			return false;
		}
	}
	else
	{
		// Error: "true" ではなかった
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
			// 中断
			m_error.SetError(ParseError_Termination, m_reader->GetPosition());
			return false;
		}
	}
	else
	{
		// Error: "false" ではなかった
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
	// 数値への変換には strtod を使用する。そのため、まずは数値扱いできる文字を全て読み取る
	m_tmpStream.Seek(0, SeekOrigin_Begin);
	int len = 0;
	TCHAR ch;
	while (true)
	{
		ch = m_reader->Peek();	// 読むだけ。ポインタは進めない
		if (ch == '.' ||
			('0' <= ch && ch <= '9') ||
			(ch == 'e' || ch == 'E') ||
			(ch == '+' || ch == '-'))
		{
			m_tmpStream.Write(&ch, sizeof(TCHAR));
			++len;
			m_reader->Read();	// ここで1つ進める
		}
		else {
			break;				// 一致しなければポインタは進めない
		}
	}
	if (len == 0)
	{
		// Error: 数値っぽい文字が見つからなかった
		m_error.SetError(ParseError_NumberInvalid, m_reader->GetPosition());
		return false;
	}
	ch = '\0';
	m_tmpStream.Write(&ch, sizeof(TCHAR));	// 終端 \0

	// double へ変換する
	TCHAR* str = (TCHAR*)m_tmpStream.GetBuffer();
	TCHAR* endptr = NULL;
	NumberConversionResult result;
	double value = StringUtils::ToDouble(str, len, &endptr, &result);
	if ((endptr - str) != len)	// 正常に変換できていれば、読み取った文字数が全て消費されるはず
	{
		// Error: 構文が正しくない
		m_error.SetError(ParseError_NumberInvalid, m_reader->GetPosition());
		return false;
	}
	if (result == NumberConversionResult_Overflow) {
		// Error: オーバーフローが発生した
		m_error.SetError(ParseError_NumberOverflow, m_reader->GetPosition());
		return false;
	}

	// Handler に通知する
	if (!m_handler->OnDouble(value))
	{
		// 中断
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

	// 1つずつ読んだ文字を格納していく一時バッファ。シーク位置を先頭に戻しておく
	m_tmpStream.Seek(0, SeekOrigin_Begin);

	m_reader->Read();	// skip '"'
	while (true)
	{
		TCHAR c = m_reader->Peek();

		// エスケープシーケンス
		if (c == '\\')
		{
			m_reader->Read();	// skip '\'
			TCHAR esc = m_reader->Read();
			// 基本的なエスケープ
			if (unsigned(esc) < 256 && escapeTable[(unsigned char)esc])
			{
				m_tmpStream.Write(&escapeTable[(unsigned char)esc], sizeof(TCHAR));
			}
			// Unicode エスケープ
			else if (esc == 'u')
			{
				// 未実装
				m_error.SetError(ParseError_StringEscapeInvalid, m_reader->GetPosition());
				return false;
			}
			else
			{
				// Error: 無効なエスケープ
				m_error.SetError(ParseError_StringEscapeInvalid, m_reader->GetPosition());
				return false;
			}
		}
		// 文字列終端
		else if (c == '"')
		{
			m_reader->Read();	// skip '"'
			break;
		}
		// 文字列の途中でバッファが切れた
		else if (m_reader->IsEOF() || c == '\0')
		{
			// Error: " が一致しなかった
			m_error.SetError(ParseError_StringMissQuotationMark, m_reader->GetPosition());
			return false;
		}
		// 0x20 未満の制御文字は使えない
		else if ((unsigned)c < 0x20) {
			// RFC 4627: unescaped = %x20-21 / %x23-5B / %x5D-10FFFF
			m_error.SetError(ParseError_StringEscapeInvalid, m_reader->GetPosition());
			return false;
		}
		// 普通の文字
		else
		{
			m_tmpStream.Write(&c, sizeof(TCHAR));
			m_reader->Read();
		}
	}

	// Handler に通知
	bool cont = false;
	if (isKey) {
		cont = m_handler->OnKey((TCHAR*)m_tmpStream.GetBuffer(), ((int)m_tmpStream.GetPosition()) / sizeof(TCHAR));
	}
	else {
		cont = m_handler->OnString((TCHAR*)m_tmpStream.GetBuffer(), ((int)m_tmpStream.GetPosition()) / sizeof(TCHAR));
	}
	if (!cont)
	{
		// 中断
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

	// Handler に Array の開始を通知
	if (!m_handler->OnStartArray())
	{
		// 中断
		m_error.SetError(ParseError_Termination, m_reader->GetPosition());
		return false;
	}

	SkipWhitespace();
	if (m_reader->Peek() == ']')
	{
		// 空配列だった。Array の終了を通知する
		m_reader->Read(); 	// ']' の次を指しておく
		if (!m_handler->OnEndArray(0))
		{
			// 中断
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
			return false;	// エラーは処理済み
		}

		++elementCount;
		SkipWhitespace();

		switch (m_reader->Read())
		{
		case ',':
			SkipWhitespace();
			break;
		case ']':
			// 配列の終端
			if (!m_handler->OnEndArray(elementCount))
			{
				// 中断
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

	// ここに来ることはない
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonReader::ParseObject()
{
	m_reader->Read();  // Skip '{'

	// オブジェクト定義の開始
	if (!m_handler->OnStartObject())
	{
		// 中断
		m_error.SetError(ParseError_Termination, m_reader->GetPosition());
		return false;
	}

	SkipWhitespace();
	if (m_reader->Peek() == '}')
	{
		// メンバが1つも無かった
		m_reader->Read();	// '}' の次を指しておく
		if (!m_handler->OnEndObject(0))
		{
			// 中断
			m_error.SetError(ParseError_Termination, m_reader->GetPosition());
			return false;
		}
		return true;
	}

	int memberCount = 0;
	while (true)
	{
		// 最初はメンバ名
		if (m_reader->Peek() != '"')
		{
			// Error: メンバ名の開始が見つからなかった
			m_error.SetError(ParseError_ObjectMissKeyStart, m_reader->GetPosition());
			return false;
		}
		if (!ParseString(true)) return false;
		SkipWhitespace();

		// 続いて ':'
		if (m_reader->Read() != ':')
		{
			// Error: ':' が見つからなかった
			m_error.SetError(ParseError_ObjectMissColon, m_reader->GetPosition());
			return false;
		}
		SkipWhitespace();

		// 最後に値
		if (!ParseValue()) return false;
		SkipWhitespace();

		++memberCount;

		switch (m_reader->Read())
		{
		case ',':
			SkipWhitespace();
			break;
		case '}':
			// オブジェクト定義終端
			if (!m_handler->OnEndObject(memberCount))
			{
				// 中断
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

	// ここには来ないはず
}

} // namespace Json
} // namespace Lumino
