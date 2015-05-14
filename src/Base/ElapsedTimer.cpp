
#include "../Internal.h"
#include <Lumino/Base/ElapsedTimer.h>

namespace Lumino
{

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

} // namespace Lumino
