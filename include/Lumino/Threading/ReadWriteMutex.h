
#pragma once

#include "../Base/NonCopyable.h"

LN_NAMESPACE_BEGIN
namespace Threading
{

/**
	@page	Doc_Threading
		- @ref Doc_ReadWriteMutex_1

	@page	Doc_ReadWriteMutex_1	複数の読み取りロックと、1つの書き込みロックを行う

	@code
		class Test
		{
		private:
			ReadWriteMutex m_RWMutex;
			int m_Value;

		public:
			int GetValue()
			{
				ScopedReadLock lock(m_RWMutex);
				return m_nValue;
			}

			void SetValue(int nValue)
			{
				ScopedWriteLock lock(m_RWMutex);
				m_nValue = nValue;
			}
		}
	@endcode
*/

/**
	@brief	複数の読み取りロックと、1つの書き込みロックを行う Mutex
*/
class ReadWriteMutex
	: public NonCopyable
{
public:
	ReadWriteMutex();
	~ReadWriteMutex();

public:

	/**
		@brief	読み取りロック
	*/
	void LockRead();

	/**
		@brief	読み取りアンロック
	*/
	void UnlockRead();

	/**
		@brief	書き込みロック
	*/
	void LockWrite();

	/**
		@brief	書き込みアンロック
	*/
	void UnlockWrite();

private:

#ifdef LN_THREAD_WIN32
	CRITICAL_SECTION    mReaderCountLock;
	CRITICAL_SECTION    mWriterLock;
	HANDLE              mNoReaders;		///< ReadLock() しているスレッドがひとつも無いか
	int                 mReaderCount;
#else
	pthread_mutex_t     mLock;
	pthread_cond_t      mRead;
	pthread_cond_t      mWrite;
	unsigned            mReaders;
	unsigned            mWriters;
	unsigned            mReadWaiters;
	unsigned            mWriteWaiters;
#endif
};

} // namespace Threading
LN_NAMESPACE_END
