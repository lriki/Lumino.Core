
#pragma once
#include "../Base/Stack.h"
#include "../IO/BinaryReader.h"
#include "../IO/BinaryWriter.h"
#include "../IO/MemoryStream.h"

LN_NAMESPACE_BEGIN
class JsonDocument;
class JsonValue;
class JsonMember;

/**
	@brief	SAX �X�^�C���� JSON �p�[�T����ʒm���󂯎��C���^�[�t�F�C�X�ł��B
*/
class JsonHandler
{
public:
	virtual ~JsonHandler();

public:
	virtual bool OnNull() = 0;
	virtual bool OnBool(bool value) = 0;
	virtual bool OnDouble(double value) = 0;
	virtual bool OnString(const TCHAR* str, int len) = 0;
	virtual bool OnStartArray() = 0;
	virtual bool OnEndArray(int elementCount) = 0;
	virtual bool OnStartObject() = 0;
	virtual bool OnKey(const TCHAR* str, int len) = 0;
	virtual bool OnEndObject(int memberCount) = 0;
};

/**
	@brief	JsonDocument ���\�z���邽�߂� JsonHandler �̎����ł��B
*/
class JsonDOMHandler
	: public JsonHandler
{
public:
	JsonDOMHandler(JsonDocument* document);
	virtual ~JsonDOMHandler();

public:
	virtual bool OnNull();
	virtual bool OnBool(bool value);
	virtual bool OnDouble(double value);
	virtual bool OnString(const TCHAR* str, int len);
	virtual bool OnStartArray();
	virtual bool OnEndArray(int elementCount);
	virtual bool OnStartObject();
	virtual bool OnKey(const TCHAR* str, int len);
	virtual bool OnEndObject(int memberCount);

	void Build();	// Parse ���I�������ɌĂяo��

private:
	void BuildValue(BinaryReader* reader, JsonValue* v);
	void BuildMember(BinaryReader* reader, JsonMember* m);

private:
	JsonDocument*	m_document;
	MemoryStream	m_valueRawData;
	BinaryWriter	m_writer;
	Stack<size_t>	m_startIndexStack;
};

LN_NAMESPACE_END
