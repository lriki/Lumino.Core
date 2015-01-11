
#pragma once

#include "../Base/NonCopyable.h"

namespace Lumino
{
namespace Threading
{
/**
	@page	Doc_Threading
		- @ref Doc_Mutex_1

	@page	Doc_Mutex_1	�P���Ȕr��������s��

	@code
		Mutex	m_Mutex;
		�E�E�E
		m_Mutex.Lock();
		// �N���e�B�J���Z�N�V����
		m_Mutex.Unlock();
	@endcode
*/

/**
	@brief	�P���Ȕr��������s���~���[�e�b�N�X
	@code
			Mutex	m_Mutex;
			�E�E�E
			m_Mutex.Lock();
			// �N���e�B�J���Z�N�V����
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
	
	/// ���b�N
    void Lock();

	/// �A�����b�N
    void Unlock();

private:
#ifdef LN_THREAD_WIN32
    mutable CRITICAL_SECTION m_CriticalSection;
#else
    pthread_mutex_t mMutex;
#endif
};

/**
	@brief	�X�R�[�v���Ń~���[�e�b�N�X�����b�N����
	@code
			Mutex	m_Mutex;
			�E�E�E
			try {
				ScopedLock lock(m_Mutex);
				// �N���e�B�J���Z�N�V�������� return �� throw ���Ă�

				// �X�R�[�v�I�[�� Unlock �����

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
} // namespace Lumino
