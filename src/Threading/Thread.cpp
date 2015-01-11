
/*
	以前は Thread_Win32.cpp と Thread_Unix.cpp に分けていたが、
	Cygwin では _beginthreadex
*/

#include <sstream>
#include <iostream>
#include <process.h>
#include "../Internal.h"
#include <Threading/Thread.h>
#include <Threading/ThreadingExceptions.h>

namespace Lumino
{
namespace Threading
{

#ifdef LN_THREAD_WIN32
//=============================================================================
// Thread (Win32)
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class Thread::internal_thr_execute
{
public:

	static unsigned int __stdcall _execute_static( void* ptr )
	{
		Thread* obj = static_cast<Thread*>( ptr );

		// 実行
		try
		{
			obj->Execute();
		}
	    catch ( Exception& e )
		{
			obj->mLastException = e.Copy();
		}
		catch (...)
		{
			obj->mLastException = LN_NEW ThreadException();
		}

		// 終了フラグを立てる
		obj->mFinished.SetTrue();
		return 0;
	}

	static DWORD __stdcall _execute_static_winapi( void* ptr )
	{
		return _execute_static( ptr );
	}
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Thread::Thread()
	: mFinished			( true )
	, mLastException		( NULL )
    , mThread				( NULL )
    , mThreadID			( 0 )
	, mUseCreateThreadAPI	( false )
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Thread::~Thread()
{
    if ( mThread )
    {
		// TODO:
        //printf( "< Thread::~Thread >\n" );
        //printf( "スレッドが終了していません。wait() で待機します。\n" );
        //printf( "もし応答が無くなった場合はスレッドが無限ループになっていないか確認してください。" );
        Wait();
    }

	LN_SAFE_DELETE( mLastException );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Thread::Reset()
{
	// 前のが終了していない場合は待つ
	Wait();

	LN_SAFE_DELETE( mLastException );
	mFinished.SetFalse();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Thread::Start()
{
	Reset();

	if ( mUseCreateThreadAPI )
	{
		//mThread = (HANDLE)(::AfxBeginThread(_thr_prx::_execute_static, this));

		DWORD threadID;
		mThread = CreateThread(
			NULL, //セキュリティ属性
			0, //スタックサイズ
			internal_thr_execute::_execute_static_winapi, //スレッド関数
			this, //スレッド関数に渡す引数
			0, //作成オプション(0またはCREATE_SUSPENDED)
			&threadID);//スレッドID
		mThreadID = threadID;

		// TODO :起動できなかった例外
		// TODO :既に起動している例外
	}
	else
	{
		mThread = reinterpret_cast< HANDLE >( _beginthreadex( NULL, 0, internal_thr_execute::_execute_static, this, 0, &mThreadID ) );
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Thread::Wait()
{
	if ( mThread )
	{
		//mFinished.Wait();
		::WaitForSingleObject(mThread, INFINITE);

		if (!mUseCreateThreadAPI)
		{
			::CloseHandle(mThread);
		}
		mThread = NULL;

		// スレッドで例外が発生していれば throw する
		if ( mLastException != NULL ) {
			throw *mLastException;
		}
	}

	mThreadID = 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Thread::IsFinished()
{
	return mFinished.IsTrue();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
intptr_t Thread::GetThreadID() const
{ 
    return mThreadID;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
intptr_t Thread::GetCurrentThreadID()
{
    return ::GetCurrentThreadId();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Thread::Sleep(int msTime)
{
    ::Sleep(msTime);
}
#else
//=============================================================================
// Thread (pthread)
//=============================================================================

class Thread::internal_thr_execute
{
public:

	static void* _execute_static( void* ptr )
	{
		Thread* obj = static_cast<Thread*>( ptr );
		
		// 実行
		try
		{
			obj->Execute();
		}
	    catch ( Exception& e )
		{
			obj->mLastException = e.Copy();
		}
		catch (...)
		{
			obj->mLastException = LN_NEW ThreadException();
		}

		// 終了フラグを立てる
		obj->mFinished.SetTrue();
		return 0;
	}
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Thread::Thread()
	: mFinished(false)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Thread::~Thread()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Thread::Start()
{
	// 前のが終了していない場合は待つ
	Wait();

	// 初期化
	LN_SAFE_DELETE( mLastException );
	mFinished.SetFalse();

	// 開始
	pthread_create( &mThread,  NULL, internal_thr_execute::_execute_static, this );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Thread::Wait()
{
	if ( mThread )
	{
		// 待機
		pthread_join(mThread, NULL);
		mThread = NULL;

		// スレッドで例外が発生していれば throw する
		if ( mLastException != NULL ) {
			throw *mLastException;
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Thread::IsFinished()
{
	return mFinished.IsTrue();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
intptr_t Thread::GetThreadID() const
{ 
	return reinterpret_cast<intptr_t>(mThread);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
intptr_t Thread::GetCurrentThreadID()
{
    return reinterpret_cast<intptr_t>(pthread_self());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Thread::Sleep(int msTime)
{
     ::usleep( msTime * 1000 );
}

#endif

//=============================================================================
// DelegateThread
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DelegateThread::Start(Delegate00& func)
{
	m_ThreadFunc = func;
	Thread::Start();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DelegateThread::Execute()
{
	if (!m_ThreadFunc.IsEmpty()) {
		m_ThreadFunc.Call();
	}
}

} // namespace Threading
} // namespace Lumino
