
#pragma once
#include <memory>
#include "Common.h"
#include "../Base/RefObject.h"
#include "../Base/String.h"
#include "PropertyMetadata.h"

#define LN_MEMBER_OFFSETOF(type, member)  \
    (reinterpret_cast<std::size_t>(  \
      &reinterpret_cast<char const volatile&>(  \
        ((type*)0)->member)))

LN_NAMESPACE_BEGIN
namespace tr
{

template<typename TValue>
class Property2;

// TODO; Internal
typedef void(*PropertyChangedCallback)(ReflectionObject* obj, PropertyChangedEventArgs* e);
typedef PropertyInstanceData*(*InstanceDataGetterFunc)(ReflectionObject* obj);

// TODO; Internal
struct PropertyInstanceData
//	: public RefObject	// TODO: これもいらないかも
{
//public:
	PropertySetSource	baseValueSource = PropertySetSource::Default;

	//// TODO: 以下、昔の
	//Property*			InheritanceKey;		// プロパティを親から継承するとき、this またはこの値をキーとして検索する。
	//ReflectionObject*	InheritanceParent;	// ↑ので見つかった親あるいは祖先オブジェクト
	//const Property*		InheritanceTarget;	// ↑のオブジェクトのどのプロパティから受け継ぐか (走査の結果継承元が見つからなかった場合、この PropertyInstanceData が表す Propery を指す。このとき InheritanceParent は NULL)
	//uint16_t			RevisionCount;
	//uint16_t			PathRevisionCount;
	//bool				IsDefault;

	//PropertyInstanceData()
	//	: InheritanceTarget(nullptr)
	//	, InheritanceParent(nullptr)
	//	, RevisionCount(0)
	//	, PathRevisionCount(0)
	//	, IsDefault(true)
	//{}
};

/**
	@brief		ReflectionObject のサブクラスが実装できるプロパティを表します。
	@details	プロパティは、メンバ変数に対する簡易的なリフレクションを提供します。		
*/
class Property
{
public:
	Property(TypeInfo* ownerClassType, PropertyMetadata* metadata, size_t memberOffset, bool stored);
	virtual ~Property();

	virtual void SetValue(ReflectionObject* target, Variant value, PropertySetSource source) const { LN_THROW(0, InvalidOperationException); }
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
	static void SetPropertyValue(ReflectionObject* obj, const Property* prop, const Variant& value, PropertySetSource source = PropertySetSource::ByLocal);
	
	/**
		@brief		指定したオブジェクトのプロパティの値を取得します。
	*/
	static Variant GetPropertyValue(ReflectionObject* obj, const Property* prop);

	/**
		@brief		指定したオブジェクトのプロパティの値を設定します。あらかじめ型が分かっている場合、SetPropertyValue() よりも少ないオーバーヘッドで設定できます。
	*/
	template<typename TValue>
	static void SetPropertyValueDirect(ReflectionObject* obj, const Property* prop, const TValue& value, PropertySetSource source = PropertySetSource::ByLocal);

	/**
		@brief		指定したオブジェクトのプロパティの値を取得します。あらかじめ型が分かっている場合、GetPropertyValue() よりも少ないオーバーヘッドで設定できます。
	*/
	template<typename TValue>
	static const TValue& GetPropertyValueDirect(const ReflectionObject* obj, const Property* prop);


	static PropertyInstanceData* GetInstanceData(ReflectionObject* obj, const Property* prop)
	{
		//if (prop->m_instanceDataGetterFunc == nullptr) return nullptr;
		return prop->m_instanceDataGetterFunc(obj);

		//std::unique_ptr<PropertyInstanceData>* pp = prop->m_instanceDataGetterFunc(obj);
		//if ((*pp) == nullptr) pp->reset(LN_NEW PropertyInstanceData());
		//return pp->get();
	}

	static void SetBaseValueSource(ReflectionObject* obj, const Property* prop, PropertySetSource source)
	{
		PropertyInstanceData* data = GetInstanceData(obj, prop);
		data->baseValueSource = source;
	}

	static PropertySetSource GetBaseValueSource(ReflectionObject* obj, const Property* prop)
	{
		PropertyInstanceData* data = GetInstanceData(obj, prop);
		//if (data == nullptr) return PropertySetSource::Default;
		return data->baseValueSource;
	}

