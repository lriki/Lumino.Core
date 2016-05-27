
#include "../Internal.h"
#include <Lumino/Base/String.h>
#include <Lumino/IO/StringReader.h>
#include <Lumino/Json/JsonHandler.h>
#include <Lumino/Json/JsonReader.h>
#include <Lumino/Json/JsonDocument.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// JsonDocument
//==============================================================================

//------------------------------------------------------------------------------
void JsonDocument::Parse(const String& text)
{
	StringReader textReader(text);
	Parse(&textReader);
}
void JsonDocument::Parse(const TCHAR* text, int len)
{
	LN_CHECK_ARG(text != NULL);

	StringReader textReader(String(text, (len < 0) ? (int)StringTraits::tcslen(text) : len));
	Parse(&textReader);
}

//------------------------------------------------------------------------------
void JsonDocument::Parse(TextReader* textReader)
{
	JsonDOMHandler handler(this);
	JsonReader reader(&handler);
	reader.Parse(textReader);
	handler.Build();
}

LN_NAMESPACE_END
