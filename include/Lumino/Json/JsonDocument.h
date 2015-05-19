
#pragma once

#include "../IO/TextReader.h"
#include "JsonValue.h"

namespace Lumino
{
namespace Json
{

/*
	@brief	JSON データのルート要素です。
*/
class JsonDocument
	: public JsonValue
{
public:

	/*
		@brief	指定した JSON 形式文字列を解析し、ドキュメントを構築します。
	*/
	void Parse(const TCHAR* text, int len = -1);

	/*
		@brief	指定した TextReader から JSON 形式文字列を解析し、ドキュメントを構築します。
	*/
	void Parse(TextReader* textReader);

	//void Load();

private:
};

} // namespace Json
} // namespace Lumino
