
#pragma once
#include "Common.h"

LN_NAMESPACE_BEGIN
	
/**
	@brief	�o�ߎ��Ԃ𑪒肷�邽�߂̃N���X�ł��B
*/
class ElapsedTimer
{
public:
	ElapsedTimer();
	~ElapsedTimer();

public:

	/// �v���J�n
	void Start();

	/// �o�ߎ��Ԃ̎擾 (ms)
	uint64_t GetElapsedTime() const;

	/// �o�ߎ��Ԃ̎擾 (ns)
	uint64_t GetElapsedTimeNS() const;

private:
#ifdef LN_OS_WIN32
    LARGE_INTEGER   mFreq;
    LARGE_INTEGER   mBefore;
#else
	uint64_t		m_start;	// (us)
#endif
};

LN_NAMESPACE_END