	static void NotifyPropertyChanged(ReflectionObject* target, const Property* prop, PropertySetSource source);

private:
	friend class TypeInfo;
	TypeInfo*			m_ownerClassType;
	PropertyMetadata*	m_metadata;
	size_t				m_memberOffset;
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
	typedef Property2<TValue>*(*GetPrpertyPtrFunc)(ReflectionObject* obj);
	typedef void(*SetterFunc)(ReflectionObject* obj, TValue& value);
	typedef void(*GetterFunc)(ReflectionObject* obj, TValue** valuePtr);
	typedef void(*OnPropertyChangedFunc)(ReflectionObject* obj, PropertyChangedEventArgs* e);
	// ↑※static 関数のポインタでないと、言語バインダを作りにくくなる。

public:
	TypedProperty(TypeInfo* ownerTypeInfo, const TCHAR* name, size_t memberOffset, PropertyMetadata* metadata)
		: Property(ownerTypeInfo, metadata, memberOffset, false)
		, m_name(name)
		, m_setter(nullptr)
		, m_getter(nullptr)
	{}

	virtual ~TypedProperty() {}

public:
	virtual const String& GetName() const { return m_name; }

	virtual void SetValue(ReflectionObject* target, Variant value, PropertySetSource source) const
	{
		SetValueDirect(target, Variant::Cast<TValue>(value), source);
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

	void SetValueDirect(ReflectionObject* target, const TValue& value, PropertySetSource source) const
	{
		LN_CHECK_STATE(m_setter != nullptr);

		// 独自の値を持つことを示すフラグを立てる
		uint32_t f = 0x1 << GetLocalIndex();
		uint32_t* flags = GetOwnerClassType()->GetHasLocalValueFlags(target);
		(*flags) |= f;

		SetBaseValueSource(target, this, source);

		if (m_getter != nullptr)
		{
			TValue* v;
			m_getter(target, &v);
			m_setter(target, const_cast<TValue&>(value));
			NotifyPropertyChanged(target, this, source);

		}
		else
		{
			m_setter(target, const_cast<TValue&>(value));
			NotifyPropertyChanged(target, this, source);
		}
	}
	const TValue& GetValueDirect(const ReflectionObject* target) const
	{
		LN_THROW(m_getter != NULL, InvalidOperationException);
		TValue* v;
		m_getter(const_cast<ReflectionObject*>(target), &v);
		return *v;
	}

#if 0
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
#endif


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
	GetPrpertyPtrFunc		m_getPropPtr;
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
	typedef Property2<TValue>*(*GetPrpertyPtrFunc)(ReflectionObject* obj);
	typedef void(*SetterFunc)(ReflectionObject* obj, TValue& value);
	typedef void(*GetterFunc)(ReflectionObject* obj, TValue** outValuePtr);

	//template<typename TPropertyChangedCallbackCaster>
	TypedPropertyInitializer(
		TypedProperty<TValue>* prop,
		GetPrpertyPtrFunc getPropPtr,
		SetterFunc setter,
		GetterFunc getter,
		InstanceDataGetterFunc instanceDataGetter)
	{
		prop->m_getPropPtr = getPropPtr;
		prop->m_setter = setter;
		prop->m_getter = getter;
		prop->m_instanceDataGetterFunc = instanceDataGetter;
	}
};



//------------------------------------------------------------------------------
template<typename TValue>
void Property::SetPropertyValueDirect(ReflectionObject* obj, const Property* prop, const TValue& value, PropertySetSource source)
{
	LN_CHECK_ARG(obj != nullptr);
	LN_CHECK_ARG(prop != nullptr);
	auto t = static_cast<const TypedProperty<TValue>*>(prop);
	t->SetValueDirect(obj, value, source);
}

//------------------------------------------------------------------------------
template<typename TValue>
const TValue& Property::GetPropertyValueDirect(const ReflectionObject* obj, const Property* prop)
{
	LN_CHECK_ARG(obj != nullptr);
	LN_CHECK_ARG(prop != nullptr);
	auto t = static_cast<const TypedProperty<TValue>*>(prop);
	return t->GetValueDirect(obj);
}


class PropertyHelper
{
public:

	template<typename TValue, typename TProperty>
	static void SetValue(TProperty& field, TValue& value)
	{
		field.Set(value);
	}
	template<typename TValue, typename TProperty>
	static void GetValue(TProperty& field, TValue** outValuePtr)
	{
		*outValuePtr = &field.m_value;
	}

