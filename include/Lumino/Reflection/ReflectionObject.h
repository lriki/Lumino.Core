
#pragma once
#include "../Base/Common.h"
#include "../Base/RefObject.h"
#include "TypeInfo.h"

LN_NAMESPACE_BEGIN
namespace tr
{


#define LN_TR_REFLECTION_TYPEINFO_DECLARE() \
	private: \
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


} // namespace tr
LN_NAMESPACE_END

