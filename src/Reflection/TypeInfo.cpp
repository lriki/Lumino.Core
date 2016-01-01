
#include "../Internal.h"
#include <Lumino/Reflection/TypeInfo.h>
#include <Lumino/Reflection/ReflectionObject.h>

LN_NAMESPACE_BEGIN
namespace tr
{

//=============================================================================
// TypeInfo
//=============================================================================

/*
	HasLocalValueFlagsGetter はクラスごとに必要となる。
	ReflectionObject に仮想関数を持たせて取得するわけには行かない。
	例えば、
		ReflectionObject
			UIElement
				Control
					Button
	という継承関係があった場合、UIElement の LocalFlags は派生元・先関係なく
	純粋に UIElement に定義されたプロパティの LocalFlags がほしい。
*/

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TypeInfo* TypeInfo::GetTypeInfo(const ReflectionObject* obj)
{
	return ReflectionHelper::GetTypeInfo(obj);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TypeInfo::TypeInfo(
	const TCHAR* className,
	TypeInfo* baseClass,
	HasLocalValueFlagsGetter getter,
	BindingTypeInfoSetter bindingTypeInfoSetter,
	BindingTypeInfoGetter bindingTypeInfoGetter)
	: m_name(className)
	, m_baseClass(baseClass)
	, m_hasLocalValueFlagsGetter(getter)
	, m_bindingTypeInfoSetter(bindingTypeInfoSetter)
	, m_bindingTypeInfoGetter(bindingTypeInfoGetter)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void TypeInfo::RegisterProperty(Property* prop)
//{
//	LN_VERIFY_RETURN(!prop->m_registerd);
//	LN_VERIFY_RETURN(m_propertyList.GetCount() < 32);
//
//	//if (!prop->m_registerd)
//	{
//		prop->m_localIndex = m_propertyList.GetCount();
//		m_propertyList.Add(prop);
//		prop->m_registerd = true;
//	}
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TypeInfo::SetBindingTypeInfo(void* data)
{
	m_bindingTypeInfoSetter(data);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void* TypeInfo::GetBindingTypeInfo(const ReflectionObject* obj)
{
	TypeInfo* type = GetTypeInfo(obj);
	return type->m_bindingTypeInfoGetter();
}

} // namespace tr
LN_NAMESPACE_END

