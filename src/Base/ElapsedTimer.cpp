#include "../Internal.h"
#if defined(LN_OS_WIN32)
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <Lumino/Base/ElapsedTimer.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// ElapsedTimer
//==============================================================================

//------------------------------------------------------------------------------
ElapsedTimer::ElapsedTimer()
	: m_freq(0)
	, m_start(0)
{
}

//------------------------------------------------------------------------------
ElapsedTimer::~ElapsedTimer()
{
}

#if defined(LN_OS_WIN32)
//------------------------------------------------------------------------------
void ElapsedTimer::Start()
{
	LARGE_INTEGER freq;
	LARGE_INTEGER counter;
	::QueryPerformanceFrequency(&freq);
	::QueryPerformanceCounter(&counter);
	m_freq = freq.QuadPart;
	m_start = counter.QuadPart;
}

//------------------------------------------------------------------------------
uint64_t ElapsedTimer::GetElapsedTime() const
{
	LARGE_INTEGER counter;
	::QueryPerformanceCounter(&counter);
	return (uint64_t)((counter.QuadPart - m_start) * (1000) / m_freq);
}
//------------------------------------------------------------------------------
uint64_t ElapsedTimer::GetElapsedTimeNS() const
{
	LARGE_INTEGER counter;
	::QueryPerformanceCounter(&counter);
	return (uint64_t)((counter.QuadPart - m_start) * (1000 * 1000 * 1000) / m_freq);
}

#else

//------------------------------------------------------------------------------
void ElapsedTimer::Start()
{
	timeval t;
	gettimeofday(&t, NULL);	// 第2引数は廃止予定 http://linuxjm.osdn.jp/html/LDP_man-pages/man2/gettimeofday.2.html
	m_start = (uint64_t)t.tv_sec * 1000000 + (uint64_t)t.tv_usec;
}

//------------------------------------------------------------------------------
uint64_t ElapsedTimer::GetElapsedTime() const
{
	timeval t;
	gettimeofday(&t, NULL);
	uint64_t ct = (uint64_t)t.tv_sec * 1000000 + (uint64_t)t.tv_usec;
	return (ct - m_start) / 1000;	// us 単位なので ms に変換する
}

//------------------------------------------------------------------------------
uint64_t ElapsedTimer::GetElapsedTimeNS() const
{
	timeval t;
	gettimeofday(&t, NULL);
	uint64_t ct = (uint64_t)t.tv_sec * 1000000 + (uint64_t)t.tv_usec;
	return (ct - m_start);
}

#endif

LN_NAMESPACE_END
