
#include "../Internal.h"
#include <Lumino/Reflection/Notify.h>
#include <Lumino/Reflection/TypeInfo.h>
#include <Lumino/Reflection/ReflectionEventArgs.h>
#include <Lumino/Reflection/ReflectionObject.h>
#include <Lumino/Reflection/Property.h>

LN_NAMESPACE_BEGIN
namespace tr
{

//==============================================================================
// ReflectionHelper
//==============================================================================
//------------------------------------------------------------------------------
//void ReflectionHelper::RaiseReflectionEvent(ReflectionObject* obj, ReflectionEventBase* ev, ReflectionEventArgs* e)
//{
//	ev->Raise(e);
//}

//==============================================================================
// TypeInfo
//==============================================================================

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

//------------------------------------------------------------------------------
TypeInfo* TypeInfo::GetTypeInfo(const ReflectionObject* obj)
{
	return ReflectionHelper::GetTypeInfo(obj);
}

//------------------------------------------------------------------------------
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
	, m_internalGroup(0)
{
}

//------------------------------------------------------------------------------
void TypeInfo::RegisterProperty(Property* prop)
{
	if (LN_CHECKEQ_ARG(prop->m_registerd)) return;
	if (LN_CHECKEQ_ARG(m_propertyList.GetCount() >= 32)) return;
	prop->m_localIndex = m_propertyList.GetCount();
	m_propertyList.Add(prop);
	prop->m_registerd = true;
}

//------------------------------------------------------------------------------
void TypeInfo::RegisterReflectionEvent(ReflectionEventInfo* ev)
{
	if (LN_CHECKEQ_ARG(ev->m_registerd)) return;
	m_routedEventList.Add(ev);
	ev->m_registerd = true;
}

//------------------------------------------------------------------------------
bool TypeInfo::InvokeReflectionEvent(ReflectionObject* target, const ReflectionEventInfo* ev, ReflectionEventArgs* e)
{
	for (ReflectionEventInfo* dynamicEvent : m_routedEventList)
	{
		if (dynamicEvent == ev)
		{
			// owner に AddHandler されているイベントハンドラを呼び出す。
			dynamicEvent->CallEvent(target, e);
			return e->handled;	// ev と同じイベントは1つしかリスト内に無いはずなのですぐ return
		}
	}

	// ベースクラスがあれば、さらにベースクラスを見に行く
	if (m_baseClass != nullptr)
	{
		return m_baseClass->InvokeReflectionEvent(target, ev, e);
	}
	return false;
}

//------------------------------------------------------------------------------
void TypeInfo::SetBindingTypeInfo(void* data)
{
	m_bindingTypeInfoSetter(data);
}

//------------------------------------------------------------------------------
void* TypeInfo::GetBindingTypeInfo(const ReflectionObject* obj)
{
	TypeInfo* type = GetTypeInfo(obj);
	return type->m_bindingTypeInfoGetter();
}

} // namespace tr
LN_NAMESPACE_END

