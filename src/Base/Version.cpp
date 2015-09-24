
#include <Lumino/Base/Version.h>

namespace Lumino
{
namespace Version
{

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Core::GetMajor()
{
	return LUMINO_CORE_VERSION_MAJOR;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Core::GetMinor()
{
	return LUMINO_CORE_VERSION_MINOR;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Core::GetPatch()
{
	return LUMINO_CORE_VERSION_PATCH;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const char* Core::GetVersionString()
{
	return LUMINO_CORE_VERSION_STRING;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Core::IsAtLeast(int major, int minor, int patch)
{
	if (LUMINO_CORE_VERSION_MAJOR < major) return false;
	if (LUMINO_CORE_VERSION_MAJOR > major) return true;
	if (LUMINO_CORE_VERSION_MINOR < minor) return false;
	if (LUMINO_CORE_VERSION_MINOR > minor) return true;
	if (LUMINO_CORE_VERSION_PATCH < patch) return false;
	return true;
}

} // namespace Version
} // namespace Lumino
