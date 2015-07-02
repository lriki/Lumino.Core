
#pragma once

#include "../Internal.h"
#include <Lumino/Base/StringTraits.h>
#include <Lumino/IO/StringReader.h>
#include <Lumino/Json/JsonHandler.h>
#include <Lumino/Json/JsonReader.h>
#include <Lumino/Json/JsonDocument.h>

namespace Lumino
{
namespace Json
{

//=============================================================================
// JsonDocument
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonDocument::Parse(const TCHAR* text, int len)
{
	if (LN_VERIFY_ASSERT(text != NULL)) { return; }

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

} // namespace Json
} // namespace Lumino
