
#include <type_traits>
#include "../Base/Common.h"
#include "../Base/RefObject.h"
#include "../Base/String.h"
#include "../Base/EnumExtension.h"

LN_NAMESPACE_BEGIN
namespace tr
{
class ReflectionObject;
class ReflectionArrayObject;
struct ReflectionStruct;

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
	ArrayObject,
};

namespace detail
{
	extern void* enabler;

	class KindPrimitive {};
	class KindEnum {};
	class KindStruct {};
	class KindReflectionObject {};
	class KindReflectionArrayObject {};
}

/**
	@brief		���܂��܂ȃf�[�^�^���i�[�ł��铮�I�^�ϐ��ł��B
	@details	��ʓI�� Variant �� any �^�Ɣ�ׂāA�i�[�ł���f�[�^�^��������x�������邱�Ƃō�������}���Ă��܂��B
				�i�[�ł���f�[�^�^�̃T�C�Y�� 32byte �܂łł��B

	@section	��ʓI�Ȏg����
	~~~~~~~~~~~~~~~{.cpp}
	// �i�[
	Variant v1 = 10;
	Variant v2 = Point(5, 10);
	Variant v3 = "str";

	// �擾
	int a1 = Variant::Cast<int>(v1);
	Point a2 = Variant::Cast<Point>(v2);
	String a3 = Variant::Cast<String>(v3);
	~~~~~~~~~~~~~~~

	@note		Windows7 64Bit Corei7 - �i�[�Ǝ擾�� 1000000 ��s�������x��r�B
				- Variant		27,625us
				- boost::any	54,852us
*/
class Variant
{
public:
	static const Variant Null;


public:
	Variant();
	Variant(const Variant& value) : Variant() { Copy(value); }
	//Variant(std::nullptr_t value);
	//Variant(bool value) : Variant() { SetBool(value); }
	//Variant(int32_t value) : Variant() {}
	//Variant(float value);
	Variant(const TCHAR* value) : Variant() { SetString(value); }
	//Variant(const String& value);
	//Variant(const Enum& value) : Variant() { SetEnumValue(value.GetValue()); }
	////Variant(const ReflectionStruct& value) : Variant() { }
	//Variant(ReflectionObject* value) : Variant() { SetReflectionObject(value); }
	//Variant(ReflectionArrayObject* value) : Variant() { SetReflectionArrayObject(value); }

	~Variant() { Release(); }
	Variant& operator = (const Variant& obj) { Copy(obj); return (*this); }




	template<typename T, typename TKind> struct AccessorSelector {};

	//template<typename T, typename std::enable_if<std::is_pointer<T>::value>::type*& = detail::enabler>
	//Variant(const T& value)
	//{
	//	//using typeKind = first_enabled_t<
	//	//	std::enable_if<std::is_enum<T>::value, detail::KindEnum>,
	//	//	std::enable_if<std::is_pointer<T>::value, std::false_type>,
	//	//	detail::KindStruct>;
	//	//PodSetterSelector<T, typeKind>::SetValue(this, value);
	//}

	//// enum �����o or struct
	//template<typename T, typename std::enable_if<std::is_pod<T>::value>::type*& = detail::enabler>
	//Variant(const T& value)
	//{
	//	using typeKind = first_enabled_t<
	//		std::enable_if<std::is_enum<T>::value, detail::KindEnum>,
	//		std::enable_if<std::is_pointer<T>::value, std::false_type>,
	//		detail::KindStruct>;
	//	PodSetterSelector<T, typeKind>::SetValue(this, value);
	//}

	template<typename T>
	Variant(const T& value)
		: Variant()
	{
		AccessorSelectorHelper<T>::SetValue(this, value);
	}

	template<typename T>
	Variant(RefPtr<T>& value)
		: Variant(value.GetObjectPtr())
	{
	}


	//template<typename T, typename std::enable_if<std::is_enum<T>::value>::type*& = detail::enabler>
	//Variant(T value) { SetEnumValue(value); }	// T �� enum �����o�̏ꍇ�͂��̃R���X�g���N�^���Ă΂��B


	//template<typename T, typename std::enable_if<std::is_base_of<T, ReflectionStruct>::value>::type*& = detail::enabler>
	//Variant(const T& value) { }

	//template<typename T, typename std::enable_if<std::is_pod<T>::value>::type*& = detail::enabler>
	//Variant(T value) { }	// T �� POD �^�̏ꍇ�͂��̃R���X�g���N�^���Ă΂��B


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

