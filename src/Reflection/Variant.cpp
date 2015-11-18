
#include "../Internal.h"
#include <Lumino/Reflection/ReflectionObject.h>
#include <Lumino/Reflection/ReflectionListObject.h>
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
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Variant::Set(bool value)
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

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Variant::Release()
{
	if (m_type == VariantType::Object) {
		LN_SAFE_RELEASE(m_object);
	}
	else if (m_type == VariantType::ListObject) {
		LN_SAFE_RELEASE(m_valueList);
	}
	m_type = VariantType::Null;
}

} // namespace tr
LN_NAMESPACE_END
