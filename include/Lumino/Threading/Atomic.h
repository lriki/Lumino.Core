
#pragma once

#include "../Base/Common.h"
#include "../Base/NonCopyable.h"

namespace Lumino
{
namespace Threading
{
	
/**
	@brief		スレッドセーフなインクリメント・デクリメントを行う整数変数のクラス
*/
class LUMINO_EXPORT Atomic
    : private NonCopyable
{
public:
	Atomic( int32_t initialValue = 0 );
	~Atomic();

public:

    /// 値を取得する
	int32_t Get() const;

    /// 値を設定する
    void Set( int32_t value );

    /// 値をインクリメントする (インクリメント後の値 を返す)
    int32_t Increment();

    /// 値をデクリメントする (デクリメント後の値 を返す)
    int32_t Decrement();

private:
#ifdef LN_THREAD_WIN32
    volatile LONG			mValue;
#else
	volatile int32_t		mValue;
    mutable pthread_mutex_t	mMutex;
#endif
};

} // namespace Threading
} // namespace Lumino
