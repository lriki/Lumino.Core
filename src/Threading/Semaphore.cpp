
#include "../Internal.h"
#include <Lumino/Threading/Semaphore.h>

LN_NAMESPACE_BEGIN

#if defined(LN_OS_WIN32)
//=============================================================================
// Semaphore
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Semaphore::Semaphore(int initialCount, int maxCount)
{
	LN_CHECK_ARGS_RETURN(initialCount >= 0);
	LN_CHECK_ARGS_RETURN(maxCount >= 1);
	LN_CHECK_ARGS_RETURN(initialCount <= maxCount);
	m_handle = ::CreateSemaphore(NULL, initialCount, maxCount, NULL);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Semaphore::~Semaphore()
{
	if (m_handle)
	{
		::CloseHandle(m_handle);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Semaphore::Lock()
{
	::WaitForSingleObject(m_handle, INFINITE);// == WAIT_OBJECT_0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Semaphore::Unlock()
{
	LONG prev;
	::ReleaseSemaphore(m_handle, 1, &prev);
}

#else

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Semaphore::Semaphore(int startCount, int maxCount)
	: mValue(startCount)
	, mMaxCount(maxCount)
{
	pthread_mutex_init(&mMutex, 0);
	pthread_cond_init(&mCondition, 0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Semaphore::~Semaphore()
{
	pthread_cond_destroy(&mCondition);
	pthread_mutex_destroy(&mMutex);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Semaphore::decrease()
{
	pthread_mutex_lock(&mMutex);
	while (mValue < 1)
	{
		pthread_cond_wait(&mCondition, &mMutex);
	}
	mValue -= 1;
	if (mValue < 0)
	{
		mValue = 0;
	}
	pthread_cond_broadcast(&mCondition);
	pthread_mutex_unlock(&mMutex);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Semaphore::increase()
{
	pthread_mutex_lock(&mMutex);
	mValue += 1;
	if (mValue > mMaxCount)
	{
		mValue = mMaxCount;
	}
	pthread_cond_broadcast(&mCondition);
	pthread_mutex_unlock(&mMutex);
}
#endif

LN_NAMESPACE_END
