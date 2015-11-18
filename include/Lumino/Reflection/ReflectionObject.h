
#pragma once
#include "../Base/Common.h"
#include "../Base/RefObject.h"

LN_NAMESPACE_BEGIN
namespace tr
{
	

/**
	@brief		
*/
class ReflectionObject
	: public RefObject
{
public:
	ReflectionObject();
	virtual ~ReflectionObject();

protected:

};

#define LN_TR_REFLECTION_TYPEINFO_DECLARE() \
	private: \
		static TypeInfo			lnref_typeInfo; \
		LocalValueHavingFlags	lnref_localValueHavingFlags; \
		virtual TypeInfo*		lnref_GetThisTypeInfo() const; \
		static uint32_t*		lnref_GetLocalValueHavingFlags(CoreObject* _this);
		// Å™TODO: ÉâÉÄÉ_éÆÇ…Ç≈Ç´Ç»Ç¢ÅH

#define LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(classType, baseClassType) \
	TypeInfo				classType::lnref_typeInfo(_T(#classType), baseClassType::GetClassTypeInfo(), &classType::lnref_GetLocalValueHavingFlags); \
	TypeInfo*				classType::lnref_GetThisTypeInfo() const { return &lnref_typeInfo; } \
	LocalValueHavingFlags*	classType::lnref_GetLocalValueHavingFlags(CoreObject* _this) { return &static_cast<classType*>(_this)->lnref_localValueHavingFlags; }


} // namespace tr
LN_NAMESPACE_END

