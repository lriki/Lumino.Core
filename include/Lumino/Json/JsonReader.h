
#pragma once

#include <Lumino/Text/Encoding.h>
#include "../IO/Stream.h"
#include "JsonError.h"

namespace Lumino
{
namespace Json
{

/*
	@brief	SAX スタイルの JSON パーサです。
*/
template<typename TChar>
class JsonReader
{
public:
	JsonReader();
	~JsonReader();

public:
	void Parse(const TChar* text, int len = -1, Text::Encoding* encoding = Text::Encoding::GetEncodingTemplate<TChar>());
	void Parse(Stream* inputStream, Text::Encoding* encoding);

private:
	JsonError		m_error;
	JsonHandler*	m_handler;
	Text::Encoding*	m_inputEncoding;
	MemoryStream	m_tmpStream;
};

} // namespace Json
} // namespace Lumino
