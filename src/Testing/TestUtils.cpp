
#include "../../include/Lumino/Testing/TestUtils.h"

namespace Lumino
{

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
String TestUtils::GetFilePath(const char* baseSourceFilePath, const char* fileName)
{
	PathName base(baseSourceFilePath);
	PathName path(base.GetParent(), fileName);
	return String(path.GetCStr());
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
PathName TestUtils::GetDirPath(const char* baseFilePath)
{
	PathName base(baseFilePath);
	return base.GetParent();
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
PathNameA TestUtils::GetFilePathA(const char* baseFilePath, const char* fileName)
{
	PathNameA base(baseFilePath);
	PathNameA path(base.GetParent(), fileName);
	return path;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
PathNameW TestUtils::GetFilePathW(const char* baseFilePath, const wchar_t* fileName)
{
	PathNameW base(baseFilePath);
	PathNameW path(base.GetParent(), fileName);
	return path;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
bool TestUtils::CheckArrays(const void* ary1, const void* ary2, size_t count)
{
	byte_t* b1 = (byte_t*)ary1;
	byte_t* b2 = (byte_t*)ary2;
	for (size_t i = 0; i < count; ++i)
	{
		if (b1[i] != b2[i]) {
			printf("not match arrays [%d]\n", i);
			return false;
		}
	}
	return true;
}

} // namespace Lumino
