
#pragma once

namespace Lumino
{
	
/**
	@brief	経過時間を測定するためのクラスです。
*/
class ElapsedTimer
{
public:
	ElapsedTimer();
	~ElapsedTimer();

public:

	/// 計測開始
	void Start();

	/// 経過時間の取得 (ms)
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
