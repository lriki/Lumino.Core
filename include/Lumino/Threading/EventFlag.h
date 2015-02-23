
#pragma once

#include "../Base/Common.h"
#include "../Base/NonCopyable.h"

namespace Lumino
{
namespace Threading
{
/**
	@brief		待ち合わせ機能を持つスレッドセーフな bool 型変数のクラス
*/
class LUMINO_EXPORT EventFlag
    : private NonCopyable
{
public:
	/// デフォルトコンストラクタ (初期値は false)
    EventFlag();

	/// コンストラクタ (初期値を指定する) 
	explicit EventFlag(bool initFlag);

	/// デストラクタ
	virtual ~EventFlag();

public:

    /// 値を true にする
	void SetTrue();

	/// 値を false にする
	void SetFalse();

	/// 値が true かを判定する
	bool IsTrue() const;

	/// 値が true になるまで待機する
    void Wait();

private:
#ifdef LN_THREAD_WIN32
    HANDLE			mHandle;
#else
	mutable pthread_mutex_t	mMutex;
	mutable pthread_cond_t	mWait;
	bool					mSignal;
#endif
};

} // namespace Threading
} // namespace Lumino
