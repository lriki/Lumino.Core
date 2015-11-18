
#include <type_traits>
#include "../Base/Common.h"
#include "../Base/RefObject.h"
#include "../Base/String.h"
#include "../Base/EnumExtension.h"

LN_NAMESPACE_BEGIN
namespace tr
{
class ReflectionObject;
class ReflectionListObject;

enum class VariantType
{
	Null,
	Bool,
	Int32,
	Float,
	String,
	Enum,
	Struct,
	Object,
	ListObject,
};

namespace details
{
	extern void* enabler;
}

/**
	@brief		
*/
class Variant
{
public:
	static const Variant Null;

	class KindPrimitive {};
	class KindEnum {};
	class KindReflectionObject {};
	class KindReflectionListObject {};

public:
	Variant();
	Variant(const Variant& value);
	Variant(std::nullptr_t value);
	Variant(bool value) { Set(value); }
	Variant(int32_t value) {}
	Variant(float value);
	Variant(const TCHAR* value);
	Variant(const String& value);
	Variant(const Enum& value) { SetEnumValue(value.GetValue()); }
	Variant(ReflectionObject* value);
	Variant(ReflectionListObject* value);

	~Variant() { Release(); }
	Variant& operator = (const Variant& obj) { Copy(obj); return (*this); }


	template<typename T, typename std::enable_if<std::is_enum<T>::value>::type*& = details::enabler>
	Variant(T value) { SetEnumValue(value); }	// T が enum メンバの場合はこのコンストラクタが呼ばれる。


	//template<typename T, EnumSubClass>
	//Variant(RefPtr<T>& obj)
	//{
	//}

	//template<class T>
	//Variant(RefPtr<T>& obj);

public:
	VariantType GetType() const { return m_type; }



	template<typename ...Args>
	struct first_enabled {};

	template<typename T, typename ...Args>
	struct first_enabled<std::enable_if<true, T>, Args...> { using type = T; };
	template<typename T, typename ...Args>
	struct first_enabled<std::enable_if<false, T>, Args...> : first_enabled<Args...>{};
	template<typename T, typename ...Args>
	struct first_enabled<T, Args...> { using type = T; };

	template<typename ...Args>
	using first_enabled_t = typename first_enabled<Args...>::type;


	
	
	template<bool condition, typename IfTrue, typename IfFalse>
	using conditional_t = typename std::conditional<condition, IfTrue, IfFalse>::type;

	/**
		@brief		指定した Variant の値を指定した型にキャストする。
		@code
					UIElement* item = Variant::Cast<UIElement*>(value);
		@endcode
	*/
	template<typename T>
	static T Cast(const Variant& value)
	{
		//using bar = typename std::conditional < typename std::is_base_of<Enum, T>::value, EnumSubClass, PrimitiveTypeClass >;
		//typedef
		//	typename std::conditional <
		//	std::is_base_of<Enum, T>::value,
		//	EnumSubClass,
		//	PrimitiveTypeClass >::type
		//	bar;
		using typeKind = typename std::conditional <
			std::is_base_of<Enum, T>::value,
			KindEnum,
			KindPrimitive >::type;
		return CastSelector<T, typeKind>::GetValue(value);

		//template<typename T>
		//using bar = first_enabled_t<
		//	std::enable_if<typename std::is_base_of<Enum, T>::type, EnumSubClass>,
		//	std::enable_if<typename std::is_base_of<ReflectionListObject, T>::type, ReflectionListObjectSubClass>,
		//	std::enable_if<typename std::is_base_of<ReflectionObject, T>::type, ReflectionObjectSubClass>,
		//	PrimitiveTypeClass>;
		//return CastSelector<T, typename std::is_base_of<Enum, T>::type, typename std::is_base_of<RefPtrCore, T>::type >::GetValue(value);
	}

private:
	void Set(bool value);
	bool GetBool() const;
	void SetEnumValue(EnumValueType value);
	EnumValueType GetEnumValue() const;


	template<typename T, typename TType> struct CastSelector { static T GetValue(const Variant& v) { return 0; } };
	


#if 0
	template<typename T, typename TIsEnum> struct CastSelector { static T GetValue(const Variant& v) { return static_cast<T>(v.GetObject()); } };
	//template<typename TRefPtr, typename TIsEnum> struct CastSelector { static TRefPtr GetValue(const Variant& v) { return TRefPtr(static_cast<TRefPtr::PtrType>(v.GetObject())); } };

	template<> struct CastSelector < bool, std::false_type >		{ static bool GetValue(const Variant& v) { return v.GetBool(); } };
	template<> struct CastSelector < int, std::false_type >		{ static int GetValue(const Variant& v) { return v.GetInt(); } };
	template<> struct CastSelector < float, std::false_type >		{ static float GetValue(const Variant& v) { return v.GetFloat(); } };
	template<> struct CastSelector < String, std::false_type >		{ static String GetValue(const Variant& v) { return v.GetString(); } };
	template<> struct CastSelector < Rect, std::false_type >		{ static Rect GetValue(const Variant& v) { return v.GetRect(); } };
	template<> struct CastSelector < SizeF, std::false_type >		{ static SizeF GetValue(const Variant& v) { return v.GetSizeF(); } };
	template<> struct CastSelector < ThicknessF, std::false_type > { static const ThicknessF& GetValue(const Variant& v) { return v.GetThicknessF(); } };
	template<typename T> struct CastSelector < T, std::true_type > { static T GetValue(const Variant& v) { return *((T*)(&v.m_enum)); } };	// TODO: 型チェック
//	template<typename TRefPtr, typename U> struct CastSelector < TRefPtr<U>, std::true_type > { static TRefPtr<U> GetValue(const Variant& v) { return TRefPtr<U>(static_cast<U>(v.GetObject())); } };	// TODO: 型チェック
#endif

public:
	//bool operator == (const Variant& right) const;

private:
	void Copy(const Variant& obj);
	void Release();

private:
	VariantType	m_type;
	union
	{
		bool					m_bool;
		uint32_t				m_int32;
		float					m_float;
		EnumValueType			m_enum;
		ReflectionObject*		m_object;
		ReflectionListObject*	m_valueList;
	};
	String			m_string;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<> struct Variant::CastSelector<bool, Variant::KindPrimitive>		{ static bool GetValue(const Variant& v) { return v.GetBool(); } };
template<typename T> struct Variant::CastSelector<T, Variant::KindEnum>		{ static T GetValue(const Variant& v) { return static_cast<T::enum_type>(v.GetEnumValue()); } };

} // namespace tr
LN_NAMESPACE_END

