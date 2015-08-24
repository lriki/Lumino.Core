#pragma once

#include <gtest/gtest.h>
//#include <mbctype.h>
#include <LuminoCore.h>
#include <Lumino/Testing/TestUtils.h>

using namespace Lumino;

#define LOCALFILE(fileName) TestUtils::GetFilePath(__FILE__, fileName).GetCStr()
#define LOCALFILEA(fileName) TestUtils::GetFilePathA(__FILE__, fileName).GetCStr()
#define LOCALFILEW(fileName) TestUtils::GetFilePathW(__FILE__, fileName).GetCStr()


inline PathName Test_GetTempFilePath(const TCHAR* fileName)
{
	PathName base(__FILE__);
	PathName tempDir(base.GetParent(), _T("../../"));
	tempDir.Append(_T("tmp"));
	PathName path(tempDir, fileName);
	return PathName(path.GetCStr());
}

#define TEMPFILE(fileName)	Test_GetTempFilePath(_T(fileName)).GetCStr()

