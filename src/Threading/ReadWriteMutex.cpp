
#include "../Internal.h"
#include "../../include/Lumino/Threading/ReadWriteMutex.h"

namespace Lumino
{
namespace Threading
{

#ifdef LN_THREAD_WIN32
//=============================================================================
// ReadWriteMutex (Win32)
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ReadWriteMutex::ReadWriteMutex()
	: mNoReaders(NULL)
	, mReaderCount(0)
{
	::InitializeCriticalSection(&mReaderCountLock);
	::InitializeCriticalSection(&mWriterLock);

	mNoReaders = ::CreateEvent(NULL, TRUE, TRUE, NULL);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ReadWriteMutex::~ReadWriteMutex()
{
	if (mNoReaders)
	{
		::CloseHandle(mNoReaders);
		mNoReaders = NULL;
	}
	::DeleteCriticalSection(&mWriterLock);
	::DeleteCriticalSection(&mReaderCountLock);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ReadWriteMutex::LockRead()
{
	::EnterCriticalSection(&mWriterLock);
	::EnterCriticalSection(&mReaderCountLock);
	mReaderCount++;
	if (mReaderCount == 1)
	{
		// ReadLock �X���b�h���� 0 �� 1 �ɂȂ����B
		// �ЂƂȏ㑶�݂��Ă��邱�ƂɂȂ�B
		::ResetEvent(mNoReaders);
	}
	::LeaveCriticalSection(&mReaderCountLock);
	::LeaveCriticalSection(&mWriterLock);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ReadWriteMutex::UnlockRead()
{
	::EnterCriticalSection(&mReaderCountLock);
	if (mReaderCount > 0)
	{
		mReaderCount--;
		if (mReaderCount == 0)
		{
			// ReadLock �X���b�h���� 1 �� 0 �ɂȂ����B
			// ReadLock ���Ă���X���b�h�͑��݂��Ă��Ȃ��B
			// ���� WriteLock �҂��̃X���b�h������΁A���̎��_�œ����o���B
			::SetEvent(mNoReaders);
		}
	}
	::LeaveCriticalSection(&mReaderCountLock);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ReadWriteMutex::LockWrite()
{
	::EnterCriticalSection(&mWriterLock);
	if (mReaderCount > 0)
	{
		// �����ЂƂȏ� ReadLock ����Ă���΁A�����ő҂B
		::WaitForSingleObject(mNoReaders, INFINITE);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ReadWriteMutex::UnlockWrite()
{
	::LeaveCriticalSection(&mWriterLock);
}

#else
//=============================================================================
// ReadWriteMutex (pthread)
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ReadWriteMutex::ReadWriteMutex()
	: mReaders          ( 0 )
	, mWriters          ( 0 )
	, mReadWaiters      ( 0 )
	, mWriteWaiters     ( 0 )
{
	pthread_mutex_init( &mLock, NULL );
	pthread_cond_init( &mRead, NULL );
	pthread_cond_init( &mWrite, NULL );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ReadWriteMutex::~ReadWriteMutex()
{
	pthread_mutex_destroy( &mLock );
	pthread_cond_destroy( &mRead );
	pthread_cond_destroy( &mWrite );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ReadWriteMutex::LockRead()
{
	pthread_mutex_lock( &mLock );
	if ( mWriters || mWriteWaiters )
	{
		++mReadWaiters;
		do {
			pthread_cond_wait( &mRead, &mLock );
		} while ( mWriters || mWriteWaiters );
		--mReadWaiters;
	}
	++mReaders;
	pthread_mutex_unlock( &mLock );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ReadWriteMutex::UnlockRead()
{
	pthread_mutex_lock( &mLock );
	--mReaders;
	if ( mWriteWaiters ) {
		pthread_cond_signal( &mWrite );
	}
	pthread_mutex_unlock( &mLock );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ReadWriteMutex::LockWrite()
{
	pthread_mutex_lock( &mLock );
	if ( mReaders || mWriters )
	{
		++mWriteWaiters;
		do {
			pthread_cond_wait( &mWrite, &mLock );
		} while ( mReaders || mWriters );
		--mWriteWaiters;
	}
	mWriters = 1;
	pthread_mutex_unlock( &mLock );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ReadWriteMutex::UnlockWrite()
{
	pthread_mutex_lock( &mLock );
	mWriters = 0;
	if ( mWriteWaiters )
	{
		pthread_cond_signal( &mWrite );
	}
	else if ( mReadWaiters )
	{
		pthread_cond_broadcast( &mRead );
	}
	pthread_mutex_unlock( &mLock );
}
#endif

} // namespace Threading
} // namespace Lumino
