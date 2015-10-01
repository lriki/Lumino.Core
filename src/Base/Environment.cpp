
#include <time.h>
#include "../Internal.h"
#ifdef LN_OS_WIN32
    #include <Shlobj.h>
#elif defined(LN_OS_MAC)
    #include <mach/mach_time.h>
#endif
#include <Lumino/Base/Environment.h>

// linux
#include <Lumino/Base/StringTraits.h>
#include <Lumino/IO/FileSystem.h>

namespace Lumino
{

//=============================================================================
// Environment
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ByteOrder Environment::GetByteOrder()
{
	if (IsLittleEndian()) {
		return ByteOrder_Little;
	}
	return ByteOrder_Big;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Environment::IsLittleEndian()
{
	const unsigned short x = 1;   // 0x0001
	return (*(unsigned char *)&x) != 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t Environment::GetTickCount()
{
#ifdef _WIN32
	// timeGetTime() は timeBeginPeriod() によって精度が変わるため、
	// GetTickCount() の方が無難かもしれない
	return ::GetTickCount();
#elif defined(LN_OS_MAC)
    return GetTickCountNS() / 1000000;
    
#else
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		return 0;
	}
	return ts.tv_nsec / 1000000;
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t Environment::GetTickCountNS()
{
#ifdef _WIN32
	static LARGE_INTEGER freq = {};
	static bool initGetTickCount = false;

	// 初回呼び出し時に初期化
	if (!initGetTickCount)
	{
		::QueryPerformanceFrequency(&freq);
		initGetTickCount = true;
	}

	LARGE_INTEGER current;
	::QueryPerformanceCounter(&current);

	if (freq.QuadPart) {
		return static_cast<long long>(((double)current.QuadPart) * 1000 * 1000 * 1000 / freq.QuadPart);		// ns 単位
	}
    return 0;
    
#elif defined(LN_OS_MAC)
    static mach_timebase_info_data_t info = { 0, 0 };
    
    uint64_t cpuTime = mach_absolute_time();
    if (info.denom == 0) {
        mach_timebase_info(&info);
    }
    return cpuTime * info.numer / info.denom;
    
#else
	LN_THROW(0, NotImplementedException);
	return 0;
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<>
const char* Environment::GetNewLine<char>()
{
#ifdef LN_OS_WIN32
	return "\r\n";
#else
	return "\n";
#endif
}
template<>
const wchar_t* Environment::GetNewLine<wchar_t>()
{
#ifdef LN_OS_WIN32
	return L"\r\n";
#else
	return L"\n";
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#ifdef LN_OS_WIN32
template<>
void Environment::GetSpecialFolderPath(SpecialFolder specialFolder, char* outPath)
{
	if (outPath == NULL) { return; }
	switch (specialFolder)
	{
	case SpecialFolder::ApplicationData:
		::SHGetSpecialFolderPathA(NULL, outPath, CSIDL_APPDATA, FALSE);
		break;
	case SpecialFolder::Temporary:
		::GetTempPathA(LN_MAX_PATH, outPath);
		break;
	}
}
template<>
void Environment::GetSpecialFolderPath(SpecialFolder specialFolder, wchar_t* outPath)
{
	if (outPath == NULL) { return; }
	switch (specialFolder)
	{
	case SpecialFolder::ApplicationData:
		::SHGetSpecialFolderPathW(NULL, outPath, CSIDL_APPDATA, FALSE);
		break;
	case SpecialFolder::Temporary:
		::GetTempPathW(LN_MAX_PATH, outPath);
		break;
	}
}
#elif defined(LN_OS_LINUX)
template<typename TChar>
void Environment::GetSpecialFolderPath(SpecialFolder specialFolder, TChar* outPath)
{
	if (outPath == NULL) { return; }

	switch (specialFolder)
	{
	case SpecialFolder::ApplicationData:
	{
		const TChar* pathes[] =
		{
			LN_T(TChar, "~/.local/share"),
			LN_T(TChar, "/usr/local/share"),
			LN_T(TChar, "/usr/share"),
		};
		for (int i = 0; i < LN_ARRAY_SIZE_OF(pathes); ++i) {
			if (FileSystem::ExistsDirectory(pathes[i])) {
				StringTraits::tstrcpy(outPath, LN_MAX_PATH, pathes[i]);
				return;
			}
		}
		break;
	}
	case SpecialFolder::Temporary:
		const TChar* pathes[] =
		{
			LN_T(TChar, "/tmp"),
		};
		for (int i = 0; i < LN_ARRAY_SIZE_OF(pathes); ++i) {
			if (FileSystem::ExistsDirectory(pathes[i])) {
				StringTraits::tstrcpy(outPath, LN_MAX_PATH, pathes[i]);
				return;
			}
		}
		break;
	}
	LN_THROW(0, InvalidOperationException);
}
template void Environment::GetSpecialFolderPath(SpecialFolder specialFolder, char* outPath);
template void Environment::GetSpecialFolderPath(SpecialFolder specialFolder, wchar_t* outPath);

#else
#endif


} // namespace Lumino