	//template<typename T, typename std::enable_if<std::is_pointer<T>::value>::type*& = detail::enabler>
	//static T Cast(const Variant& value)
	//{
	//	using typeKind = first_enabled_t<
	//		std::enable_if<std::is_base_of<Enum, T>::value, detail::KindEnum>,
	//		std::enable_if<std::is_base_of<ReflectionListObject, T>::value, detail::KindReflectionListObject>,
	//		std::enable_if<std::is_base_of<ReflectionObject, T>::value, detail::KindReflectionObject>,
	//		detail::KindPrimitive>;
	//	return CastSelector<T, typeKind>::GetValue(value);
	//}

	/**
		@brief		�w�肵�� Variant �̒l���w�肵���^�ɃL���X�g����B
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

		//using typeKind = typename std::conditional <
		//	std::is_base_of<Enum, T>::value,
		//	detail::KindEnum,
		//	detail::KindPrimitive >::type;

		//using typeKind = first_enabled_t<
		//	std::enable_if<std::is_base_of<Enum, T>::value, detail::KindEnum>,
		//	std::enable_if<std::is_base_of<ReflectionListObject, T>::value, detail::KindReflectionListObject>,
		//	std::enable_if<std::is_base_of<ReflectionObject, T>::value, detail::KindReflectionObject>,
		//	detail::KindPrimitive>;

		// �����ŁuGetValue ����`����Ă��Ȃ��v�Ƃ����G���[���\�������ꍇ�AT �� Variant �������Ȃ��^���w�肳�ꂽ���Ƃ��Ӗ�����B
		//return CastSelector<T, typeKind>::GetValue(value);

		//template<typename T>
		//using bar = first_enabled_t<
		//	std::enable_if<typename std::is_base_of<Enum, T>::type, EnumSubClass>,
		//	std::enable_if<typename std::is_base_of<ReflectionListObject, T>::type, ReflectionListObjectSubClass>,
		//	std::enable_if<typename std::is_base_of<ReflectionObject, T>::type, ReflectionObjectSubClass>,
		//	PrimitiveTypeClass>;
		//return CastSelector<T, typename std::is_base_of<Enum, T>::type, typename std::is_base_of<RefPtrCore, T>::type >::GetValue(value);

		return AccessorSelectorHelper<T>::GetValue(value);
	}

private:
	void SetBool(bool value);
	bool GetBool() const;
	void SetString(const TCHAR* value);
	void SetString(const String& value);
	String GetString() const;
	void SetEnumValue(EnumValueType value);
	EnumValueType GetEnumValue() const;
	void SetStruct(const void* value, size_t size, const std::type_info& typeInfo);
	const void* GetStruct() const;
	void SetReflectionObject(ReflectionObject* obj);
	ReflectionObject* GetReflectionObject() const;
	void SetReflectionArrayObject(ReflectionArrayObject* obj);
	ReflectionArrayObject* GetReflectionArrayObject() const;

	//void SetValue(bool value) { SetBool(value); }
	//void SetValue(ReflectionObject* value) { SetReflectionObject(value); }
	//void SetValue(ReflectionArrayObject* value) { SetReflectionArrayObject(value); }

	// �l�^�p�� AccessorSelectorHelper
	template<typename T> struct AccessorSelectorHelper
	{
		using typeKind = first_enabled_t<
			std::enable_if<std::is_same<T, bool>::value, detail::KindPrimitive>,
			std::enable_if<std::is_same<T, String>::value, detail::KindPrimitive>,
			std::enable_if<std::is_base_of<Enum, T>::value, detail::KindEnum>,
			std::enable_if<std::is_enum<T>::value, detail::KindEnum>,
			std::enable_if<std::is_class<T>::value, detail::KindStruct>,		// ��POD�\���� or �N���X����
			std::enable_if<std::is_pod<T>::value, detail::KindStruct>,			// POD�\����
			std::false_type>;

		static void SetValue(Variant* variant, const T& value)
		{
			AccessorSelector<T, typeKind>::SetValue(variant, value);
		}
		static T GetValue(const Variant& value)
		{
			static_assert(std::is_same<T, const String&>::value == false, "Reference string type is not supported.");
			return AccessorSelector<T, typeKind>::GetValue(&value);
		}
	};
	// �|�C���^�^�p�� AccessorSelectorHelper
	template<typename T> struct AccessorSelectorHelper<T*>
	{
		using typeKind = first_enabled_t<
			std::enable_if<std::is_base_of<ReflectionArrayObject, T>::value, detail::KindReflectionArrayObject>,
			std::enable_if<std::is_base_of<ReflectionObject, T>::value, detail::KindReflectionObject>,
			std::false_type>;

		static void SetValue(Variant* variant, T* value)
		{
			AccessorSelector<T*, typeKind>::SetValue(variant, value);
		}
		static T* GetValue(const Variant& value)
		{
			return AccessorSelector<T*, typeKind>::GetValue(&value);
		}
	};
	/* ��struct �^�̔��ʂ� is_pod ���g�p���Ă��邪�Ais_pod �̓|�C���^�^�� true �Ƃ݂Ȃ��Ă��܂����߁A�O�i�ŕ�����K�v������B
	 * �܂��A�|�C���^�͂��̎��̂̌^�ɑ΂��� is_base_of �������̂ŁA������܂��O�i�ŕ�����K�v������B
	 * �����񃊃e�����̓|�C���^�^�ł͂Ȃ��z��^�ƂȂ�B�R�������Ɍ����ē��ꉻ����̂��ʓ|�Ȃ̂ŁA�R���X�g���N�^�I�[�o�[���[�h�ŉ������Ă���B
	 */

