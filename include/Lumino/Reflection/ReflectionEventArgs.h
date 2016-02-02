
#pragma once
#include "Common.h"
#include "ReflectionObject.h"

LN_NAMESPACE_BEGIN
namespace tr
{
	
/**
	@brief		
*/
class ReflectionEventArgs
	: public tr::ReflectionObject
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:
	ReflectionEventArgs();
	virtual ~ReflectionEventArgs();

public:
	bool	handled;
};

/**
	@brief	PropertyChanged �C�x���g�̈����ł��B
*/
class PropertyChangedEventArgs
	: public ReflectionEventArgs
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:
	PropertyChangedEventArgs(const Property* prop, const Variant& newValue, const Variant& oldValue);
	virtual ~PropertyChangedEventArgs();

public:
	const Property*	changedProperty;
	const Variant&	newValue;
	const Variant&	oldValue;
};

} // namespace tr
LN_NAMESPACE_END