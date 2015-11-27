
#pragma once
#include "../Base/Common.h"
#include "../Base/RefObject.h"
#include "../Base/String.h"

LN_NAMESPACE_BEGIN
namespace tr
{
class ReflectionObject;
class TypeInfo;
class PropertyMetadata;

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

} // namespace tr
LN_NAMESPACE_END


