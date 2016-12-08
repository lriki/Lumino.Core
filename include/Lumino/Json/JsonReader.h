
#pragma once
#include "../IO/TextReader.h"
#include "../IO/MemoryStream.h"
#include "Common.h"
#include "JsonDiagnostics.h"
#include "JsonHandler.h"

LN_NAMESPACE_BEGIN
namespace tr {

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

	const String& GetPropertyName() const;

	/** ���݂̃m�[�h�� Bool �ł���ꍇ�A�p�[�X���ʂ� bool �l���擾�ł��܂��B*/
	bool GetBoolValue() const;
	/** ���݂̃m�[�h�� Int32 �ł���ꍇ�A�p�[�X���ʂ� int32_t �l���擾�ł��܂��B*/
	int32_t GetInt32Value() const;
	/** ���݂̃m�[�h�� Int64 �ł���ꍇ�A�p�[�X���ʂ� int64_t �l���擾�ł��܂��B*/
	int64_t GetInt64Value() const;
	/** ���݂̃m�[�h�� Double �ł���ꍇ�A�p�[�X���ʂ� double �l���擾�ł��܂��B*/
	double GetDoubleValue() const;


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

	union 
	{
		bool	m_bool;
		int32_t	m_int32;
		int64_t	m_int64;
		double	m_double;

	} m_valueData;

	//RefPtr<TextReader>		m_reader;
	RefPtr<detail::PositioningTextReader>	m_reader;
	//State					m_currentState;
	ParserState				m_currentState;
	Token					m_currentToken;
	String					m_value;
	List<TCHAR>				m_textCache;
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

} // namespace tr
LN_NAMESPACE_END
