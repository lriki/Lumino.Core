#pragma once

#include "../Base/String.h"
#include "../IO/PathName.h"

LN_NAMESPACE_BEGIN

class TestUtils
{
public:
	/// baseFilePath のファイルと同じディレクトリの fileName を指すパスを得る
	static String GetFilePath(const char* baseFilePath, const char* fileName);
	static String GetFilePath(const char* baseFilePath, const wchar_t* fileName) { return GetFilePathW(baseFilePath, fileName).c_str(); }

	/// baseFilePath のファイルと同じディレクトリの fileName を指すパスを得る
	static PathName GetDirPath(const char* baseFilePath);

	/// baseFilePath のファイルと同じディレクトリの fileName を指すパスを得る
	static PathNameA GetFilePathA(const char* baseFilePath, const char* fileName);
	static PathNameW GetFilePathW(const char* baseFilePath, const wchar_t* fileName);

	/// 2 つの配列をバイト単位で比較する
	static bool CheckArrays(const void* ary1, const void* ary2, size_t count);
};

// __FILE__ のあるフォルダパス (PathName) を取得する
#define LN_TEST_GET_DIR_PATH() TestUtils::GetDirPath(__FILE__)

// __FILE__ と同じフォルダのファイルパス (TCHAR) を取得する
#define LN_TEST_GET_FILE_PATH(fileName) TestUtils::GetFilePath(__FILE__, fileName).c_str()

// __FILE__ と同じフォルダのファイルパス (char) を取得する
#define LN_TEST_GET_FILE_PATH_A(fileName) TestUtils::GetFilePathA(__FILE__, fileName).c_str()

// __FILE__ と同じフォルダのファイルパス (wchar_t) を取得する
#define LN_TEST_GET_FILE_PATH_W(fileName) TestUtils::GetFilePathW(__FILE__, fileName).c_str()

LN_NAMESPACE_END
