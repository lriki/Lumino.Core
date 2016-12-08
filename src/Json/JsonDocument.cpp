
#include "../Internal.h"
#include <Lumino/Base/String.h>
#include <Lumino/IO/StringReader.h>
#include <Lumino/IO/StreamWriter.h>
#include <Lumino/Json/JsonHandler.h>
#include <Lumino/Json/JsonReader.h>
#include <Lumino/Json/JsonWriter.h>
#include <Lumino/Json/JsonDocument.h>

LN_NAMESPACE_BEGIN
namespace tr {

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
	LN_CHECK_ARG(text != nullptr);

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




//==============================================================================
// JsonElement2
//==============================================================================
//------------------------------------------------------------------------------
JsonElement2::JsonElement2()
	: m_type(JsonValueType::Null)
{
}

//------------------------------------------------------------------------------
JsonElement2::~JsonElement2()
{
}


//==============================================================================
// JsonObject2
//==============================================================================
//------------------------------------------------------------------------------
JsonValue2::JsonValue2(JsonDocument2* ownerDoc)
{
	SetOwnerDocument(ownerDoc);
}

//------------------------------------------------------------------------------
JsonValue2::~JsonValue2()
{
	CheckRelease();
}

//------------------------------------------------------------------------------
void JsonValue2::SetNull()
{
	CheckRelease();
	SetType(JsonValueType::Null);
}

//------------------------------------------------------------------------------
void JsonValue2::SetBool(bool value)
{
	CheckRelease();
	SetType(JsonValueType::Bool);
	m_bool = value;
}

//------------------------------------------------------------------------------
void JsonValue2::SetInt32(int32_t value)
{
	CheckRelease();
	SetType(JsonValueType::Int32);
	m_int32 = value;
}

//------------------------------------------------------------------------------
void JsonValue2::SetInt64(int64_t value)
{
	CheckRelease();
	SetType(JsonValueType::Int64);
	m_int64 = value;
}

//------------------------------------------------------------------------------
void JsonValue2::SetFloat(float value)
{
	CheckRelease();
	SetType(JsonValueType::Float);
	m_float = value;
}

//------------------------------------------------------------------------------
void JsonValue2::SetDouble(double value)
{
	CheckRelease();
	SetType(JsonValueType::Double);
	m_double = value;
}

//------------------------------------------------------------------------------
void JsonValue2::SetString(const StringRef& value)
{
	CheckRelease();
	SetType(JsonValueType::String);
	m_stringCore = LN_NEW ln::detail::GenericStringCore<TCHAR>();
	m_stringCore->assign(value.GetBegin(), value.GetLength());
}

//------------------------------------------------------------------------------
bool JsonValue2::IsNull() const
{
	return GetType() == JsonValueType::Null;
}

//------------------------------------------------------------------------------
bool JsonValue2::GetBool() const
{
	LN_FAIL_CHECK_STATE(GetType() == JsonValueType::Bool) return false;
	return m_bool;
}

//------------------------------------------------------------------------------
int32_t JsonValue2::GetInt32() const
{
	LN_FAIL_CHECK_STATE(GetType() == JsonValueType::Int32) return false;
	return m_int32;
}

//------------------------------------------------------------------------------
int64_t JsonValue2::GetInt64() const
{
	LN_FAIL_CHECK_STATE(GetType() == JsonValueType::Int64) return false;
	return m_int64;
}

//------------------------------------------------------------------------------
float JsonValue2::GetFloat() const
{
	LN_FAIL_CHECK_STATE(GetType() == JsonValueType::Float) return false;
	return m_float;
}

//------------------------------------------------------------------------------
double JsonValue2::GetDouble() const
{
	LN_FAIL_CHECK_STATE(GetType() == JsonValueType::Double) return false;
	return m_double;
}

//------------------------------------------------------------------------------
String JsonValue2::GetString() const
{
	String str;
	ln::detail::StringHelper::AttachStringCore(&str, m_stringCore);
	return str;
}

//------------------------------------------------------------------------------
void JsonValue2::CheckRelease()
{
	if (GetType() == JsonValueType::String)
	{
		m_stringCore->Release();
	}
	SetType(JsonValueType::Null);
}

//------------------------------------------------------------------------------
void JsonValue2::OnSave(JsonWriter* writer)
{
	LN_FAIL_CHECK_ARG(writer != nullptr) return;
	switch (GetType())
	{
		case JsonValueType::Null:
			writer->WriteNull();
			break;
		case JsonValueType::Bool:
			writer->WriteBool(m_bool);
			break;
		case JsonValueType::Int32:
			writer->WriteInt32(m_int32);
			break;
		case JsonValueType::Int64:
			writer->WriteInt64(m_int64);
			break;
		case JsonValueType::Float:
			writer->WriteFloat(m_float);
			break;
		case JsonValueType::Double:
			writer->WriteDouble(m_double);
			break;
		case JsonValueType::String:
			writer->WriteString(m_stringCore->c_str(), m_stringCore->length());
			break;
		default:
			LN_FAIL_CHECK_STATE(0);
			break;
	}
}


//==============================================================================
// JsonObject2
//==============================================================================

//------------------------------------------------------------------------------
JsonObject2::JsonObject2(JsonDocument2* ownerDoc)
{
	SetOwnerDocument(ownerDoc);
}

//------------------------------------------------------------------------------
JsonObject2::~JsonObject2()
{
}

//------------------------------------------------------------------------------
void JsonObject2::AddMemberBool(const StringRef& name, bool value)
{
	GetValue(name)->SetBool(value);
}

//------------------------------------------------------------------------------
void JsonObject2::AddMemberInt32(const StringRef& name, int32_t value)
{
	GetValue(name)->SetInt32(value);
}

//------------------------------------------------------------------------------
void JsonObject2::AddMemberInt64(const StringRef& name, int64_t value)
{
	GetValue(name)->SetInt64(value);
}

//------------------------------------------------------------------------------
void JsonObject2::AddMemberFloat(const StringRef& name, float value)
{
	GetValue(name)->SetFloat(value);
}

//------------------------------------------------------------------------------
void JsonObject2::AddMemberDouble(const StringRef& name, double value)
{
	GetValue(name)->SetDouble(value);
}

//------------------------------------------------------------------------------
void JsonObject2::AddMemberString(const StringRef& name, const StringRef& value)
{
	GetValue(name)->SetString(value);
}

//------------------------------------------------------------------------------
JsonObject2* JsonObject2::AddMemberObject(const StringRef& name)
{
	return GetObject(name);
}

//------------------------------------------------------------------------------
void JsonObject2::OnSave(JsonWriter* writer)
{
	LN_FAIL_CHECK_ARG(writer != nullptr) return;

	writer->WriteStartObject();

	for (auto& m : m_memberList)
	{
		writer->WritePropertyName(m.name.c_str(), m.name.GetLength());
		m.value->Save(writer);
	}

	writer->WriteEndObject();
}

//------------------------------------------------------------------------------
bool JsonObject2::IsValueType(JsonValueType type)
{
	return
		type == JsonValueType::Bool ||
		type == JsonValueType::Int32 ||
		type == JsonValueType::Int64 ||
		type == JsonValueType::Float ||
		type == JsonValueType::Double ||
		type == JsonValueType::String;
}

//------------------------------------------------------------------------------
JsonValue2* JsonObject2::GetValue(const StringRef& name)
{
	Member* m = m_memberList.Find([name](const Member& m) { return m.name == name; });
	if (m == nullptr || !IsValueType(m->value->GetType()))
	{
		JsonValue2* ptr = GetOwnerDocument()->NewElement<JsonValue2>();
		m_memberList.Add({ name, ptr });
		return ptr;
	}
	return static_cast<JsonValue2*>(m->value);
}

//------------------------------------------------------------------------------
JsonObject2* JsonObject2::GetObject(const StringRef& name)
{
	Member* m = m_memberList.Find([name](const Member& m) { return m.name == name; });
	if (m == nullptr || m->value->GetType() != JsonValueType::Object)
	{
		JsonObject2* ptr = GetOwnerDocument()->NewElement<JsonObject2>();
		m_memberList.Add({ name, ptr });
		return ptr;
	}
	return static_cast<JsonObject2*>(m->value);
}


//==============================================================================
// JsonElementCache
//==============================================================================
namespace detail {

//------------------------------------------------------------------------------
void JsonElementCache::Initialize()
{
	BufferInfo info;
	info.buffer.Resize(2048);
	info.used = 0;
	m_buffers.Add(info);
}

//------------------------------------------------------------------------------
void* JsonElementCache::Alloc(size_t size)
{
	LN_FAIL_CHECK_ARG(size <= BufferSize) return nullptr;

	BufferInfo* cur = &m_buffers.GetLast();
	if (cur->buffer.GetSize() - cur->used < size)
	{
		BufferInfo info;
		info.buffer.Resize(2048);
		info.used = 0;
		m_buffers.Add(info);
		cur = &m_buffers.GetLast();
	}

	void* buf = cur->buffer.GetData() + cur->used;
	cur->used += size;
	return buf;
}

} // namespace detail

//==============================================================================
// JsonDocument2
//==============================================================================

//------------------------------------------------------------------------------
JsonDocument2::JsonDocument2()
	: JsonObject2(this)
{
	m_cache.Initialize();
}

//------------------------------------------------------------------------------
JsonDocument2::~JsonDocument2()
{
	// m_cache 削除前にクリアする必要がある
	Finalize();
}

////------------------------------------------------------------------------------
//void JsonDocument2::Parse(const String& text)
//{
//	StringReader textReader(text);
//	Parse(&textReader);
//}
//
////------------------------------------------------------------------------------
//void JsonDocument2::Parse(const TCHAR* text, int len)
//{
//	LN_FAIL_CHECK_ARG(text != nullptr) return;
//
//	StringReader textReader(String(text, (len < 0) ? (int)StringTraits::tcslen(text) : len));
//	Parse(&textReader);
//}
//
////------------------------------------------------------------------------------
//void JsonDocument2::Parse(TextReader* textReader)
//{
//	LN_FAIL_CHECK_ARG(textReader != nullptr) return;
//
//	JsonReader2 reader(textReader);
//	reader.Read();
//}


//------------------------------------------------------------------------------
void JsonDocument2::Save(const StringRef& filePath)
{
	StreamWriter w(filePath);
	JsonWriter jw(&w);
	JsonElement2::Save(&jw);
}

} // namespace tr
LN_NAMESPACE_END