	template<class T, class C>
	static std::size_t MemberOffsetOf(T(C::*pm)) LN_CONSTEXPR
	{
		return reinterpret_cast<std::size_t>(
			&reinterpret_cast<const volatile char&>(((C*)0)->*pm)
			);
	}
};


#define LN_TR_PROPERTY(valueType, propVar) \
	private: static ::ln::tr::Property2<valueType>*					get_Ptr##propVar(ln::tr::ReflectionObject* obj); \
	private: static void											set_##propVar(ln::tr::ReflectionObject* obj, valueType& value); \
	private: static void											get_##propVar(ln::tr::ReflectionObject* obj, valueType** outValuePtr); \
	private: static ln::tr::TypedPropertyInitializer<valueType>		init_##propVar; \
	private: static ln::tr::PropertyInstanceData*					getInstanceData_##propVar(ln::tr::ReflectionObject* obj); \
	private: ln::tr::PropertyInstanceData							instanceData_##propVar; \
	public:  static const ln::tr::Property*							propVar;

#define LN_TR_PROPERTY_IMPLEMENT(ownerClass, valueType, propVar, propName, memberVar, metadata) \
	ln::tr::PropertyMetadata						ownerClass_##metadata_##propVar = metadata; \
	static ln::tr::TypedProperty<valueType>			_##propVar(ln::tr::TypeInfo::GetTypeInfo<ownerClass>(), _T(propName), LN_MEMBER_OFFSETOF(ownerClass, memberVar), &ownerClass_##metadata_##propVar); \
	const ln::tr::Property*							ownerClass::propVar = &_##propVar; \
	::ln::tr::Property2<valueType>*					ownerClass::get_Ptr##propVar(ln::tr::ReflectionObject* obj) { return &static_cast<ownerClass*>(obj)->memberVar; } \
	void											ownerClass::set_##propVar(ln::tr::ReflectionObject* obj, valueType& value) { tr::PropertyHelper::SetValue(static_cast<ownerClass*>(obj)->memberVar, value); } \
	void											ownerClass::get_##propVar(ln::tr::ReflectionObject* obj, valueType** outValuePtr) { tr::PropertyHelper::GetValue<valueType>(static_cast<ownerClass*>(obj)->memberVar, outValuePtr); } \
	ln::tr::TypedPropertyInitializer<valueType>		ownerClass::init_##propVar(&_##propVar, &ownerClass::get_Ptr##propVar, &ownerClass::set_##propVar, &ownerClass::get_##propVar, ownerClass::getInstanceData_##propVar); \
	ln::tr::PropertyInstanceData*					ownerClass::getInstanceData_##propVar(ln::tr::ReflectionObject* obj) { return &(static_cast<ownerClass*>(obj)->instanceData_##propVar); }


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


















template<typename TValue>
class Property2
{
public:
	Property2(ReflectionObject* owner)
		: m_owner(owner)
		, m_propId(nullptr)
		, m_value()
		, m_valueSource(PropertySetSource::Default)
	{
	}

	Property2(ReflectionObject* owner, const TValue& value)
		: m_owner(owner)
		, m_propId(nullptr)
		, m_value(value)
		, m_valueSource(PropertySetSource::Default)
	{}

	~Property2()
	{}

	void Set(const TValue& value)
	{
		TryInitialize();
		if (m_value != value)
		{
			m_value = value;
			m_valueSource = PropertySetSource::ByLocal;
			Property::NotifyPropertyChanged(m_owner, m_propId, m_valueSource);
		}
	}

	const TValue& Get() const { return m_value; }

	Property2& operator = (const TValue& value) { Set(value); return *this; }
	operator const TValue&() const { return m_value; }

	//bool operator == (const Property2& right) const { return m_value == right.m_value; }
	//bool operator == (const TValue& right) const { return m_value == right; }
	//bool operator != (const Property2& right) const { return operator!=(right); }
	//bool operator != (const TValue& right) const { return operator!=(right); }

	//friend bool operator == (const Property2& lhs, const TValue& rhs);
	//friend bool operator == (const TValue& lhs, const Property2& rhs);
	//friend bool operator != (const Property2& lhs, const TValue& rhs);
	//friend bool operator != (const TValue& lhs, const Property2& rhs);

private:
	LN_DISALLOW_COPY_AND_ASSIGN(Property2);

	void TryInitialize()
	{
		if (m_propId == nullptr)
		{
			size_t offset = ((intptr_t)this) - ((intptr_t)m_owner);
			TypeInfo* info = TypeInfo::GetTypeInfo(m_owner);
			m_propId = info->FindProperty(offset);
		}
	}

	ReflectionObject*	m_owner;
	const Property*		m_propId;
	TValue				m_value;
	PropertySetSource	m_valueSource;

	friend class PropertyHelper;
};


template<typename TValue>
inline bool operator == (const Property2<TValue>& lhs, const TValue& rhs)
{
	return lhs.Get() == rhs;
}
template<typename TValue>
inline bool operator == (const TValue& lhs, const Property2<TValue>& rhs)
{
	return lhs == rhs.Get();
}
template<typename TValue>
inline bool operator != (const Property2<TValue>& lhs, const TValue& rhs)
{
	return lhs.Get() != rhs;
}
template<typename TValue>
inline bool operator != (const TValue& lhs, const Property2<TValue>& rhs)
{
	return lhs != rhs.Get();
}

} // namespace tr
LN_NAMESPACE_END