	//template<typename T> struct CastValueOrPointerSelector
	//{
	//	static T GetValue(const Variant& value)
	//	{
	//		using typeKind = first_enabled_t<
	//			std::enable_if<std::is_same<T, bool>::value, detail::KindPrimitive>,
	//			std::enable_if<std::is_base_of<Enum, T>::value, detail::KindEnum>,
	//			std::enable_if<std::is_enum<T>::value, detail::KindEnum>,
	//			std::enable_if<std::is_pod<T>::value, detail::KindPodStruct>,
	//			std::false_type>;
	//		return AccessorSelector<T, typeKind>::GetValue(&value);
	//	}
	//};
	//template<typename T> struct CastValueOrPointerSelector<T*>
	//{
	//	static T* GetValue(const Variant& value)
	//	{
	//		// Cast<T>() �� T �̓|�C���^�^�ł���ꍇ�����ɗ���B
	//		// KindReflectionArrayObject �܂��� KindReflectionObject �̃T�u�N���X�ł��邩���m�F����B
	//		using typeKind = first_enabled_t<
	//			std::enable_if<std::is_base_of<ReflectionArrayObject, T>::value, detail::KindReflectionArrayObject>,
	//			std::enable_if<std::is_base_of<ReflectionObject, T>::value, detail::KindReflectionObject>,
	//			std::false_type>;
	//		return AccessorSelector<T*, typeKind>::GetValue(&value);
	//	}
	//};


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
	template<typename T> struct CastSelector < T, std::true_type > { static T GetValue(const Variant& v) { return *((T*)(&v.m_enum)); } };	// TODO: �^�`�F�b�N
//	template<typename TRefPtr, typename U> struct CastSelector < TRefPtr<U>, std::true_type > { static TRefPtr<U> GetValue(const Variant& v) { return TRefPtr<U>(static_cast<U>(v.GetObject())); } };	// TODO: �^�`�F�b�N
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
		ln::detail::GenericStringCore<TCHAR>*	m_string;
		EnumValueType			m_enum;
		ReflectionObject*		m_object;
		ReflectionArrayObject*	m_arrayObject;
		byte_t					m_struct[32];
	};
	size_t			m_structSize;
	//String			m_string;
	const std::type_info*		m_typeInfo;
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<> struct Variant::AccessorSelector<bool, detail::KindPrimitive>
{
	static void SetValue(Variant* v, bool value) { v->SetBool(value); }
	static bool GetValue(const Variant* v) { return v->GetBool(); }
};
template<> struct Variant::AccessorSelector<String, detail::KindPrimitive>
{
	static void SetValue(Variant* v, const String& value) { v->SetString(value); }
	static String GetValue(const Variant* v) { return v->GetString(); }		// �߂�l�͎Q�Ƃł̓_��
};
template<typename T> struct Variant::AccessorSelector<T, detail::KindEnum>
{
	static void SetValue(Variant* v, T value) { v->SetEnumValue(value); }
	static T GetValue(const Variant* v) { return static_cast<typename T::enum_type>(v->GetEnumValue()); }
};
template<typename T> struct Variant::AccessorSelector<T, detail::KindReflectionObject>
{
	static void SetValue(Variant* v, T value) { v->SetReflectionObject(value); }
	static T GetValue(const Variant* v) { return static_cast<T>(v->GetReflectionObject()); }
};
template<typename T> struct Variant::AccessorSelector<T, detail::KindReflectionArrayObject>
{
	static void SetValue(Variant* v, T value) { v->SetReflectionArrayObject(value); }
	static T GetValue(const Variant* v) { return static_cast<T>(v->GetReflectionArrayObject()); }
};
template<typename T> struct Variant::AccessorSelector<T, detail::KindStruct>
{
	static void SetValue(Variant* v, T value) { v->SetStruct(&value, sizeof(T), typeid(T)); }
	static T GetValue(const Variant* v) { return *((T*)v->GetStruct()); }
};
template<typename T> struct Variant::AccessorSelector<const T&, detail::KindStruct>	// const Struct& �̂悤�ȎQ�Ƃł� get ���T�|�[�g����
{
	static const T& GetValue(const Variant* v) { return *((T*)v->GetStruct()); }
};

} // namespace tr
LN_NAMESPACE_END

