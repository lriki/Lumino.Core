
#pragma once
#include "../IO/TextReader.h"
#include "../IO/MemoryStream.h"
#include "JsonError.h"
#include "JsonHandler.h"

LN_NAMESPACE_BEGIN

/**
	@brief	SAX �X�^�C���� JSON �p�[�T�ł��B
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
	int				m_currentCharCount;	// �G���[�\���p
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

	// TODO: ReadLine �� ReadToEnd �� Read ����������Ύg����悤�ɂ���ׂ�
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

	/** �I�u�W�F�N�g��`�̊J�n ('{') */
	StartObject,

	/** �I�u�W�F�N�g��`�̏I�� ('}') */
	EndObject,

	/** �z���`�̊J�n ('[') */
	StartArray,

	/** �z���`�̏I�� (']') */
	EndArray,

	/** �I�u�W�F�N�g�̃v���p�e�B�̖��O */
	PropertyName,

	/** ���l�^ (double) */
	Double,

	/** "null" �l */
	Null,

	/** �^�U�l ("true" �܂��� "false"�BGetValue() �Œl���m�F���邱��) */
	Boolean,	// TODO: Bool (Xml �Ƃ��낦��H)

	/** ������l */
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
		@brief		���̃m�[�h�ǂݎ��܂��B
		@return		����ɓǂݎ��ꂽ�ꍇ�� true�B����ȏ�ǂݎ��m�[�h�����݂��Ȃ��ꍇ�� false�B
	*/
	bool Read();

	bool TryRead();

	/**
		@brief		���݂̃m�[�h�̎�ނ��擾���܂��B
	*/
	JsonToken GetTokenType() const;

	/**
		@brief		���݂̃m�[�h�̒l (������`��) ���擾���܂��B
		@details	�l�������ꍇ�͋󕶎����Ԃ��܂��B
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
		ObjectStart,	// { �̎��̏�ԁB���� } �ł��ǂ��B
		Object,			// , �̎��̏�ԁB���� } �ł̓_���B
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
		//int			valuePos;	// m_textCache ��̃C���f�b�N�X
		//int			valueLen;	// m_textCache ��̃C���f�b�N�X
	};

	struct ParserState
	{
		State				state = State::Start;
		String				propertyName;
		ContainerType		containerType = ContainerType::None;	// ���݉�͒��̃u���b�N�̎��
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
