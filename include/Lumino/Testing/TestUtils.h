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

	/// baseFilePath �̃t�@�C���Ɠ����f�B���N�g���� fileName ���w���p�X�𓾂�
	static PathNameA GetFilePathA(const char* baseFilePath, const char* fileName);
	static PathNameW GetFilePathW(const char* baseFilePath, const wchar_t* fileName);

	/// 2 �̔z����o�C�g�P�ʂŔ�r����
	static bool CheckArrays(const void* ary1, const void* ary2, size_t count);
};

// __FILE__ �̂���t�H���_�p�X (PathName) ���擾����
#define LN_TEST_GET_DIR_PATH() TestUtils::GetDirPath(__FILE__)

// __FILE__ �Ɠ����t�H���_�̃t�@�C���p�X (TCHAR) ���擾����
#define LN_TEST_GET_FILE_PATH(fileName) TestUtils::GetFilePath(__FILE__, fileName).c_str()

// __FILE__ �Ɠ����t�H���_�̃t�@�C���p�X (char) ���擾����
#define LN_TEST_GET_FILE_PATH_A(fileName) TestUtils::GetFilePathA(__FILE__, fileName).c_str()

// __FILE__ �Ɠ����t�H���_�̃t�@�C���p�X (wchar_t) ���擾����
#define LN_TEST_GET_FILE_PATH_W(fileName) TestUtils::GetFilePathW(__FILE__, fileName).c_str()

} // namespace Lumino
