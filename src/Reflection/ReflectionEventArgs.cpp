
#include "../Internal.h"
#include <Lumino/Reflection/ReflectionEventArgs.h>

LN_NAMESPACE_BEGIN
namespace tr
{

//==============================================================================
// ReflectionEventArgs
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(ReflectionEventArgs, ReflectionObject);

//------------------------------------------------------------------------------
ReflectionEventArgs::ReflectionEventArgs()
	: handled(false)
{
}

//------------------------------------------------------------------------------
ReflectionEventArgs::~ReflectionEventArgs()
{
}

//==============================================================================
// PropertyChangedEventArgs
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(PropertyChangedEventArgs, ReflectionEventArgs);

//------------------------------------------------------------------------------
PropertyChangedEventArgs::PropertyChangedEventArgs(const Property* prop, const Variant& newValue_, const Variant& oldValue_, PropertySetSource cause_)
	: changedProperty(prop)
	, newValue(newValue_)
	, oldValue(oldValue_)
	, cause(cause_)
{
}

//------------------------------------------------------------------------------
PropertyChangedEventArgs::~PropertyChangedEventArgs()
{
}

} // namespace tr
LN_NAMESPACE_END
