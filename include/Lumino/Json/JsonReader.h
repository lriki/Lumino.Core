
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

	/** "null" トークン */
	//Null,

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

	/**
		@brief		現在のノードの種類を取得します。
	*/
	JsonToken GetTokenType() const;

	/**
		@brief		現在のノードの値 (文字列形式) を取得します。
		@details	値が無い場合は空文字列を返します。
	*/
	const String& GetValue() const;

private:
	enum class State
	{
		Start,
		Object,
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
		ContainerType		containerType = ContainerType::None;
	};

	RefPtr<TextReader>		m_reader;
	//State					m_currentState;
	ParserState				m_currentState;
	Token					m_currentToken;
	String					m_value;
	Array<TCHAR>			m_textCache;
	Stack<ParserState>		m_stateStack;

	JsonReader2();
	bool SkipWhitespace();
	bool ParseValue();
	bool ParseObject();
	bool TryParsePropertyName();
	bool ParseString(bool isKey);
	bool ParsePostValue(bool* outSkip);

	void SetToken(JsonToken newToken, bool hasValue);
	void PushState(/*ContainerType containerType*/);
	void PopState();
};

LN_NAMESPACE_END
