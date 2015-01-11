
#include "../Internal.h"
#include <Threading/EventFlag.h>

namespace Lumino
{
namespace Threading
{

#ifdef LN_THREAD_WIN32
//=============================================================================
// EventFlag (Win32)
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EventFlag::EventFlag()
	: mHandle( NULL )
{
    mHandle = CreateEvent( NULL, TRUE, FALSE, NULL );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EventFlag::EventFlag( bool initFlag )
	: mHandle( NULL )
{
    BOOL t = ( initFlag ) ? TRUE : FALSE;
    // 手動リセットで作成
    mHandle = CreateEvent( NULL, TRUE, t, NULL );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EventFlag::~EventFlag()
{
    if ( mHandle )
    {
        ::CloseHandle( mHandle );
        mHandle = NULL;
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void EventFlag::SetTrue()
{
    if ( mHandle )
    {
        ::SetEvent( mHandle );
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void EventFlag::SetFalse()
{
    if ( mHandle )
    {
        ::ResetEvent( mHandle );
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool EventFlag::IsTrue() const
{
    if ( mHandle )
    {
        return ( ::WaitForSingleObject( mHandle, 0 ) == WAIT_OBJECT_0 );
    }
    return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void EventFlag::Wait()
{
    if ( mHandle )
    {
		::WaitForSingleObject( mHandle, INFINITE );
    }
}

#else
//=============================================================================
// EventFlag (pthread)
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EventFlag::EventFlag()
{
	pthread_mutex_init( &mMutex, NULL );
	pthread_cond_init( &mWait, NULL );
	mSignal = false;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EventFlag::EventFlag( bool init_flag )
{
	pthread_mutex_init( &mMutex, NULL );
	pthread_cond_init( &mWait, NULL );
	mSignal = init_flag;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EventFlag::~EventFlag()
{
	pthread_cond_destroy( &mWait );
	pthread_mutex_destroy( &mMutex );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void EventFlag::SetTrue()
{
	// ミューテックスをロックする
	int r = pthread_mutex_lock( &mMutex );
	if ( r != 0 )
	{ 
		errno = r; 
		return;
	}

	mSignal = true;

	// 中断しているスレッドに通知を送る
	r = pthread_cond_broadcast( &mWait );//pthread_cond_signal( &mWait );
	if ( r != 0 )
	{ 
		errno = r; 
		return;
	}

	// ミューテックスをアンロックする
	r = pthread_mutex_unlock( &mMutex );
	if ( r != 0 )
	{ 
		errno = r; 
		return;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void EventFlag::SetFalse()
{
	pthread_mutex_lock( &mMutex );
	mSignal = false;
	pthread_mutex_unlock( &mMutex );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool EventFlag::IsTrue() const
{
	pthread_mutex_lock( &mMutex );
	bool b = mSignal;
	pthread_mutex_unlock( &mMutex );
	return b;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void EventFlag::Wait()
{
	// ミューテックスをロックする
	int r = pthread_mutex_lock( &mMutex );
	if ( r != 0 ) { 
		errno = r; 
		return;
	}

	// true ならアンロックして中断してみる
	if ( mSignal )
	{
		pthread_mutex_unlock( &mMutex );
		return;
	}

	// スレッドをスリープさせる
	r = pthread_cond_wait( &mWait, &mMutex );
	if ( r != 0 ) { 
		errno = r; 
		return;
	}

	// アンロック
	pthread_mutex_unlock( &mMutex );
	if ( r != 0 ) { 
		errno = r; 
		return;
	}
}

#endif

} // namespace Threading
} // namespace Lumino
