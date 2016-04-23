
#pragma once
#include "../IO/TextReader.h"
#include "../IO/MemoryStream.h"
#include "JsonError.h"
#include "JsonHandler.h"

LN_NAMESPACE_BEGIN

/**
	@brief	SAX スタイルの JSON パーサです。
	@note	RFC 4627
			https://www.ietf.org/rfc/rfc4627.txt
*/
class JsonReader
{
public:
	JsonReader(JsonHandler* handler);
	~JsonReader();

public:
	void Parse(const String& text);
	void Parse(const TCHAR* text, int len = -1);
	void Parse(TextReader* textReader);
	bool HasError() const { return m_error.ErrorCode != JsonParseError::NoError; }
	const JsonError& GetError() const { return m_error; }

private:
	bool SkipWhitespace();
	bool ParseValue();
	bool ParseNull();
	bool ParseTrue();
	bool ParseFalse();
	bool ParseNumber();
	bool ParseString(bool isKey);
	bool ParseArray();
	bool ParseObject();

private:
	JsonError		m_error;
	JsonHandler*	m_handler;
	TextReader*		m_reader;
	MemoryStream	m_tmpStream;
	int				m_currentCharCount;	// エラー表示用
};









namespace detail
{

class PositioningTextReader
	: public TextReader
{
public:
	PositioningTextReader(TextReader* innter)
	{
		m_innter = innter;
		m_pos = 0;
		m_line = 0;
		m_column = 0;
		m_lastCR = false;
		//m_matched = 0;
	}

	virtual int Peek() override
	{
		return m_innter->Peek();
	}

	virtual int Read() override
	{
		int c = m_innter->Read();
		if (c >= 0) {
			AdvancePosition((TCHAR)c);
		}
		return c;
	}

	// TODO: ReadLine と ReadToEnd は Read を実装すれば使えるようにするべき
	virtual bool ReadLine(String* line) override
	{
		LN_THROW(0, NotImplementedException);
		//bool r = m_innter->ReadLine(line);
		//++m_line;
		//m_column = 0;
		//return r;
	}

	virtual String ReadToEnd() override
	{
		LN_THROW(0, NotImplementedException);
	}

	virtual bool IsEOF() override
	{
		return m_innter->IsEOF();
	}

	int GetPosition() const
	{
		return m_pos;
	}

	int GetLineNumber() const
	{
		return m_line;
	}

	int GetColumnNumber() const
	{
		return m_column;
	}

private:

	void AdvancePosition(TCHAR ch)
	{
		++m_pos;
		if (m_lastCR)
		{
			m_lastCR = false;
			if (ch == '\n')
			{
				return;
			}
		}
		
		if (ch == '\r' || ch == '\n')
		{
			++m_line;
			m_column = 0;
			if (ch == '\r') {
				m_lastCR = true;
			}
			return;
		}

		++m_column;

		//if (m_lastCR)
		//{
		//	if (ch == '/n')
		//	{
		//		/* CRLF */
		//		++m_line;
		//		m_column = 0;
		//		m_lastCR = false;
		//		return;
		//	}
		//	else
		//	{
		//		/* CR */

		//	}
		//}
		//if (ch == '/r')
		//{
		//	m_lastCR = true;
		//	return;
		//}
		//else if (ch == '/n')
		//{

		//}
	}

private:
	RefPtr<TextReader>	m_innter;
	int					m_pos;
	int					m_line;
	int					m_column;
	bool				m_lastCR;
	//int					m_matched;
};

}



enum class JsonToken
{
	None = 0,

	/** オブジェクト定義の開始 ('{') */
	StartObject,

	/** オブジェクト定義の終了 ('}') */
	EndObject,

	/** 配列定義の開始 ('[') */
	StartArray,

	/** 配列定義の終了 (']') */
	EndArray,

	/** オブジェクトのプロパティの名前 */
	PropertyName,

	/** 数値型 (double) */
	Double,

	/** "null" 値 */
	Null,

	/** 真偽値 ("true" または "false"。GetValue() で値を確認すること) */
	Boolean,	// TODO: Bool (Xml とそろえる？)

	/** 文字列値 */
	String,
};

class JsonReader2
{
public:
	JsonReader2(const String& text);
	JsonReader2(TextReader* textReader);
	~JsonReader2();

public:
	
	/**
		@brief		次のノード読み取ります。
		@return		正常に読み取られた場合は true。それ以上読み取るノードが存在しない場合は false。
	*/
	bool Read();

	bool TryRead();

	/**
		@brief		現在のノードの種類を取得します。
	*/
	JsonToken GetTokenType() const;

	/**
		@brief		現在のノードの値 (文字列形式) を取得します。
		@details	値が無い場合は空文字列を返します。
	*/
	const String& GetValue() const;

	const JsonError2& GetError() const;

	void ReadAsStartObject();
	void ReadAsEndObject();
	void ReadAsStartArray();
	void ReadAsEndArray();
	const String& ReadAsPropertyName();
	bool ReadAsBool();
	const String& ReadAsString();

private:
	enum class ResultState
	{
		Success = 0,
		Error = 1,
	};

	enum class State
	{
		Start,
		ObjectStart,	// { の次の状態。次は } でも良い。
		Object,			// , の次の状態。次は } ではダメ。
		ArrayStart,
		Array,
		Property,
		PostValue,
	};

	enum class ContainerType
	{
		None,
		Object,
		Array,
	};

	struct Token
	{
		JsonToken	type;
		//int			valuePos;	// m_textCache 上のインデックス
		//int			valueLen;	// m_textCache 上のインデックス
	};

	struct ParserState
	{
		State				state = State::Start;
		String				propertyName;
		ContainerType		containerType = ContainerType::None;	// 現在解析中のブロックの種類
	};

	//RefPtr<TextReader>		m_reader;
	RefPtr<detail::PositioningTextReader>	m_reader;
	//State					m_currentState;
	ParserState				m_currentState;
	Token					m_currentToken;
	String					m_value;
	Array<TCHAR>			m_textCache;
	Stack<ParserState>		m_stateStack;
	JsonError2				m_error;

	JsonReader2();
	bool SkipWhitespace();
	bool ParseValue();
	bool ParseNull();
	bool ParseTrue();
	bool ParseFalse();
	bool ParseNumber();
	bool ParseObject();
	bool TryParsePropertyName();
	bool ParseString(bool isKey);
	bool ParsePostValue(bool* outSkip);

	bool SetToken(JsonToken newToken, const TCHAR* value = nullptr, int valueLen = 0);
	void PushState(/*ContainerType containerType*/);
	void PopState();
	void SetError(JsonParseError2 code, const String& message = String::GetEmpty());
};

LN_NAMESPACE_END
