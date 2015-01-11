
#pragma once

#include "../Base/NonCopyable.h"

namespace Lumino
{
namespace Threading
{
/**
	@page	Doc_Threading
		- @ref Doc_ReadWriteMutex_1

	@page	Doc_ReadWriteMutex_1	�����̓ǂݎ�胍�b�N�ƁA1�̏������݃��b�N���s��

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
	@brief	�����̓ǂݎ�胍�b�N�ƁA1�̏������݃��b�N���s�� Mutex
*/
class ReadWriteMutex
	: public NonCopyable
{
public:
	ReadWriteMutex();
	~ReadWriteMutex();

public:

	/**
		@brief	�ǂݎ�胍�b�N
	*/
	void LockRead();

	/**
		@brief	�ǂݎ��A�����b�N
	*/
	void UnlockRead();

	/**
		@brief	�������݃��b�N
	*/
	void LockWrite();

	/**
		@brief	�������݃A�����b�N
	*/
	void UnlockWrite();

private:

#ifdef LN_THREAD_WIN32
	CRITICAL_SECTION    mReaderCountLock;
	CRITICAL_SECTION    mWriterLock;
	HANDLE              mNoReaders;		///< ReadLock() ���Ă���X���b�h���ЂƂ�������
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
} // namespace Lumino
