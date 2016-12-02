
#pragma once
#include "../IO/TextReader.h"
#include "JsonValue.h"

LN_NAMESPACE_BEGIN
namespace tr {

/**
	@brief	JSON �f�[�^�̃��[�g�v�f�ł��B
*/
class JsonDocument
	: public JsonValue
{
public:

	/*
		@brief	�w�肵�� JSON �`�����������͂��A�h�L�������g���\�z���܂��B
	*/
	void Parse(const String& text);

	/*
		@brief	�w�肵�� JSON �`�����������͂��A�h�L�������g���\�z���܂��B
	*/
	void Parse(const TCHAR* text, int len = -1);

	/*
		@brief	�w�肵�� TextReader ���� JSON �`�����������͂��A�h�L�������g���\�z���܂��B
	*/
	void Parse(TextReader* textReader);

	//void Load();

private:
};

} // namespace tr
LN_NAMESPACE_END
