#include "../Internal.h"
#ifdef LN_WIN32
#else
	#include <sys/time.h>
#endif
#include <Lumino/Base/ElapsedTimer.h>

namespace Lumino
{

#ifdef LN_WIN32
//=============================================================================
// ElapsedTimer
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ElapsedTimer::ElapsedTimer()
{
	memset(&mFreq, 0x00, sizeof(mFreq));
	memset(&mBefore, 0x00, sizeof(mBefore));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ElapsedTimer::~ElapsedTimer()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ElapsedTimer::Start()
{
	::QueryPerformanceFrequency(&mFreq);
	::QueryPerformanceCounter(&mBefore);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t ElapsedTimer::GetElapsedTime() const
{
	LARGE_INTEGER counter;
	::QueryPerformanceCounter(&counter);
	return (uint64_t)((counter.QuadPart - mBefore.QuadPart) * (1000) / mFreq.QuadPart);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t ElapsedTimer::GetElapsedTimeNS() const
{
	LARGE_INTEGER counter;
	::QueryPerformanceCounter(&counter);
	return (uint64_t)((counter.QuadPart - mBefore.QuadPart) * (1000 * 1000 * 1000) / mFreq.QuadPart);
}

#else
//=============================================================================
// ElapsedTimer
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ElapsedTimer::ElapsedTimer()
	: m_start(0)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ElapsedTimer::~ElapsedTimer()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ElapsedTimer::Start()
{
	timeval t;
	gettimeofday(&t, NULL);	// 第2引数は廃止予定 http://linuxjm.osdn.jp/html/LDP_man-pages/man2/gettimeofday.2.html
	m_start = (uint64_t)t.tv_sec * 1000000 + (uint64_t)t.tv_usec;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t ElapsedTimer::GetElapsedTime() const
{
	timeval t;
	gettimeofday(&t, NULL);
	uint64_t ct = (uint64_t)t.tv_sec * 1000000 + (uint64_t)t.tv_usec;
	return (ct - m_start) / 1000;	// us 単位なので ms に変換する
}

#endif

} // namespace Lumino
