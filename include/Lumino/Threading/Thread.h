
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
	@brief		スレッドクラスの基底クラス
	@details	使い方
				①このクラスを継承した新しいクラスを作成し、Execute() を実装します。
				②Start() でスレッドを開始します。
				③Wait() で終了を待ちます。
				  終了した際に Execute() で発生した例外がある場合、Wait() から際 throw されます。
				④インスタンスを破棄します。
*/
class LUMINO_EXPORT Thread
    : private NonCopyable
{
public:
    Thread();
    virtual ~Thread();

public:

	/**
		@brief	スレッド関数 (これを自身で実装する)
	*/
	virtual void Execute() = 0;

public:

	/**
		@brief	実行開始
	*/
	void Start();

	/**
		@brief		スレッドの終了を待つ
		@details	スレッドがハンドルされない例外で終了した場合、
					この関数はその例外を再throwします。
	*/
	void Wait();

	/**
		@brief	終了しているかどうかの判定
	*/
	bool IsFinished();

	/**
		@brief	スレッド識別子の取得 (start() 後に呼ぶこと)
	*/
	intptr_t GetThreadID() const;

public:

	/**
		@brief	現在実行中のスレッドのIDを取得
	*/
	static intptr_t GetCurrentThreadID();

	/**
		@brief	指定した時間だけ現在のスレッドをスリープ
		@param	msTime	: 待機時間 (ミリ秒)
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
	@brief	Delegate をスレッド関数として実行するスレッド
	@code
			class Test
			{
			private:
				DelegateThread	m_Thread;
				void ThreadFunc()
				{
					// 別のスレッドで実行されるメンバ関数
				}
			public:
				void Run()
				{
					// スレッド起動
					m_Thread.Start(LN_CreateDelegate(this, &Test::ThreadFunc));
				}
			};
	@endcode
*/
class LUMINO_EXPORT DelegateThread
    : public Thread
{
#ifdef LN_CPP11
public:
	void Start(Delegate<void()> func);

protected:
	virtual void Execute();

private:
	Delegate<void()>	m_ThreadFunc;
#else
public:
	void Start(Delegate00 func);

protected:
	virtual void Execute();
	
private:
	Delegate00	m_ThreadFunc;
#endif
};

} // namespace Threading
} // namespace Lumino
