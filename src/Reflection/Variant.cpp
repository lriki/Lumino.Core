
#include "../Internal.h"
#include <Lumino/Reflection/ReflectionObject.h>
#include <Lumino/Reflection/ReflectionArrayObject.h>
#include <Lumino/Reflection/Variant.h>

LN_NAMESPACE_BEGIN
namespace tr
{

//=============================================================================
// Variant
//=============================================================================

const Variant Variant::Null;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Variant::Variant()
	: m_type(VariantType::Null)
	, m_object(nullptr)
	, m_structSize(0)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Variant::SetBool(bool value)
{
	Release();
	m_type = VariantType::Bool;
	m_bool = value;
}
bool Variant::GetBool() const
{
	LN_CHECK_STATE_RETURNV(m_type == VariantType::Bool, false);
	return m_bool;
}
void Variant::SetString(const String& value)
{
	Release();
	m_type = VariantType::String;
	m_string = value;
}
const String& Variant::GetString() const
{
	LN_CHECK_STATE_RETURNV(m_type == VariantType::String, String::GetEmpty());
	return m_string;
}
void Variant::SetEnumValue(EnumValueType value)
{
	Release();
	m_type = VariantType::Enum;
	m_enum = value;
}
EnumValueType Variant::GetEnumValue() const
{
	LN_CHECK_STATE_RETURNV(m_type == VariantType::Enum, false);
	return m_enum;
}
void Variant::SetStruct(const void* value, size_t size)
{
	LN_CHECK_ARGS(size <= sizeof(m_struct));
	m_type = VariantType::Struct;
	memcpy(m_struct, value, size);
	m_structSize = size;
}
const void* Variant::GetStruct() const
{
	LN_CHECK_STATE_RETURNV(m_type == VariantType::Struct, nullptr);
	return (const void*)m_struct;
}
void Variant::SetReflectionObject(ReflectionObject* obj)
{
	Release();
	m_type = VariantType::Object;
	LN_REFOBJ_SET(m_object, obj);
}
ReflectionObject* Variant::GetReflectionObject() const
{
	LN_CHECK_STATE(m_type == VariantType::Object || m_type == VariantType::ArrayObject);	// List �� Object �̈ꕔ�B
	return m_object;
}
void Variant::SetReflectionArrayObject(ReflectionArrayObject* obj)
{
	Release();
	m_type = VariantType::ArrayObject;
	LN_REFOBJ_SET(m_arrayObject, obj);
}
ReflectionArrayObject* Variant::GetReflectionArrayObject() const
{
	LN_CHECK_STATE(m_type == VariantType::ArrayObject);
	return m_arrayObject;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Variant::Copy(const Variant& obj)
{
	Release();
	m_type = obj.m_type;
	switch (m_type)
	{
	case VariantType::Null:
		break;
	case VariantType::Bool:
		m_bool = obj.m_bool;
		break;
	case VariantType::Int32:
		m_int32 = obj.m_int32;
		break;
	case VariantType::Float:
		m_float = obj.m_float;
		break;
	case VariantType::String:
		m_string = obj.m_string;
		break;
	case VariantType::Enum:
		m_enum = obj.m_enum;
		break;
	case VariantType::Struct:
		m_structSize = obj.m_structSize;
		memcpy(m_struct, obj.m_struct, m_structSize);
		break;
	case VariantType::Object:
		LN_REFOBJ_SET(m_object, obj.m_object);
		break;
	case VariantType::ArrayObject:
		LN_REFOBJ_SET(m_arrayObject, obj.m_arrayObject);
		break;
	default:
		LN_ASSERT(0);
		break;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Variant::Release()
{
	if (m_type == VariantType::Object) {
		LN_SAFE_RELEASE(m_object);
	}
	else if (m_type == VariantType::ArrayObject) {
		LN_SAFE_RELEASE(m_arrayObject);
	}
	m_type = VariantType::Null;
	m_structSize = 0;
}

} // namespace tr
LN_NAMESPACE_END
