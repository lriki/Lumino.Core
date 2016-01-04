
#pragma once
#include <memory>
#include "Common.h"
#include "../Base/RefObject.h"
#include "../Base/String.h"
#include "PropertyMetadata.h"

LN_NAMESPACE_BEGIN
namespace tr
{

// TODO; Internal
typedef void(*PropertyChangedCallback)(ReflectionObject* obj, PropertyChangedEventArgs* e);
typedef std::unique_ptr<PropertyInstanceData>*(*InstanceDataGetterFunc)(ReflectionObject* obj);

// TODO; Internal
class PropertyInstanceData
	: public RefObject
{
public:
	Property*			InheritanceKey;		// プロパティを親から継承するとき、this またはこの値をキーとして検索する。
	ReflectionObject*	InheritanceParent;	// ↑ので見つかった親あるいは祖先オブジェクト
	const Property*		InheritanceTarget;	// ↑のオブジェクトのどのプロパティから受け継ぐか (走査の結果継承元が見つからなかった場合、この PropertyInstanceData が表す Propery を指す。このとき InheritanceParent は NULL)
	uint16_t			RevisionCount;
	uint16_t			PathRevisionCount;
	bool				IsDefault;

	PropertyInstanceData()
		: InheritanceTarget(nullptr)
		, InheritanceParent(nullptr)
		, RevisionCount(0)
		, PathRevisionCount(0)
		, IsDefault(true)
	{}
};

/**
	@brief		ReflectionObject のサブクラスが実装できるプロパティを表します。
	@details	プロパティは、メンバ変数に対する簡易的なリフレクションを提供します。		
*/
class Property
{
public:
	Property(TypeInfo* ownerClassType, PropertyMetadata* metadata, bool stored);
	virtual ~Property();

	virtual void SetValue(ReflectionObject* target, Variant value) const { LN_THROW(0, InvalidOperationException); }
	virtual Variant GetValue(const ReflectionObject* target) const { LN_THROW(0, InvalidOperationException); }
	virtual void AddItem(ReflectionObject* target, const Variant& value) const { LN_THROW(0, InvalidOperationException); }

	virtual bool IsReadable() const { return false; }
	virtual bool IsWritable() const { return false; }
	virtual bool IsList() const { return false; }

	TypeInfo* GetOwnerClassType() const { return m_ownerClassType; }
	int GetLocalIndex() const { return m_localIndex; }	// TypeInfo 内でこのプロパティを一意に識別するインデックス値
	bool IsStored() const { return m_stored; }

public:

	/**
		@brief		指定したオブジェクトのプロパティの値を設定します。
	*/
	static void SetPropertyValue(ReflectionObject* obj, const Property* prop, const Variant& value);
	
	/**
		@brief		指定したオブジェクトのプロパティの値を取得します。
	*/
	static Variant GetPropertyValue(ReflectionObject* obj, const Property* prop);

	/**
		@brief		指定したオブジェクトのプロパティの値を設定します。あらかじめ型が分かっている場合、SetPropertyValue() よりも少ないオーバーヘッドで設定できます。
	*/
	template<typename TValue>
	static void SetPropertyValueDirect(ReflectionObject* obj, const Property* prop, const TValue& value);

