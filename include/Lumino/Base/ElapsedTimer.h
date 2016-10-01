
#pragma once
#include "Common.h"

LN_NAMESPACE_BEGIN
	
/**
	@brief	経過時間を測定するためのクラスです。
*/
class ElapsedTimer
{
public:
	ElapsedTimer();
	~ElapsedTimer();

	/** 計測開始 */
	void Start();

	/** 経過時間の取得 (ms) */
	uint64_t GetElapsedTime() const;

	/** 経過時間の取得 (ns) */
	uint64_t GetElapsedTimeNS() const;

private:
	uint64_t		m_freq;
	uint64_t		m_start;
};

LN_NAMESPACE_END
