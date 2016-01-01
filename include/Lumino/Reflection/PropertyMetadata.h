
#pragma once
#include "../Base/Common.h"
#include "../Base/EnumExtension.h"
#include "Variant.h"

LN_NAMESPACE_BEGIN
namespace tr
{
class Property;
	
/* プロパティの動作オプションです。*/
LN_ENUM_FLAGS(PropertyOptions)
{
	None = 0x0000,			/**< オプション指定無し。*/
	Inherits = 0x0001,		/**< 子要素によって継承されます。*/
};
LN_ENUM_FLAGS_DECLARE(PropertyOptions);

/**
	@brief	プロパティの初期値などのメタデータを定義します。
*/
class PropertyMetadata
{
	// メンバ関数ポインタは void* に変換できない。そのためラップするクラスを用意している
	class CallbackWrapper
	{
	public:
		virtual ~CallbackWrapper() {}
	};

	template<typename TCallback>
	class SimpleCallbackWrapper : public CallbackWrapper
	{
	public:
		SimpleCallbackWrapper(TCallback callback) : m_callback(callback) {}
		TCallback GetCallback() { return m_callback; }
		TCallback m_callback;
	};

public:
	PropertyMetadata()
	{
		m_defaultValue = Variant::Null;
		m_propertyChangedCallback = NULL;
		m_options = PropertyOptions::None;
		m_inheritanceTarget = NULL;
	}

	PropertyMetadata(const Variant& defaultValue)
		: PropertyMetadata()
	{
		m_defaultValue = defaultValue;
	}
	template<typename TOwnerClass>
	PropertyMetadata(const Variant& defaultValue, void(TOwnerClass::*propertyChanged)(PropertyChangedEventArgs*))
		: PropertyMetadata()
	{
		m_defaultValue = defaultValue;
		m_propertyChangedCallback = LN_NEW SimpleCallbackWrapper< void(TOwnerClass::*)(PropertyChangedEventArgs*) >(propertyChanged);
	}
	template<typename TOwnerClass>
	PropertyMetadata(const Variant& defaultValue, PropertyOptions options, void(TOwnerClass::*propertyChanged)(PropertyChangedEventArgs*))
		: PropertyMetadata()
	{
		m_defaultValue = defaultValue;
		m_propertyChangedCallback = LN_NEW SimpleCallbackWrapper< void(TOwnerClass::*)(PropertyChangedEventArgs*) >(propertyChanged);
		m_options = options;
	}
	template<typename TOwnerClass>
	PropertyMetadata(const Variant& defaultValue, PropertyOptions options, Property* inheritanceKey, void(TOwnerClass::*propertyChanged)(PropertyChangedEventArgs*))
		: PropertyMetadata()
	{
		m_defaultValue = defaultValue;
		m_propertyChangedCallback = LN_NEW SimpleCallbackWrapper< void(TOwnerClass::*)(PropertyChangedEventArgs*) >(propertyChanged);
		m_options = options;
		m_inheritanceKey = inheritanceKey;
	}

	~PropertyMetadata()
	{
		LN_SAFE_DELETE(m_propertyChangedCallback);
	}

public:
	const Variant& GetDefaultValue() const { return m_defaultValue; }
	template<typename TCallback>
	TCallback GetPropertyChangedCallback()
	{
		if (m_propertyChangedCallback == NULL) { return NULL; }
		return static_cast< SimpleCallbackWrapper<TCallback>* >(m_propertyChangedCallback)->GetCallback();
	}
	PropertyOptions GetPropertyOptions() const { return m_options; }
	Property* GetInheritanceTarget() const { return m_inheritanceTarget; }

private:
	Variant					m_defaultValue;
	CallbackWrapper*		m_propertyChangedCallback;
	PropertyOptions			m_options;
	Property*				m_inheritanceTarget;
};

} // namespace tr
LN_NAMESPACE_END