	/**
		@brief		指定したオブジェクトのプロパティの値を取得します。あらかじめ型が分かっている場合、GetPropertyValue() よりも少ないオーバーヘッドで設定できます。
	*/
	template<typename TValue>
	static const TValue& GetPropertyValueDirect(const ReflectionObject* obj, const Property* prop);

protected:
	static void NotifyPropertyChanged(ReflectionObject* target, const Property* prop, const Variant& newValue, const Variant& oldValue);

private:
	friend class TypeInfo;
	TypeInfo*			m_ownerClassType;
	PropertyMetadata*	m_metadata;
	bool				m_stored;
	int					m_localIndex;
	bool				m_registerd;

public:	// TODO
	InstanceDataGetterFunc	m_instanceDataGetterFunc;
};





template<typename TValue>
class TypedProperty
	: public Property
{
public:
	typedef void(*SetterFunc)(ReflectionObject* obj, TValue& value);
	typedef void(*GetterFunc)(ReflectionObject* obj, TValue** valuePtr);
	typedef void(*OnPropertyChangedFunc)(ReflectionObject* obj, PropertyChangedEventArgs* e);
	// ↑※static 関数のポインタでないと、言語バインダを作りにくくなる。

public:
	TypedProperty(TypeInfo* ownerTypeInfo, const TCHAR* name, PropertyMetadata* metadata)
		: Property(ownerTypeInfo, metadata, false)
		, m_name(name)
		, m_setter(NULL)
		, m_getter(NULL)
		//, m_propChanged(NULL)
	{}

	virtual ~TypedProperty() {}

public:
	virtual const String& GetName() const { return m_name; }

	virtual void SetValue(ReflectionObject* target, Variant value) const
	{
		SetValueDirect(target, Variant::Cast<TValue>(value));
	}
	virtual Variant GetValue(const ReflectionObject* target) const
	{
		LN_THROW(m_getter != NULL, InvalidOperationException);
		TValue* v;
		m_getter(const_cast<ReflectionObject*>(target), &v);
		return Variant(*v);
	}
	//virtual void AddItem(ReflectionObject* target, const Variant& value) const
	//{
	//	LN_THROW(m_getter != NULL, InvalidOperationException);
	//	//VariantList* list = m_getter(target);
	//	// TODO: 間違えて UIElementFractory のまま追加してしまうことがあった。型チェックできると良い。
	//	//list->AddVariant(value);
	//	auto list = m_getter(target);
	//	AddItemInternal(list, value);
	//}
	virtual bool IsReadable() const { return m_getter != NULL; }	// TODO: virtual やめたほうが高速化できる。
	virtual bool IsWritable() const { return m_getter != NULL; }
	//virtual bool IsList() const { return ListOperationSelector2<TValue>::IsList(); }

	void SetValueDirect(ReflectionObject* target, const TValue& value) const
	{
		LN_THROW(m_setter != NULL, InvalidOperationException);

		// 独自の値を持つことを示すフラグを立てる
		uint32_t f = 0x1 << GetLocalIndex();
		uint32_t* flags = GetOwnerClassType()->GetHasLocalValueFlags(target);
		(*flags) |= f;

		if (m_getter != NULL)
		{
			TValue* v;
			m_getter(target, &v);
			Variant oldValue(*v);
			m_setter(target, const_cast<TValue&>(value));
			NotifyPropertyChanged(target, this, value, oldValue);
		}
		else
		{
			m_setter(target, const_cast<TValue&>(value));
			NotifyPropertyChanged(target, this, value, Variant::Null);
		}
	}
	const TValue& GetValueDirect(const ReflectionObject* target) const
	{
		LN_THROW(m_getter != NULL, InvalidOperationException);
		TValue* v;
		m_getter(const_cast<ReflectionObject*>(target), &v);
		return *v;
	}

	template<typename T>
	void AddItemInternal(T& list, const Variant& item) const	// 値型
	{
		LN_THROW(0, InvalidOperationException);
	}
	template<typename T>
	void AddItemInternal(T* list, const Variant& item) const	// ポインタ型
	{
		ListOperationSelector<T, std::is_base_of<CoreList, T>::type >::AddItem(*list, item);
	}


	template<typename T, typename TIsList> struct ListOperationSelector
	{
		static bool IsList() { return false; }
		static void AddItem(T& list, const Variant& item) { LN_THROW(0, InvalidOperationException); }
	};
	template<typename T> struct ListOperationSelector<T, std::true_type>
	{
		static bool IsList() { return true; }
		static void AddItem(T& list, const Variant& item) { list.AddVariant(item); }
	};


	//template<typename T> struct ListOperationSelector2
	//{
	//	static bool IsList() { return false; }
	//};
	//template<typename T> struct ListOperationSelector2<T*>
	//{
	//	static bool IsList() { return ListOperationSelector<T, std::is_base_of<ReflectionObjectList, T>::type>::IsList(); }
	//};

	//virtual void NotifyPropertyChange(ReflectionObject* target, PropertyChangedEventArgs* e) const
	//{
	//	if (m_propChanged != NULL) {
	//		m_propChanged(target, e);
	//	}
	//}

private:
	template<typename T> friend class TypedPropertyInitializer;
	String					m_name;
	SetterFunc				m_setter;
	GetterFunc				m_getter;
	//OnPropertyChangedFunc	m_propChanged;
};


// TODO; Internal
template<typename TValue>
class TypedPropertyInitializer
{
	// Initializer は .h 側に不必要な型を書きたくないから用意したもの。
	// 現在 TypedProperty はメンバ static ではなく、グローバル static を前提としている。
	// となると、private メンバには当然アクセスできないが、このクラス経由で各メンバへの参照経路を設定する。

