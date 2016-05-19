﻿
#include "../Internal.h"
#include "../../include/Lumino/Threading/Mutex.h"

LN_NAMESPACE_BEGIN

#ifdef LN_THREAD_WIN32
//==============================================================================
// Mutex (Win32)
//==============================================================================
//------------------------------------------------------------------------------
Mutex::Mutex()
{
    ::InitializeCriticalSection( &m_CriticalSection );
}

//------------------------------------------------------------------------------
Mutex::~Mutex()
{
	::DeleteCriticalSection( &m_CriticalSection );
}

//------------------------------------------------------------------------------
void Mutex::Lock()
{
	::EnterCriticalSection( &m_CriticalSection );
}

//------------------------------------------------------------------------------
void Mutex::Unlock()
{
	::LeaveCriticalSection( &m_CriticalSection );
}

#else
//==============================================================================
// Mutex (pthread)
//==============================================================================
//------------------------------------------------------------------------------
Mutex::Mutex()
{
	pthread_mutex_init( &mMutex, NULL );
}

//------------------------------------------------------------------------------
Mutex::~Mutex()
{
	pthread_mutex_destroy( &mMutex );
}

//------------------------------------------------------------------------------
void Mutex::Lock()
{
	pthread_mutex_lock( &mMutex );
}

//------------------------------------------------------------------------------
void Mutex::Unlock()
{
	pthread_mutex_unlock( &mMutex );
}
#endif

LN_NAMESPACE_END
