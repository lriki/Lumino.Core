
#pragma once
#include "../Base/Common.h"
#include "../Base/RefObject.h"
#include "../Base/Collection.h"
#include "TypeInfo.h"
#include "Notify.h"

LN_NAMESPACE_BEGIN
namespace tr
{

#define LN_TR_REFLECTION_TYPEINFO_DECLARE_COMMON(typeInfo) \
	private: \
		template<typename T> friend class ln::RefPtr; \
		friend class ln::tr::ReflectionHelper; \
		static typeInfo							lnref_typeInfo; \
		ln::tr::LocalValueHavingFlags			lnref_localValueHavingFlags; \
		virtual typeInfo*						lnref_GetThisTypeInfo() const; \
		static void*							lnref_bindingTypeInfo; \

#define LN_TR_REFLECTION_TYPEINFO_IMPLEMENT_COMMON(typeInfo, classType, baseClassType) \
	typeInfo						classType::lnref_typeInfo(_T(#classType), ln::tr::ReflectionHelper::GetClassTypeInfo<baseClassType>(), &ln::tr::ReflectionHelper::GetLocalValueHavingFlags<classType>, &ln::tr::ReflectionHelper::SetBindingTypeInfo<classType>, &ln::tr::ReflectionHelper::GetBindingTypeInfo<classType>); \
	typeInfo*						classType::lnref_GetThisTypeInfo() const { return &lnref_typeInfo; } \
	void*							classType::lnref_bindingTypeInfo = nullptr;

#define LN_TR_REFLECTION_TYPEINFO_DECLARE() \
	LN_TR_REFLECTION_TYPEINFO_DECLARE_COMMON(ln::tr::TypeInfo)

#define LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(classType, baseClassType) \
	LN_TR_REFLECTION_TYPEINFO_IMPLEMENT_COMMON(ln::tr::TypeInfo, classType, baseClassType)


/**
	@brief		
*/
class ReflectionObject
	: public RefObject
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:
	ReflectionObject();
	virtual ~ReflectionObject();

	void SetUserData(void* data) { m_userData = data; }
	void* GetUserData() const { return m_userData; }


protected:
	friend class Property;
	void RaiseReflectionEvent(const ReflectionEventBase& ev, ReflectionEventArgs* args);
	virtual void OnPropertyChanged(PropertyChangedEventArgs* e);

	template<typename... TArgs>
	void RaiseDelegateEvent(DelegateEvent<TArgs...>& ev, TArgs... args) { ev.Raise(args...); }

private:
	void SetPropertyValueInternal(const Property* prop, const Variant& value, bool reset);

	void*	m_userData;
};

/**
	@brief		
*/
template<typename T>
class ReflectionObjectList
	: public RefObject
	, public Collection<T>
{
public:
	typedef typename Collection<T>::value_type	value_type;

public:
	ReflectionObjectList()
	{}

	virtual ~ReflectionObjectList()
	{
		Collection<T>::Clear();
	}

protected:
	virtual void InsertItem(int index, const value_type& item) override
	{
		Collection<T>::InsertItem(index, item);
		LN_SAFE_ADDREF(item);
	}
	virtual void ClearItems() override
	{
		for (auto* item : *this) {
			LN_SAFE_RELEASE(item);
		}
		Collection<T>::ClearItems();
	}
	virtual void RemoveItem(int index) override
	{
		if (Collection<T>::GetAt(index) != nullptr) {
			Collection<T>::GetAt(index)->Release();
		}
		Collection<T>::RemoveItem(index);
	}
	virtual void SetItem(int index, const value_type& item) override
	{
		LN_SAFE_ADDREF(item);
		if (Collection<T>::GetAt(index) != nullptr) {
			Collection<T>::GetAt(index)->Release();
		}
		Collection<T>::SetItem(index, item);
	}

private:
};

} // namespace tr

typedef tr::ReflectionObject Object;

LN_NAMESPACE_END

