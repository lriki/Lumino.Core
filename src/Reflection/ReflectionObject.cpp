/*
	[2015/12/7] LN_TR_PROPERTY
		metadata はメンバに持っておいたほうが後々都合がよさそう。
		Property はこれのポインタを参照する。実体は持たない。
		いずれ metadata の継承とかオーバーライドとかやるときは、ポインタのほうが融通効くかも。

*/
#include "../Internal.h"
#include <Lumino/Reflection/TypeInfo.h>
#include <Lumino/Reflection/ReflectionObject.h>
#include <Lumino/Reflection/ReflectionArrayObject.h>

LN_NAMESPACE_BEGIN
namespace tr
{
#if 0
//=============================================================================
// TypeInfo
//=============================================================================

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

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TypeInfo::TypeInfo(const TCHAR* className, TypeInfo* baseClass, HasLocalValueFlagsGetter getter)
	: m_name(className)
	, m_baseClass(baseClass)
	, m_hasLocalValueFlagsGetter(getter)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TypeInfo::RegisterProperty(Property* prop)
{
	LN_VERIFY_RETURN(!prop->m_registerd);
	LN_VERIFY_RETURN(m_propertyList.GetCount() < 32);

	//if (!prop->m_registerd)
	{
		prop->m_localIndex = m_propertyList.GetCount();
		m_propertyList.Add(prop);
		prop->m_registerd = true;
	}
}
#endif

//=============================================================================
// ReflectionObject
//=============================================================================
ln::tr::TypeInfo				ReflectionObject::lnref_typeInfo(_T("ReflectionObject"), nullptr, nullptr, nullptr, nullptr);
ln::tr::TypeInfo*				ReflectionObject::lnref_GetThisTypeInfo() const { return &lnref_typeInfo; }

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ReflectionObject::ReflectionObject()
	: m_userData(nullptr)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ReflectionObject::~ReflectionObject()
{
}


//=============================================================================
// ReflectionArrayObject
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ReflectionArrayObject::ReflectionArrayObject()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ReflectionArrayObject::~ReflectionArrayObject()
{
}

} // namespace tr
LN_NAMESPACE_END

