
#pragma once

LN_NAMESPACE_BEGIN
namespace detail { class SemaphoreImpl; }

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
	detail::SemaphoreImpl*	m_impl;
};

LN_NAMESPACE_END
