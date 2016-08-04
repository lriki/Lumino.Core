
#include "../Internal.h"
#include <Lumino/Threading/Semaphore.h>

LN_NAMESPACE_BEGIN

#if defined(LN_OS_WIN32)
//==============================================================================
// Semaphore
//==============================================================================
//------------------------------------------------------------------------------
Semaphore::Semaphore(int initialCount, int maxCount)
{
	if (LN_CHECKEQ_ARG(initialCount < 0)) return;
	if (LN_CHECKEQ_ARG(maxCount < 1)) return;
	if (LN_CHECKEQ_ARG(initialCount > maxCount)) return;
	m_handle = ::CreateSemaphore(NULL, initialCount, maxCount, NULL);
}

//------------------------------------------------------------------------------
Semaphore::~Semaphore()
{
	if (m_handle)
	{
		::CloseHandle(m_handle);
	}
}

//------------------------------------------------------------------------------
void Semaphore::Lock()
{
	::WaitForSingleObject(m_handle, INFINITE);// == WAIT_OBJECT_0);
}

//------------------------------------------------------------------------------
void Semaphore::Unlock()
{
	LONG prev;
	::ReleaseSemaphore(m_handle, 1, &prev);
}

#else

//------------------------------------------------------------------------------
Semaphore::Semaphore(int startCount, int maxCount)
	: m_value(startCount)
	, m_maxCount(maxCount)
{
	pthread_mutex_init(&m_mutex, 0);
	pthread_cond_init(&m_condition, 0);
}

//------------------------------------------------------------------------------
Semaphore::~Semaphore()
{
	pthread_cond_destroy(&m_condition);
	pthread_mutex_destroy(&m_mutex);
}

//------------------------------------------------------------------------------
void Semaphore::Lock()
{
	pthread_mutex_lock(&m_mutex);
	while (m_value < 1)
	{
		pthread_cond_wait(&m_condition, &m_mutex);
	}
	m_value -= 1;
	if (m_value < 0)
	{
		m_value = 0;
	}
	pthread_cond_broadcast(&m_condition);
	pthread_mutex_unlock(&m_mutex);
}

//------------------------------------------------------------------------------
void Semaphore::Unlock()
{
	pthread_mutex_lock(&m_mutex);
	m_value += 1;
	if (m_value > m_maxCount)
	{
		m_value = m_maxCount;
	}
	pthread_cond_broadcast(&m_condition);
	pthread_mutex_unlock(&m_mutex);
}
#endif

LN_NAMESPACE_END
