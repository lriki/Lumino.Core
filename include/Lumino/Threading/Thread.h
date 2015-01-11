
#pragma once

#include "../Base/Common.h"
#include "../Base/NonCopyable.h"
#include "../Base/Delegate.h"
#include "EventFlag.h"

namespace Lumino
{
class Exception;

namespace Threading
{
/**
	@brief		�X���b�h�N���X�̊��N���X
	@details	�g����
				�@���̃N���X���p�������V�����N���X���쐬���AExecute() ���������܂��B
				�AStart() �ŃX���b�h���J�n���܂��B
				�BWait() �ŏI����҂��܂��B
				  �I�������ۂ� Execute() �Ŕ���������O������ꍇ�AWait() ����� throw ����܂��B
				�C�C���X�^���X��j�����܂��B
*/
class LUMINO_EXPORT Thread
    : private NonCopyable
{
public:
    Thread();
    virtual ~Thread();

public:

	/**
		@brief	�X���b�h�֐� (��������g�Ŏ�������)
	*/
	virtual void Execute() = 0;

public:

	/**
		@brief	���s�J�n
	*/
	void Start();

	/**
		@brief		�X���b�h�̏I����҂�
		@details	�X���b�h���n���h������Ȃ���O�ŏI�������ꍇ�A
					���̊֐��͂��̗�O����throw���܂��B
	*/
	void Wait();

	/**
		@brief	�I�����Ă��邩�ǂ����̔���
	*/
	bool IsFinished();

	/**
		@brief	�X���b�h���ʎq�̎擾 (start() ��ɌĂԂ���)
	*/
	intptr_t GetThreadID() const;

public:

	/**
		@brief	���ݎ��s���̃X���b�h��ID���擾
	*/
	static intptr_t GetCurrentThreadID();

	/**
		@brief	�w�肵�����Ԃ������݂̃X���b�h���X���[�v
		@param	msTime	: �ҋ@���� (�~���b)
	*/
	static void Sleep(int msTime);

private:
	void Reset();

private:
	EventFlag	mFinished;
	Exception*	mLastException;
    
#ifdef LN_THREAD_WIN32
    HANDLE			mThread;
	unsigned int	mThreadID;
	bool			mUseCreateThreadAPI;
#else
	pthread_t		mThread;
#endif

	class internal_thr_execute;
	friend class internal_thr_execute;
};


/**
	@brief	Delegate ���X���b�h�֐��Ƃ��Ď��s����X���b�h
	@code
			class Test
			{
			private:
				DelegateThread	m_Thread;
				void ThreadFunc()
				{
					// �ʂ̃X���b�h�Ŏ��s����郁���o�֐�
				}
			public:
				void Run()
				{
					// �X���b�h�N��
					m_Thread.Start(LN_CreateDelegate(this, &Test::ThreadFunc));
				}
			};
	@endcode
*/
class LUMINO_EXPORT DelegateThread
    : public Thread
{
public:
	void Start(Delegate00& func);

protected:
	virtual void Execute();
	
private:
	Delegate00	m_ThreadFunc;
};

} // namespace Threading
} // namespace Lumino