	// ・LN_PROPERTY を書く宣言(.h)側に、TypedProperty<> を公開したくない。基底のポインタ Property* として公開したい。
	//		→	TypedProperty<> 以外のプロパティを作りたいとき、マクロ名を変えないとならなくなる。
	//			「宣言は LN_PROPERTY だけ書けばOK」にして、考えることは極力減らしたい。
	// ・とすると、TypedProperty<> の実体はメンバではなくグローバル変数にしなければならない。
	//		→	マクロで生成される各種アクセサにアクセスできなくなる。

public:
	typedef void(*SetterFunc)(ReflectionObject* obj, TValue& value);
	typedef void(*GetterFunc)(ReflectionObject* obj, TValue** valuePtr);

	// Obsolete
	//TypedPropertyInitializer(TypedProperty<TValue>* prop, SetterFunc setter, GetterFunc getter, PropertyChangedCallback propChanged)
	//{
	//	prop->m_setter = setter;
	//	prop->m_getter = getter;
	//	//prop->m_propChanged = propChanged;
	//}

	//template<typename TPropertyChangedCallbackCaster>
	TypedPropertyInitializer(
		TypedProperty<TValue>* prop,
		SetterFunc setter,
		GetterFunc getter,
		//TPropertyChangedCallbackCaster propertyChangedCallbackCaster,
		//PropertyMetadata* metadata,
		InstanceDataGetterFunc instanceDataGetter)
	{
		prop->m_setter = setter;
		prop->m_getter = getter;
		//prop->m_propChanged = propertyChangedCallbackCaster;
		//prop->m_metadata = metadata;
		prop->m_instanceDataGetterFunc = instanceDataGetter;
		//assert(metadata != NULL);
	}
};



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TValue>
void Property::SetPropertyValueDirect(ReflectionObject* obj, const Property* prop, const TValue& value)
{
	LN_THROW(prop != NULL, ArgumentException);
	auto t = static_cast<const TypedProperty<TValue>*>(prop);
	t->SetValueDirect(obj, value);
	//PropertyInstanceData* data = prop->GetPropertyInstanceData(this);
	//if (data != NULL)
	//{
	//	if (data->IsDefault == true)
	//	{
	//		// 新しく設定される瞬間、これまで継承元として参照していたプロパティと this に対して
	//		// プロパティ参照更新値を1つ進める。子は Get しようとしたとき、継承元を再検索する。
	//		if (data->InheritanceParent != NULL) {
	//			data->InheritanceTarget->GetPropertyInstanceData(data->InheritanceParent)->PathRevisionCount++;
	//		}
	//		data->PathRevisionCount++;
	//	}
	//	data->IsDefault = false;
	//	data->RevisionCount++;
	//}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TValue>
const TValue& Property::GetPropertyValueDirect(const ReflectionObject* obj, const Property* prop)
{
	LN_THROW(prop != NULL, ArgumentException);

	//UpdateInheritanceProperty(const_cast<CoreObject*>(this), prop);

	auto t = static_cast<const TypedProperty<TValue>*>(prop);
	return t->GetValueDirect(obj);
}


class PropertyHelper
{
public:

	template<typename MemberValueT, typename PropertyValueT>
	static void SetValue(MemberValueT& field, PropertyValueT& value)
	{
		field = value;
	}
	//template<typename PropertyValueT>
	//static void GetValue(const RefPtr<PropertyValueT>& field, PropertyValueT** valuePtr)
	//{
	//	*valuePtr = field;
	//}
	template<typename MemberValueT, typename PropertyValueT>
	static void GetValue(MemberValueT& field, PropertyValueT** valuePtr)
	{
		*valuePtr = &field;
	}
};


#define LN_TR_PROPERTY(valueType, propVar) \
	public:  static const ln::tr::Property*							propVar##; \
	private: static void											set_##propVar(ln::tr::ReflectionObject* obj, valueType& value); \
	private: static void											get_##propVar(ln::tr::ReflectionObject* obj, valueType** valuePtr); \
	private: static ln::tr::TypedPropertyInitializer<valueType>		init_##propVar; \
	private: static std::unique_ptr<ln::tr::PropertyInstanceData>*	getInstanceData_##propVar(ln::tr::ReflectionObject* obj); \
	private: std::unique_ptr<ln::tr::PropertyInstanceData>			instanceData_##propVar = nullptr;

#define LN_TR_PROPERTY_IMPLEMENT(ownerClass, valueType, propVar, propName, memberVar, metadata) \
	ln::tr::PropertyMetadata						ownerClass_##metadata_##propVar = metadata; \
	static ln::tr::TypedProperty<valueType>			_##propVar(ln::tr::TypeInfo::GetTypeInfo<ownerClass>(), _T(propName), &ownerClass_##metadata_##propVar); \
	const ln::tr::Property*							ownerClass::propVar = &_##propVar; \
	void											ownerClass::set_##propVar(ln::tr::ReflectionObject* obj, valueType& value) { tr::PropertyHelper::SetValue(static_cast<ownerClass*>(obj)->memberVar, value);/*static_cast<ownerClass*>(obj)->memberVar = const_cast<valueType&>(value);*/ } \
	void											ownerClass::get_##propVar(ln::tr::ReflectionObject* obj, valueType** valuePtr) { tr::PropertyHelper::GetValue(static_cast<ownerClass*>(obj)->memberVar, valuePtr); } \
	ln::tr::TypedPropertyInitializer<valueType>		ownerClass::init_##propVar(&_##propVar, &ownerClass::set_##propVar, &ownerClass::get_##propVar,/* &ownerClass::metadata_##propVar,*/ ownerClass::getInstanceData_##propVar); \
	std::unique_ptr<ln::tr::PropertyInstanceData>*	ownerClass::getInstanceData_##propVar(ln::tr::ReflectionObject* obj) { return &(static_cast<ownerClass*>(obj)->instanceData_##propVar); }

//	typedef void(ownerClass::*PropertyChangedCallback_##propVar)(ln::tr::PropertyChangedEventArgs*);
//	void											ownerClass::changed_##propVar(ln::tr::ReflectionObject* obj, PropertyChangedEventArgs* e) { auto func = metadata_##propVar.GetPropertyChangedCallback<PropertyChangedCallback_##propVar>(); if (func != nullptr) { (static_cast<ownerClass*>(obj)->*func)(e); } }


/*
	LN_TR_PROPERTY(String, TextProperty);

	▼ 以下のように展開される。

	public:
		static const ln::tr::Property*	TextProperty;

	private:
		// 指定したオブジェクトに値を設定する
		static void	 set_TextProperty(ReflectionObject* obj, String value);

		// 指定したオブジェクトの値を取得する
		static String get_TextProperty(const ReflectionObject* obj);

		// 指定したオブジェクトに対して、PropertyMetadata に登録されている PropertyChangedCallback を呼び出す
		static void changed_TextProperty(ReflectionObject* obj, PropertyChangedEventArgs* e);

		// Property オブジェクトの初期化を行う
		static PropertyInitializer<valueType> init_TextProperty;

		// プロパティのメタ情報定義
		static PropertyMetadata metadata_TextProperty;

		// オブジェクトの持つ PropertyInstanceData のポインタへの参照を取得する (PropertyInstanceData は本当に必要になった時に new される)
		static std::unique_ptr<PropertyInstanceData>* getInstanceData_TextProperty(ReflectionObject* obj);

		// PropertyInstanceData のポインタ
		private: std::unique_ptr<PropertyInstanceData> instanceData_TextProperty = NULL;
*/

} // namespace tr
LN_NAMESPACE_END


