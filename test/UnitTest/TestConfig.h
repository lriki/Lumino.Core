#pragma once

#include <gtest/gtest.h>
//#include <mbctype.h>
#include <LuminoCore.h>
#include <Lumino/Testing/TestUtils.h>

using namespace ln;

#define LOCALFILE(fileName) TestUtils::GetFilePath(__FILE__, fileName).c_str()
#define LOCALFILEA(fileName) TestUtils::GetFilePathA(__FILE__, fileName).c_str()
#define LOCALFILEW(fileName) TestUtils::GetFilePathW(__FILE__, fileName).c_str()


inline PathName Test_GetTempFilePath(const TCHAR* fileName)
{
	PathName base(__FILE__);
	PathName tempDir(base.GetParent(), _T("../../"));
	tempDir.Append(_T("tmp"));
	PathName path(tempDir, fileName);
	FileSystem::CreateDirectory(path.GetParent());
	return PathName(path.c_str());
}

#define TEMPFILE(fileName)	Test_GetTempFilePath(_T(fileName)).c_str()

