
#ifndef LUMINO_MATH_VERSION_H
#define LUMINO_MATH_VERSION_H

#include "Common.h"

#define LUMINO_MATH_VERSION_MAJOR	1			///< ���W���[�o�[�W����
#define LUMINO_MATH_VERSION_MINOR	0			///< �}�C�i�[�o�[�W����
#define LUMINO_MATH_VERSION_PATCH	0			///< �p�b�`�ԍ�
#define LUMINO_MATH_VERSION_STRING	"1.0.0"		///< �o�[�W����������

LN_NAMESPACE_BEGIN
namespace Version
{

/**
	@brief		���w���C�u�����̃o�[�W�������ł��B
	@details	���̃N���X����擾�ł���l�̓��C�u�����̃o�C�i���t�@�C���̃o�[�W�����ԍ��ł��B
				�w�b�_�t�@�C���̃o�[�W������v���v���Z�X�Ŏg�p�������ꍇ�� 
				LUMINO_MATH_VERSION_MAJOR �� LUMINO_MATH_VERSION_MINOR ���g�p���Ă��������B
*/
class LUMINO_EXPORT Math
{
public:

	/**
		@brief	���W���[�o�[�W�������擾���܂��B
	*/
	static int GetMajor();

	/**
		@brief	�}�C�i�[�o�[�W�������擾���܂��B
	*/
	static int GetMinor();

	/**
		@brief	�p�b�`�ԍ����擾���܂��B
	*/
	static int GetPatch();

	/**
		@brief	�o�[�W����������̎擾���擾���܂��B
	*/
	static const char* GetVersionString();

	/**
		@brief	�w�肵���o�[�W�����ԍ��ƁA���C�u�����t�@�C���̃R���p�C���o�[�W�����ԍ����r���܂��B
		@return	�w��o�[�W���� >= �R���p�C���o�[�W���� �ł���ꍇ�Atrue ��Ԃ��܂��B
	*/
	static bool IsAtLeast(int major = LUMINO_MATH_VERSION_MAJOR, int minor = LUMINO_MATH_VERSION_MINOR, int patch = LUMINO_MATH_VERSION_PATCH);
};

} // namespace Version
LN_NAMESPACE_END

#endif // LUMINO_MATH_VERSION_H
