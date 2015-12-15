
#pragma once

LN_NAMESPACE_BEGIN

/**
	@brief	
*/
class Semaphore
{
public:

	/**
		@brief		エントリ数の初期値と最大値を指定してセマフォを作成します。
		@params[in]	initialCount	: (初期状態で解放済みの数)
		@params[in]	maxCount		: 最大エントリ数
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
