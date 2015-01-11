
/*
	�ȑO�� Thread_Win32.cpp �� Thread_Unix.cpp �ɕ����Ă������A
	Cygwin �ł� _beginthreadex
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

		// ���s
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

		// �I���t���O�𗧂Ă�
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
        //printf( "�X���b�h���I�����Ă��܂���Bwait() �őҋ@���܂��B\n" );
        //printf( "���������������Ȃ����ꍇ�̓X���b�h���������[�v�ɂȂ��Ă��Ȃ����m�F���Ă��������B" );
        Wait();
    }

	LN_SAFE_DELETE( mLastException );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Thread::Reset()
{
	// �O�̂��I�����Ă��Ȃ��ꍇ�͑҂�
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
			NULL, //�Z�L�����e�B����
			0, //�X�^�b�N�T�C�Y
			internal_thr_execute::_execute_static_winapi, //�X���b�h�֐�
			this, //�X���b�h�֐��ɓn������
			0, //�쐬�I�v�V����(0�܂���CREATE_SUSPENDED)
			&threadID);//�X���b�hID
		mThreadID = threadID;

		// TODO :�N���ł��Ȃ�������O
		// TODO :���ɋN�����Ă����O
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

		// �X���b�h�ŗ�O���������Ă���� throw ����
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
		
		// ���s
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

		// �I���t���O�𗧂Ă�
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
	// �O�̂��I�����Ă��Ȃ��ꍇ�͑҂�
	Wait();

	// ������
	LN_SAFE_DELETE( mLastException );
	mFinished.SetFalse();

	// �J�n
	pthread_create( &mThread,  NULL, internal_thr_execute::_execute_static, this );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Thread::Wait()
{
	if ( mThread )
	{
		// �ҋ@
		pthread_join(mThread, NULL);
		mThread = NULL;

		// �X���b�h�ŗ�O���������Ă���� throw ����
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
