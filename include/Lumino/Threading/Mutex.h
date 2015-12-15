
#pragma once

#include "../Base/NonCopyable.h"

LN_NAMESPACE_BEGIN
namespace Threading
{

/**
	@page	Doc_Threading
		- @ref Doc_Mutex_1

	@page	Doc_Mutex_1	単純な排他制御を行う

	@code
		Mutex	m_Mutex;
		・・・
		m_Mutex.Lock();
		// クリティカルセクション
		m_Mutex.Unlock();
	@endcode
*/

/**
	@brief	単純な排他制御を行うミューテックス
	@code
			Mutex	m_Mutex;
			・・・
			m_Mutex.Lock();
			// クリティカルセクション
			m_Mutex.Unlock();
	@endcode
 */
class LUMINO_EXPORT Mutex
    : public NonCopyable
{
public:
    Mutex();
    ~Mutex();

public:
	
	/// ロック
    void Lock();

	/// アンロック
    void Unlock();

private:
#ifdef LN_THREAD_WIN32
    mutable CRITICAL_SECTION m_CriticalSection;
#else
    pthread_mutex_t mMutex;
#endif
};

/**
	@brief	スコープ内でミューテックスをロックする
	@code
			Mutex	m_Mutex;
			・・・
			try {
				ScopedLock lock(m_Mutex);
				// クリティカルセクション内で return や throw しても

				// スコープ終端で Unlock される

				throw 0;
			}
			catch (){
			}
	@endcode
*/
class MutexScopedLock  
{
public:

    MutexScopedLock( Mutex& mutex )
        : m_Mutex( mutex )
    {
        m_Mutex.Lock();
    }

    ~MutexScopedLock()
    {
        m_Mutex.Unlock();
    }

private:

    Mutex& m_Mutex;
};

} // namespace Threading
LN_NAMESPACE_END
