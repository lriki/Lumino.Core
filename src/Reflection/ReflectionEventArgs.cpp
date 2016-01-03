
#include "../Internal.h"
#include <Lumino/Reflection/ReflectionEventArgs.h>

LN_NAMESPACE_BEGIN
namespace tr
{

//=============================================================================
// ReflectionEventArgs
//=============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(ReflectionEventArgs, ReflectionObject);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ReflectionEventArgs::ReflectionEventArgs()
	: handled(false)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ReflectionEventArgs::~ReflectionEventArgs()
{
}

//=============================================================================
// PropertyChangedEventArgs
//=============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(PropertyChangedEventArgs, ReflectionEventArgs);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PropertyChangedEventArgs::PropertyChangedEventArgs(const Property* prop, const Variant& newValue, const Variant& oldValue)
	: changedProperty(prop)
	, newValue(newValue)
	, oldValue(oldValue)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PropertyChangedEventArgs::~PropertyChangedEventArgs()
{
}

} // namespace tr
LN_NAMESPACE_END
