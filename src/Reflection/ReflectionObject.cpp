/*
	[2015/12/7] LN_TR_PROPERTY
		metadata はメンバに持っておいたほうが後々都合がよさそう。
		Property はこれのポインタを参照する。実体は持たない。
		いずれ metadata の継承とかオーバーライドとかやるときは、ポインタのほうが融通効くかも。

*/
#include "../Internal.h"
#include <Lumino/Reflection/Notify.h>
#include <Lumino/Reflection/TypeInfo.h>
#include <Lumino/Reflection/ReflectionObject.h>
#include <Lumino/Reflection/ReflectionArrayObject.h>
#include <Lumino/Reflection/ReflectionEventArgs.h>
#include <Lumino/Reflection/Property.h>

LN_NAMESPACE_BEGIN
namespace tr
{
#if 0
//==============================================================================
// TypeInfo
//==============================================================================

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

//------------------------------------------------------------------------------
TypeInfo::TypeInfo(const TCHAR* className, TypeInfo* baseClass, HasLocalValueFlagsGetter getter)
	: m_name(className)
	, m_baseClass(baseClass)
	, m_hasLocalValueFlagsGetter(getter)
{
}

//------------------------------------------------------------------------------
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

//==============================================================================
// ReflectionObject
//==============================================================================
ln::tr::TypeInfo				ReflectionObject::lnref_typeInfo(_T("ReflectionObject"), nullptr, nullptr, nullptr, nullptr);
ln::tr::TypeInfo*				ReflectionObject::lnref_GetThisTypeInfo() const { return &lnref_typeInfo; }

//------------------------------------------------------------------------------
ReflectionObject::ReflectionObject()
	: m_userData(nullptr)
	, m_weakRefInfo(nullptr)
{
}

//------------------------------------------------------------------------------
ReflectionObject::~ReflectionObject()
{
	Threading::MutexScopedLock lock(m_weakRefInfoMutex);
	if (m_weakRefInfo != nullptr)
	{
		m_weakRefInfo->Release();
		m_weakRefInfo->owner = nullptr;
	}
}

//------------------------------------------------------------------------------
void ReflectionObject::RaiseReflectionEvent(const ReflectionEventBase& ev, ReflectionEventArgs* args)
{
	ev.Raise(args);
}

//------------------------------------------------------------------------------
void ReflectionObject::OnPropertyChanged(PropertyChangedEventArgs* e)
{
	// e->Property を持つクラスのコールバックを呼び出す
	//e->changedProperty->NotifyPropertyChange(this, e);
}

//------------------------------------------------------------------------------
void ReflectionObject::SetPropertyValueInternal(const Property* prop, const Variant& value, bool reset, PropertySetSource source)
{
	//if (prop->IsStored())
	//{
	//	// 必要になったので作る
	//	if (m_propertyDataStore == NULL) { m_propertyDataStore = LN_NEW PropertyDataStore(); }
	//	m_propertyDataStore->SetValue(prop, value);
	//}
	//else {
		prop->SetValue(this, value, source);
	//}

	//PropertyInstanceData* data = prop->GetPropertyInstanceData(this);
	//if (data != NULL)
	//{
	//	if (reset) {
	//		data->IsDefault = true;
	//	}
	//	else
	//	{
	//		if (data->IsDefault == true)
	//		{
	//			// 新しく設定される瞬間、これまで継承元として参照していたプロパティと this に対して
	//			// プロパティ参照更新値を1つ進める。子は Get しようとしたとき、継承元を再検索する。
	//			// TODO: SetTypedPropertyValue にも同じようなのがある。共通化したい。あとテスト
	//			if (data->InheritanceParent != NULL) {
	//				data->InheritanceTarget->GetPropertyInstanceData(data->InheritanceParent)->PathRevisionCount++;
	//			}
	//			data->PathRevisionCount++;
	//		}
	//		data->IsDefault = false;
	//		data->RevisionCount++;
	//	}
	//}

	//SetPropertyValue(prop->GetName(), value);	// TODO: GetName じゃなくて、型情報も考慮するように。あるいは生ポインタ
}

//==============================================================================
// ReflectionArrayObject
//==============================================================================

//------------------------------------------------------------------------------
ReflectionArrayObject::ReflectionArrayObject()
{
}

//------------------------------------------------------------------------------
ReflectionArrayObject::~ReflectionArrayObject()
{
}

} // namespace tr
LN_NAMESPACE_END

