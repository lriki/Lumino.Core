
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
		@brief	�w�肵���I�u�W�F�N�g�̌^�����擾���܂��B
	*/
	static TypeInfo* GetTypeInfo(const ReflectionObject* obj);

	/**
		@brief	�^�����Ɏw�肵���N���X�^�̌^�����擾���܂��B
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
		@brief	�N���X�����擾���܂��B
	*/
	const String& GetName() const { return m_name; }

	void RegisterProperty(Property* prop);
	Property* FindProperty(const String& name) const;

	//// childObjProp ���p���ł���v���p�e�B������ TypeInfo ����T���B������Ȃ���� NULL ��Ԃ��B
	//// childObj : childObjProp �����I�u�W�F�N�g
	//Property* FindInheritanceProperty(const Property* childObjProp, CoreObject* childObj) const;

	//void RegisterRoutedEvent(RoutedEvent* ev);
	//RoutedEvent* FindRoutedEvent(const String& name) const;	// TODO: ����Ȃ�����
	//void InvokeRoutedEvent(CoreObject* owner, const RoutedEvent* ev, RoutedEventArgs* e);

	///// RoutedEventHandler �́A���[�U�[�����I�ɒǉ��ł���n���h�������O�ɌĂяo�����B
	///// WPF �ł́u�ÓI�n���h���v�ƌĂ΂�Ă���B���I�C�x���g�ɓo�^����̂ɔ�ׁA���������g�p���Ȃ��B
	//void RegisterRoutedEventHandler(const RoutedEvent* ev, RoutedEventHandler* handler);
	//RoutedEventHandler* FindRoutedEventHandler(const RoutedEvent* ev) const;

	//TypeInfo* GetBaseClass() const { return m_baseClass; }
	//uint32_t* GetHasLocalValueFlags(CoreObject* obj) { return m_hasLocalValueFlagsGetter(obj); }

	///// �x�[�X�N���X���܂߂��S�Ẵv���p�e�B��񋓂���
	//static void ForEachAllProperty(const TypeInfo* typeInfo, const std::function<void(Property*)>& callback);


	void SetBindingTypeInfo(void* data);
	static void* GetBindingTypeInfo(const ReflectionObject* obj);

	bool operator == (const TypeInfo& info) const { return m_name == info.m_name; }
	bool operator < (const TypeInfo& info) const { return m_name < info.m_name; }

protected:

private:
	//typedef SortedArray<const RoutedEvent*, RoutedEventHandler*>	RoutedEventHandlerList;

	String						m_name;						// �N���X��
	TypeInfo*					m_baseClass;				// �p�����N���X������ TypeInfo
	Array<Property*>			m_propertyList;				// ���̌^�̃N���X�����v���p�e�B�̃��X�g
	HasLocalValueFlagsGetter	m_hasLocalValueFlagsGetter;	// �v���p�e�B�����[�J���l��ێ����Ă��邩�������r�b�g�t���O���擾����R�[���o�b�N�B���[�J���l�������Ȃ��ꍇ�̓v���p�e�B�̒l��e����p������B
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
