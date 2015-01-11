#pragma once

#include "../Base/Array.h"

namespace Lumino
{

/**
	@brief	
*/
class DirectoryUtils
{
public:

	/**
		@brief		�f�B���N�g�����Ɋ܂܂��S�Ẵt�@�C���̃p�X���擾����
		@param[in]	dirPath	: ��������f�B���N�g��
		@param[in]	pattern	: ���������� (���C���h�J�[�h�w��\)
	*/
	static Array<String> GetFiles(const TCHAR* dirPath, const TCHAR* pattern);
};

} // namespace Lumino
