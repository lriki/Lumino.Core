
#include "../Internal.h"
#include <windows.h>

LN_NAMESPACE_BEGIN
namespace detail {

class ThreadImpl
{
public:
	ThreadImpl()
		: mThread(0)
	{
	}

	~ThreadImpl()
	{
	}

	void Start()
	{
		pthread_create(&mThread, NULL, ThreadEntry, this);
	}

	void Wait()
	{
		if (mThread)
		{
			// 待機
			pthread_join(mThread, NULL);
			mThread = 0;
		}
	}

	intptr_t GetThreadID() const
	{
		return (intptr_t)(mThread);
	}

	static void Sleep(int milliseconds)
	{
		usleep(milliseconds * 1000);
	}

	static intptr_t GetCurrentThreadId()
	{
		return (intptr_t)(pthread_self());
	}

private:
	static void* ThreadEntry(void* ptr)
	{
		Thread* obj = static_cast<Thread*>(ptr);

		// 実行
		try
		{
			obj->Execute();
		}
		catch (Exception& e)
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

	pthread_t		mThread;
};

} // namespace detail
LN_NAMESPACE_END
