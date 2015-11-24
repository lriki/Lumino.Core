
#pragma once
#include "../Base/Common.h"
#include "../Base/RefObject.h"
#include "TypeInfo.h"

LN_NAMESPACE_BEGIN
namespace tr
{

class ReflectionHelper
{
public:
	template<class T>
	static ln::tr::LocalValueHavingFlags* GetLocalValueHavingFlags(ReflectionObject* thisObj)
	{
		return &static_cast<T*>(thisObj)->lnref_localValueHavingFlags;
	}


	template<class T>
	static TypeInfo* GetClassTypeInfo()
	{
		return &T::lnref_typeInfo;
	}
};

#define LN_TR_REFLECTION_TYPEINFO_DECLARE() \
	private: \
		friend class ln::tr::ReflectionHelper; \
		static ln::tr::TypeInfo					lnref_typeInfo; \
		ln::tr::LocalValueHavingFlags			lnref_localValueHavingFlags; \
		virtual ln::tr::TypeInfo*				lnref_GetThisTypeInfo() const;

#define LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(classType, baseClassType) \
	ln::tr::TypeInfo				classType::lnref_typeInfo(_T(#classType), ln::tr::ReflectionHelper::GetClassTypeInfo<baseClassType>(), &ln::tr::ReflectionHelper::GetLocalValueHavingFlags<classType>); \
	ln::tr::TypeInfo*				classType::lnref_GetThisTypeInfo() const { return &lnref_typeInfo; }

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

protected:

};


} // namespace tr
LN_NAMESPACE_END

