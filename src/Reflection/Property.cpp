
#include "../Internal.h"
#include <Lumino/Reflection/TypeInfo.h>
#include <Lumino/Reflection/Property.h>
#include <Lumino/Reflection/EventArgsPool.h>

LN_NAMESPACE_BEGIN
namespace tr
{

//==============================================================================
// Property
//==============================================================================
static EventArgsPool g_eventArgsPool;

//------------------------------------------------------------------------------
PropertyInfo::PropertyInfo(TypeInfo* ownerClassType, PropertyMetadata* metadata, size_t memberOffset, bool stored)
	: m_ownerClassType(ownerClassType)
	, m_metadata(metadata)
	, m_memberOffset(memberOffset)
	, m_stored(stored)
	, m_registerd(false)
{
	m_ownerClassType->RegisterProperty(this);
}

//------------------------------------------------------------------------------
PropertyInfo::~PropertyInfo()
{
}

//------------------------------------------------------------------------------
void PropertyInfo::NotifyPropertyChanged(ReflectionObject* target, const PropertyInfo* prop, PropertySetSource source)
{
	RefPtr<PropertyChangedEventArgs> e(g_eventArgsPool.Create<PropertyChangedEventArgs>(prop, source), false);
	target->OnPropertyChanged(e);
	prop->m_metadata->CallPropertyChangedCallback(target, e);
}

//------------------------------------------------------------------------------
void PropertyInfo::SetPropertyValue(ReflectionObject* obj, const PropertyInfo* prop, const Variant& value, PropertySetSource source)
{
	obj->SetPropertyValueInternal(prop, value, false, source);
}

//------------------------------------------------------------------------------
Variant PropertyInfo::GetPropertyValue(ReflectionObject* obj, const PropertyInfo* prop)
{
	//if (prop->IsStored())
	//{
	//	if (m_propertyDataStore == NULL) {
	//		return prop->GetMetadata()->GetDefaultValue();
	//	}
	//	//LN_THROW(m_propertyDataStore != NULL, KeyNotFoundException);
	//	Variant v;
	//	if (m_propertyDataStore->TryGetValue(prop, &v)) {
	//		return v;
	//	}
	//	return prop->GetMetadata()->GetDefaultValue();
	//}
	//else
	//{
	//	// この const_cast は、外に公開する Getter はとにかく const 関数にしたかったためのもの。
	//	UpdateInheritanceProperty(const_cast<CoreObject*>(this), prop);
		return prop->GetValue(obj);
	//}
}

} // namespace tr
LN_NAMESPACE_END
