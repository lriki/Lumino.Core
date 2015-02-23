#pragma once

#include <gtest/gtest.h>
//#include <mbctype.h>
#include <LuminoCore.h>
#include <Lumino/Testing/TestUtils.h>

using namespace Lumino;

#define LOCALFILE(fileName) TestUtils::GetFilePath(__FILE__, fileName).GetCStr()
#define LOCALFILEA(fileName) TestUtils::GetFilePathA(__FILE__, fileName).GetCStr()
#define LOCALFILEW(fileName) TestUtils::GetFilePathW(__FILE__, fileName).GetCStr()
