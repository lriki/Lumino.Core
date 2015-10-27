
#include "../Internal.h"
#include <Lumino/Base/StringTraits.h>
#include <Lumino/IO/StringReader.h>
#include <Lumino/Json/JsonHandler.h>
#include <Lumino/Json/JsonReader.h>
#include <Lumino/Json/JsonDocument.h>

LN_NAMESPACE_BEGIN

//=============================================================================
// JsonDocument
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonDocument::Parse(const TCHAR* text, int len)
{
	LN_CHECK_ARGS_RETURN(text != NULL);

	StringReader textReader(String(text, (len < 0) ? StringTraits::StrLen(text) : len));
	Parse(&textReader);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonDocument::Parse(TextReader* textReader)
{
	JsonDOMHandler handler(this);
	JsonReader reader(&handler);
	reader.Parse(textReader);
	handler.Build();
}

LN_NAMESPACE_END
