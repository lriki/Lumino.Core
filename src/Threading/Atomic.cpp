﻿
#include "../Internal.h"
#include "../../include/Lumino/Threading/Atomic.h"

LN_NAMESPACE_BEGIN

#ifdef LN_THREAD_WIN32
//==============================================================================
// Atomic (Win32)
//==============================================================================
//------------------------------------------------------------------------------
Atomic::Atomic(int32_t initialValue)
	: mValue(initialValue)
{
}

//------------------------------------------------------------------------------
Atomic::~Atomic()
{
}

//------------------------------------------------------------------------------
int32_t Atomic::Get() const
{
	return ::InterlockedExchangeAdd( const_cast< volatile LONG* >( &mValue ), 0 );
}

//------------------------------------------------------------------------------
void Atomic::Set( int32_t nValue )
{
    ::InterlockedExchange( &mValue, nValue );
}

//------------------------------------------------------------------------------
int32_t Atomic::Increment()
{
    return ::InterlockedIncrement( &mValue );
}

//------------------------------------------------------------------------------
int32_t Atomic::Decrement()
{
    return ::InterlockedDecrement( &mValue );
}

#else
//==============================================================================
// Atomic (pthread)
//==============================================================================
// __sync_fetch_and_add でもできるけど、環境依存ある。
// http://fallabs.com/blog-ja/promenade.cgi?id=68
// とりあえず今は確実なミューテックスで。
//------------------------------------------------------------------------------
Atomic::Atomic( int32_t value )
    : mValue( value )
{
    pthread_mutex_init( &mMutex, NULL );
}

//------------------------------------------------------------------------------
Atomic::~Atomic()
{
    pthread_mutex_destroy( &mMutex );
}

//------------------------------------------------------------------------------
int32_t Atomic::Get() const
{
	pthread_mutex_lock( &mMutex );
    int32_t v = mValue;
    pthread_mutex_unlock( &mMutex );
    return v;
}

//------------------------------------------------------------------------------
void Atomic::Set( int32_t value_ )
{
    pthread_mutex_lock( &mMutex );
    mValue = value_;
    pthread_mutex_unlock( &mMutex );
}

//------------------------------------------------------------------------------
int32_t Atomic::Increment()
{
    pthread_mutex_lock( &mMutex );
    ++mValue;
    int32_t v = mValue;
    pthread_mutex_unlock( &mMutex );
    return v;
}

//------------------------------------------------------------------------------
int32_t Atomic::Decrement()
{
    pthread_mutex_lock( &mMutex );
    --mValue;
    int32_t v = mValue;
    pthread_mutex_unlock( &mMutex );
    return v;
}

#endif

LN_NAMESPACE_END
