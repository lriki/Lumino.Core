
#include "../Internal.h"
#include <Lumino/Base/Environment.h>

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
#else
	LN_THROW(0, NotImplementedException);
	return 0;
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
#ifdef LN_WIN32
	return "\r\n";
#else
	return "\n";
#endif
}
template<>
const wchar_t* Environment::GetNewLine<wchar_t>()
{
#ifdef LN_WIN32
	return L"\r\n";
#else
	return L"\n";
#endif
}

} // namespace Lumino
