
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

} // namespace Lumino
