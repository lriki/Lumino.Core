
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

	/** "null" �g�[�N�� */
	//Null,

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

	/**
		@brief		���݂̃m�[�h�̎�ނ��擾���܂��B
	*/
	JsonToken GetTokenType() const;

	/**
		@brief		���݂̃m�[�h�̒l (������`��) ���擾���܂��B
		@details	�l�������ꍇ�͋󕶎����Ԃ��܂��B
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
		//int			valuePos;	// m_textCache ��̃C���f�b�N�X
		//int			valueLen;	// m_textCache ��̃C���f�b�N�X
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
