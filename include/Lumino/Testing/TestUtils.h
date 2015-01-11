#pragma once

#include "../Base/String.h"
#include "../IO/PathName.h"

namespace Lumino
{

class TestUtils
{
public:
	/// baseFilePath �̃t�@�C���Ɠ����f�B���N�g���� fileName ���w���p�X�𓾂�
	static String GetFilePath(const char* baseFilePath, const char* fileName);

	/// baseFilePath �̃t�@�C���Ɠ����f�B���N�g���� fileName ���w���p�X�𓾂�
	static PathName GetDirPath(const char* baseFilePath);
};

// __FILE__ �̂���t�H���_�p�X (PathName) ���擾����
#define LN_TEST_GET_DIR_PATH() TestUtils::GetDirPath(__FILE__)

// __FILE__ �Ɠ����t�H���_�̃t�@�C���p�X (PathName) ���擾����
#define LN_TEST_GET_FILE_PATH(fileName) TestUtils::GetFilePath(__FILE__, fileName).c_str()

} // namespace Lumino
