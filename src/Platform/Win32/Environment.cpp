
#include "../../Internal.h"
#include "../../../include/Lumino/Platform/Environment.h"

namespace Lumino
{

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t Environment::GetTickCount()
{
	// timeGetTime() �� timeBeginPeriod() �ɂ���Đ��x���ς�邽�߁A
	// GetTickCount() �̕������������Ȃ�
	return ::GetTickCount();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t Environment::GetTickCountNS()
{
	static LARGE_INTEGER freq = {};
	static bool initGetTickCount = false;

	// ����Ăяo�����ɏ�����
	if (!initGetTickCount)
	{
		::QueryPerformanceFrequency(&freq);
		initGetTickCount = true;
	}

	LARGE_INTEGER current;
	::QueryPerformanceCounter( &current );

	if (freq.QuadPart) {
		return static_cast<long long>(((double)current.QuadPart) * 1000 * 1000 * 1000 / freq.QuadPart);		// ns �P��
	}
	return 0;
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
#endif
}
template<>
const wchar_t* Environment::GetNewLine<wchar_t>()
{
#ifdef LN_WIN32
	return L"\r\n";
#else
#endif
}

} // namespace Lumino
