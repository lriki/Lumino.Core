
#pragma once

namespace Lumino
{
	
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

private:
#ifdef LN_WIN32
    LARGE_INTEGER   mFreq;
    LARGE_INTEGER   mBefore;
#else
	uint64_t		m_start;	// (us)
#endif
};

} // namespace Lumino
