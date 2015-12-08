
#pragma once
#include "../Base/Common.h"
#include "../Base/RefObject.h"
#include "../Base/String.h"
#include "PropertyMetadata.h"

LN_NAMESPACE_BEGIN
namespace tr
{
class ReflectionObject;
class TypeInfo;
class PropertyMetadata;

// TODO; Internal
class PropertyInstanceData
	: public RefObject
{
public:
	Property*			InheritanceKey;		// �v���p�e�B��e����p������Ƃ��Athis �܂��͂��̒l���L�[�Ƃ��Č�������B
	ReflectionObject*	InheritanceParent;	// ���̂Ō��������e���邢�͑c��I�u�W�F�N�g
	const Property*		InheritanceTarget;	// ���̃I�u�W�F�N�g�̂ǂ̃v���p�e�B����󂯌p���� (�����̌��ʌp������������Ȃ������ꍇ�A���� PropertyInstanceData ���\�� Propery ���w���B���̂Ƃ� InheritanceParent �� NULL)
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
	@brief		ReflectionObject �̃T�u�N���X�������ł���v���p�e�B��\���܂��B
	@details	�v���p�e�B�́A�����o�ϐ��ɑ΂���ȈՓI�ȃ��t���N�V������񋟂��܂��B		
*/
class Property
{
public:
	Property(TypeInfo* ownerClassType, PropertyMetadata* metadata);


	virtual void SetValue(ReflectionObject* target, Variant value) const { LN_THROW(0, InvalidOperationException); }
	virtual Variant GetValue(const ReflectionObject* target) const { LN_THROW(0, InvalidOperationException); }


};




#define LN_TR_PROPERTY(valueType, propVar) \
	public:  static const ln::tr::Property*	propVar##; \
	private: static void  set_##propVar(CoreObject* obj, valueType value); \
	private: static valueType get_##propVar(const CoreObject* obj); \
	private: static void  changed_##propVar(CoreObject* obj, PropertyChangedEventArgs* e); \
	private: static TypedPropertyInitializer<valueType> init_##propVar; \
	private: static PropertyMetadata metadata_##propVar; \
	private: static std::shared_ptr<PropertyInstanceData>* getInstanceData_##propVar(CoreObject* obj); \
	private: std::shared_ptr<PropertyInstanceData> instanceData_##propVar = NULL;
	// ��TODO: unique_ptr �̕����������g��Ȃ������H

/*
	LN_TR_PROPERTY(String, TextProperty);

		�ȉ��̂悤�ɓW�J�����B

	public:
		static const ln::tr::Property*	TextProperty;

	private:
		// �w�肵���I�u�W�F�N�g�ɒl��ݒ肷��
		static void	 set_TextProperty(CoreObject* obj, String value);

		// �w�肵���I�u�W�F�N�g�̒l���擾����
		static String get_TextProperty(const CoreObject* obj);

		// �w�肵���I�u�W�F�N�g�ɑ΂��āAPropertyMetadata �ɓo�^����Ă��� PropertyChangedCallback ���Ăяo��
		static void changed_TextProperty(CoreObject* obj, PropertyChangedEventArgs* e);

		// Property �I�u�W�F�N�g�̏��������s��
		static PropertyInitializer<valueType> init_TextProperty;

		// �v���p�e�B�̃��^����`
		static PropertyMetadata metadata_TextProperty;

		// �I�u�W�F�N�g�̎��� PropertyInstanceData �̃|�C���^�ւ̎Q�Ƃ��擾���� (PropertyInstanceData �͖{���ɕK�v�ɂȂ������� new �����)
		static std::shared_ptr<PropertyInstanceData>* getInstanceData_##propVar(CoreObject* obj);

		// PropertyInstanceData �̃|�C���^
		private: std::shared_ptr<PropertyInstanceData> instanceData_TextProperty = NULL;
*/

} // namespace tr
LN_NAMESPACE_END


