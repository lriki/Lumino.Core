
#include "../Base/Common.h"
#include "../Base/RefObject.h"
#include "../Base/String.h"

LN_NAMESPACE_BEGIN
namespace tr
{
class ReflectionObject;
class Property;
typedef uint64_t LocalValueHavingFlags;

/**
	@brief		
*/
class TypeInfo
{
public:
	typedef uint64_t* (*HasLocalValueFlagsGetter)(ReflectionObject* _this);
	typedef uint8_t RevisionCount;
	static const int MaxProperties = sizeof(uint64_t) * 8;

public:
	TypeInfo(const TCHAR* className, TypeInfo* baseClass, HasLocalValueFlagsGetter getter);

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


	bool operator == (const TypeInfo& info) const { return m_name == info.m_name; }
	bool operator < (const TypeInfo& info) const { return m_name < info.m_name; }

protected:

private:
	//typedef SortedArray<const RoutedEvent*, RoutedEventHandler*>	RoutedEventHandlerList;

	String						m_name;						// �N���X��
	TypeInfo*					m_baseClass;				// �p�����N���X������ TypeInfo
	Array<Property*>			m_propertyList;				// ���̌^�̃N���X�����v���p�e�B�̃��X�g
	HasLocalValueFlagsGetter	m_hasLocalValueFlagsGetter;	// �v���p�e�B�����[�J���l��ێ����Ă��邩�������r�b�g�t���O���擾����R�[���o�b�N�B���[�J���l�������Ȃ��ꍇ�̓v���p�e�B�̒l��e����p������B

	//Array<RoutedEvent*>		m_routedEventList;
	//RoutedEventHandlerList	m_routedEventHandlerList;
	//
};

} // namespace tr
LN_NAMESPACE_END
