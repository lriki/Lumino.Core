
#include "../Internal.h"
#include <Lumino/Reflection/TypeInfo.h>
#include <Lumino/Reflection/ReflectionObject.h>

LN_NAMESPACE_BEGIN
namespace tr
{

//=============================================================================
// TypeInfo
//=============================================================================

/*
	HasLocalValueFlagsGetter �̓N���X���ƂɕK�v�ƂȂ�B
	ReflectionObject �ɉ��z�֐����������Ď擾����킯�ɂ͍s���Ȃ��B
	�Ⴆ�΁A
		ReflectionObject
			UIElement
				Control
					Button
	�Ƃ����p���֌W���������ꍇ�AUIElement �� LocalFlags �͔h�����E��֌W�Ȃ�
	������ UIElement �ɒ�`���ꂽ�v���p�e�B�� LocalFlags ���ق����B
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
//void TypeInfo::RegisterProperty(Property* prop)
//{
//	LN_VERIFY_RETURN(!prop->m_registerd);
//	LN_VERIFY_RETURN(m_propertyList.GetCount() < 32);
//
//	//if (!prop->m_registerd)
//	{
//		prop->m_localIndex = m_propertyList.GetCount();
//		m_propertyList.Add(prop);
//		prop->m_registerd = true;
//	}
//}

} // namespace tr
LN_NAMESPACE_END

