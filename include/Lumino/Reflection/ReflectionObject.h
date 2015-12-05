
#pragma once
#include "../Base/Common.h"
#include "../Base/RefObject.h"
#include "../Base/Collection.h"
#include "TypeInfo.h"

LN_NAMESPACE_BEGIN
namespace tr
{


#define LN_TR_REFLECTION_TYPEINFO_DECLARE() \
	private: \
		template<typename T> friend class ln::RefPtr; \
		friend class ln::tr::ReflectionHelper; \
		static ln::tr::TypeInfo					lnref_typeInfo; \
		ln::tr::LocalValueHavingFlags			lnref_localValueHavingFlags; \
		virtual ln::tr::TypeInfo*				lnref_GetThisTypeInfo() const; \
		static void*							lnref_bindingTypeInfo; \

#define LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(classType, baseClassType) \
	ln::tr::TypeInfo				classType::lnref_typeInfo(_T(#classType), ln::tr::ReflectionHelper::GetClassTypeInfo<baseClassType>(), &ln::tr::ReflectionHelper::GetLocalValueHavingFlags<classType>, &ln::tr::ReflectionHelper::SetBindingTypeInfo<classType>, &ln::tr::ReflectionHelper::GetBindingTypeInfo<classType>); \
	ln::tr::TypeInfo*				classType::lnref_GetThisTypeInfo() const { return &lnref_typeInfo; } \
	void*							classType::lnref_bindingTypeInfo = nullptr;

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

private:
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
	ReflectionObjectList()
	{}

	virtual ~ReflectionObjectList()
	{
		Clear();
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
		if (GetAt(index) != nullptr) {
			GetAt(index)->Release();
		}
		Collection<T>::RemoveItem(index);
	}
	virtual void SetItem(int index, const value_type& item) override
	{
		LN_SAFE_ADDREF(item);
		if (GetAt(index) != nullptr) {
			GetAt(index)->Release();
		}
		Collection<T>::SetItem(index, item);
	}

private:
};

} // namespace tr
LN_NAMESPACE_END

