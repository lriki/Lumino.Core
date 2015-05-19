
#pragma once

#include "../IO/TextReader.h"
#include "JsonValue.h"

namespace Lumino
{
namespace Json
{

/*
	@brief	JSON �f�[�^�̃��[�g�v�f�ł��B
*/
class JsonDocument
	: public JsonValue
{
public:

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

} // namespace Json
} // namespace Lumino
