
#include "../Internal.h"
#include "../../include/Lumino/Threading/ConditionFlag.h"

LN_NAMESPACE_BEGIN
namespace Threading
{

#ifdef LN_THREAD_WIN32
//==============================================================================
// ConditionFlag (Win32)
//==============================================================================
//------------------------------------------------------------------------------
ConditionFlag::ConditionFlag()
	: mHandle( NULL )
{
    mHandle = CreateEvent( NULL, TRUE, FALSE, NULL );
}

//------------------------------------------------------------------------------
ConditionFlag::ConditionFlag( bool initFlag )
	: mHandle( NULL )
{
    BOOL t = ( initFlag ) ? TRUE : FALSE;
    // 手動リセットで作成
    mHandle = CreateEvent( NULL, TRUE, t, NULL );
}

//------------------------------------------------------------------------------
ConditionFlag::~ConditionFlag()
{
    if ( mHandle )
    {
        ::CloseHandle( mHandle );
        mHandle = NULL;
    }
}

//------------------------------------------------------------------------------
void ConditionFlag::SetTrue()
{
    if ( mHandle )
    {
        ::SetEvent( mHandle );
    }
}

//------------------------------------------------------------------------------
void ConditionFlag::SetFalse()
{
    if ( mHandle )
    {
        ::ResetEvent( mHandle );
    }
}

//------------------------------------------------------------------------------
bool ConditionFlag::IsTrue() const
{
    if ( mHandle )
    {
        return ( ::WaitForSingleObject( mHandle, 0 ) == WAIT_OBJECT_0 );
    }
    return true;
}

//------------------------------------------------------------------------------
bool ConditionFlag::IsFalse() const
{
	return !IsTrue();
}

//------------------------------------------------------------------------------
void ConditionFlag::Wait()
{
    if ( mHandle )
    {
		::WaitForSingleObject( mHandle, INFINITE );
    }
}

#else
//==============================================================================
// ConditionFlag (pthread)
//==============================================================================
//------------------------------------------------------------------------------
ConditionFlag::ConditionFlag()
{
	pthread_mutex_init( &mMutex, NULL );
	pthread_cond_init( &mWait, NULL );
	mSignal = false;
}

//------------------------------------------------------------------------------
ConditionFlag::ConditionFlag( bool init_flag )
{
	pthread_mutex_init( &mMutex, NULL );
	pthread_cond_init( &mWait, NULL );
	mSignal = init_flag;
}

//------------------------------------------------------------------------------
ConditionFlag::~ConditionFlag()
{
	pthread_cond_destroy( &mWait );
	pthread_mutex_destroy( &mMutex );
}

//------------------------------------------------------------------------------
void ConditionFlag::SetTrue()
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

//------------------------------------------------------------------------------
void ConditionFlag::SetFalse()
{
	pthread_mutex_lock( &mMutex );
	mSignal = false;
	pthread_mutex_unlock( &mMutex );
}

//------------------------------------------------------------------------------
bool ConditionFlag::IsTrue() const
{
	pthread_mutex_lock( &mMutex );
	bool b = mSignal;
	pthread_mutex_unlock( &mMutex );
	return b;
}

//------------------------------------------------------------------------------
bool ConditionFlag::IsFalse() const
{
	return !IsTrue();
}

//------------------------------------------------------------------------------
void ConditionFlag::Wait()
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
LN_NAMESPACE_END
