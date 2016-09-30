#pragma once

#include <gtest/gtest.h>
#include <memory>
//#include <mbctype.h>
#define LN_INTERNAL_ACCESS	public
#include <Lumino/Testing/TestUtils.h>

using namespace ln;

PathName Test_GetTempFilePath(const TCHAR* fileName);

#define LOCALFILE(fileName) TestUtils::GetFilePath(__FILE__, fileName).c_str()
#define LOCALFILEA(fileName) TestUtils::GetFilePathA(__FILE__, fileName).c_str()
#define LOCALFILEW(fileName) TestUtils::GetFilePathW(__FILE__, fileName).c_str()
#define TEMPFILE(fileName)	Test_GetTempFilePath(_T(fileName)).c_str()

