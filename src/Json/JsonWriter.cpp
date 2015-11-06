
#include "../Internal.h"
#include <Lumino/Base/StringTraits.h>
#include <Lumino/Json/JsonWriter.h>

LN_NAMESPACE_BEGIN

//=============================================================================
// JsonWriter
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JsonWriter::JsonWriter(TextWriter* textWriter)
	: m_textWriter(textWriter)
{
	LN_VERIFY_RETURN(m_textWriter != NULL);
	m_levelStack.Reserve(32);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JsonWriter::~JsonWriter()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::StartObject()
{
	WritePrefix();
	m_levelStack.Push(Level(false));
	OnStartObject();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::EndObject()
{
	LN_VERIFY_RETURN(m_levelStack.GetCount() >= 1);
	LN_VERIFY_RETURN(!m_levelStack.GetTop().inArray);

	m_levelStack.Pop();
	OnEndObject();

	if (!m_levelStack.IsEmpty()) {	// ルート要素のクローズに備える
		m_levelStack.GetTop().valueCount++;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::StartArray()
{
	WritePrefix();
	m_levelStack.Push(Level(true));
	OnStartArray();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::EndArray()
{
	LN_VERIFY_RETURN(m_levelStack.GetCount() >= 2);
	LN_VERIFY_RETURN(m_levelStack.GetTop().inArray);

	m_levelStack.Pop();
	OnEndArray();
	m_levelStack.GetTop().valueCount++;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::WriteKey(const TCHAR* str, int length)
{
	LN_VERIFY_RETURN(m_levelStack.GetCount() >= 1);
	length = (length <= -1) ? StringTraits::StrLen(str) : 0;

	WritePrefix();
	OnKey(str, length);
	m_levelStack.GetTop().justSawKey = true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::WriteNull()
{
	LN_VERIFY_RETURN(m_levelStack.GetCount() >= 1);

	WritePrefix();
	OnNull();
	m_levelStack.GetTop().valueCount++;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::WriteBool(bool value)
{
	LN_VERIFY_RETURN(m_levelStack.GetCount() >= 1);

	WritePrefix();
	OnBool(value);
	m_levelStack.GetTop().valueCount++;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::WriteDouble(double value)
{
	LN_VERIFY_RETURN(m_levelStack.GetCount() >= 1);

	WritePrefix();
	OnDouble(value);
	m_levelStack.GetTop().valueCount++;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::WriteString(const TCHAR* str, int length)
{
	LN_VERIFY_RETURN(m_levelStack.GetCount() >= 1);
	length = (length <= -1) ? StringTraits::StrLen(str) : 0;

	WritePrefix();
	OnString(str, length);
	m_levelStack.GetTop().valueCount++;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonWriter::IsComplete() const
{
	return m_levelStack.IsEmpty();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::WritePrefix()
{
	if (!m_levelStack.IsEmpty())
	{
		Level& level = m_levelStack.GetTop();
		if (level.justSawKey) {
			OnPrefix(PrefixType_Key, level.valueCount);
		}
		else if (level.valueCount > 0)
		{
			if (level.inArray) {
				OnPrefix(PrefixType_Array, level.valueCount);
			}
			else {
				OnPrefix(PrefixType_Object, level.valueCount);
			}
		}
		level.justSawKey = false;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::OnPrefix(PrefixType type, int valueCount)
{
	if (type == PrefixType_Array || type == PrefixType_Object) {
		if (valueCount > 0) {
			m_textWriter->Write(_T(','));
		}
	}
	else {
		m_textWriter->Write(_T(':'));
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::OnStartObject()
{
	m_textWriter->Write(_T('{'));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::OnEndObject()
{
	m_textWriter->Write(_T('}'));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::OnStartArray()
{
	m_textWriter->Write(_T('['));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::OnEndArray()
{
	m_textWriter->Write(_T(']'));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::OnKey(const TCHAR* str, int length)
{
	OnString(str, length);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::OnNull()
{
	m_textWriter->Write(_T("null"), 4);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::OnBool(bool value)
{
	if (value) {
		m_textWriter->Write(_T("true"), 4);
	}
	else {
		m_textWriter->Write(_T("false"), 5);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::OnDouble(double value)
{
	m_textWriter->Write(value);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonWriter::OnString(const TCHAR* str, int length)
{
	m_textWriter->Write(_T("\""), 1);
	m_textWriter->Write(str, length);
	m_textWriter->Write(_T("\""), 1);
}

LN_NAMESPACE_END
