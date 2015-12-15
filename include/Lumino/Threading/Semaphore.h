
#pragma once

LN_NAMESPACE_BEGIN

/**
	@brief	
*/
class Semaphore
{
public:

	/**
		@brief		�G���g�����̏����l�ƍő�l���w�肵�ăZ�}�t�H���쐬���܂��B
		@params[in]	initialCount	: (������Ԃŉ���ς݂̐�)
		@params[in]	maxCount		: �ő�G���g����
	*/
	Semaphore(int initialCount, int maxCount);

	~Semaphore();

	void Lock();

	void Unlock();

private:
	LN_DISALLOW_COPY_AND_ASSIGN(Semaphore);

#if defined(LN_OS_WIN32)
	HANDLE			m_handle;
	int				m_maxCount;
#else
	int             m_value;
	int             m_maxCount;
	pthread_mutex_t m_mutex;
	pthread_cond_t  m_condition;
#endif
};

LN_NAMESPACE_END
