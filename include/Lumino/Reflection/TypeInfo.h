
#pragma once
#include "../Base/Common.h"
#include "../Base/RefObject.h"
#include "../Base/String.h"

LN_NAMESPACE_BEGIN
namespace tr
{
class TypeInfo;
class ReflectionObject;
class Property;
typedef uint64_t LocalValueHavingFlags;

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
	template<class T>
	static void SetBindingTypeInfo(void* data)
	{
		T::lnref_bindingTypeInfo = data;
	}
	template<class T>
	static void* GetBindingTypeInfo()
	{
		return T::lnref_bindingTypeInfo;
	}
	template<class T>
	static TypeInfo* GetTypeInfo(const T* obj)
	{
		return obj->lnref_GetThisTypeInfo();
	}
	template<class T>
	inline static TypeInfo* GetTypeInfo()
	{
		return &T::lnref_typeInfo;
	}
};

/**
	@brief		
*/
class TypeInfo
{
public:
	typedef uint64_t* (*HasLocalValueFlagsGetter)(ReflectionObject* _this);
	typedef void (*BindingTypeInfoSetter)(void* data);
	typedef void* (*BindingTypeInfoGetter)();
	typedef uint8_t RevisionCount;
	static const int MaxProperties = sizeof(uint64_t) * 8;

public:
	
	/**
		@brief	指定したオブジェクトの型情報を取得します。
	*/
	static TypeInfo* GetTypeInfo(const ReflectionObject* obj);

	/**
		@brief	型引数に指定したクラス型の型情報を取得します。
	*/
	template<class T>
	static TypeInfo* GetTypeInfo();

public:
	TypeInfo(
		const TCHAR* className,
		TypeInfo* baseClass,
		HasLocalValueFlagsGetter getter,
		BindingTypeInfoSetter bindingTypeInfoSetter,
		BindingTypeInfoGetter bindingTypeInfoGetter);
	
	/**
		@brief	クラス名を取得します。
	*/
	const String& GetName() const { return m_name; }

	void RegisterProperty(Property* prop);
	Property* FindProperty(const String& name) const;

	//// childObjProp が継承できるプロパティをこの TypeInfo から探す。見つからなければ NULL を返す。
	//// childObj : childObjProp を持つオブジェクト
	//Property* FindInheritanceProperty(const Property* childObjProp, CoreObject* childObj) const;

	//void RegisterRoutedEvent(RoutedEvent* ev);
	//RoutedEvent* FindRoutedEvent(const String& name) const;	// TODO: いらないかも
	//void InvokeRoutedEvent(CoreObject* owner, const RoutedEvent* ev, RoutedEventArgs* e);

	///// RoutedEventHandler は、ユーザーが動的に追加できるハンドラよりも前に呼び出される。
	///// WPF では「静的ハンドラ」と呼ばれている。動的イベントに登録するのに比べ、メモリを使用しない。
	//void RegisterRoutedEventHandler(const RoutedEvent* ev, RoutedEventHandler* handler);
	//RoutedEventHandler* FindRoutedEventHandler(const RoutedEvent* ev) const;

	//TypeInfo* GetBaseClass() const { return m_baseClass; }
	//uint32_t* GetHasLocalValueFlags(CoreObject* obj) { return m_hasLocalValueFlagsGetter(obj); }

	///// ベースクラスも含めた全てのプロパティを列挙する
	//static void ForEachAllProperty(const TypeInfo* typeInfo, const std::function<void(Property*)>& callback);


	void SetBindingTypeInfo(void* data);
	static void* GetBindingTypeInfo(const ReflectionObject* obj);

	bool operator == (const TypeInfo& info) const { return m_name == info.m_name; }
	bool operator < (const TypeInfo& info) const { return m_name < info.m_name; }

protected:

private:
	//typedef SortedArray<const RoutedEvent*, RoutedEventHandler*>	RoutedEventHandlerList;

	String						m_name;						// クラス名
	TypeInfo*					m_baseClass;				// 継承元クラスを示す TypeInfo
	Array<Property*>			m_propertyList;				// この型のクラスがもつプロパティのリスト
	HasLocalValueFlagsGetter	m_hasLocalValueFlagsGetter;	// プロパティがローカル値を保持しているかを示すビットフラグを取得するコールバック。ローカル値を持たない場合はプロパティの値を親から継承する。
	BindingTypeInfoSetter		m_bindingTypeInfoSetter;
	BindingTypeInfoGetter		m_bindingTypeInfoGetter;

	//Array<RoutedEvent*>		m_routedEventList;
	//RoutedEventHandlerList	m_routedEventHandlerList;
	//
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<class T>
inline static TypeInfo* TypeInfo::GetTypeInfo()
{
	return ReflectionHelper::GetTypeInfo<T>();
}

} // namespace tr
LN_NAMESPACE_END
