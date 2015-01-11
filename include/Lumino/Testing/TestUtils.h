#pragma once

#include "../Base/String.h"
#include "../IO/PathName.h"

namespace Lumino
{

class TestUtils
{
public:
	/// baseFilePath のファイルと同じディレクトリの fileName を指すパスを得る
	static String GetFilePath(const char* baseFilePath, const char* fileName);

	/// baseFilePath のファイルと同じディレクトリの fileName を指すパスを得る
	static PathName GetDirPath(const char* baseFilePath);
};

// __FILE__ のあるフォルダパス (PathName) を取得する
#define LN_TEST_GET_DIR_PATH() TestUtils::GetDirPath(__FILE__)

// __FILE__ と同じフォルダのファイルパス (PathName) を取得する
#define LN_TEST_GET_FILE_PATH(fileName) TestUtils::GetFilePath(__FILE__, fileName).c_str()

} // namespace Lumino
