
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

} // namespace Lumino
