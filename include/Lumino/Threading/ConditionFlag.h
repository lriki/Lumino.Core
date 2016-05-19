
#pragma once

#include "../Base/Common.h"
#include "../Base/NonCopyable.h"

LN_NAMESPACE_BEGIN
namespace Threading
{

/**
	@brief		待ち合わせ機能を持つスレッドセーフな bool 型変数のクラス
*/
class LUMINO_EXPORT ConditionFlag
    : private NonCopyable
{
public:
	/// デフォルトコンストラクタ (初期値は false)
	ConditionFlag();

	/// コンストラクタ (初期値を指定する) 
	explicit ConditionFlag(bool initFlag);

	/// デストラクタ
	virtual ~ConditionFlag();

public:

    /// 値を true にする
	void SetTrue();

	/// 値を false にする
	void SetFalse();

	/// 値が true かを判定する
	bool IsTrue() const;

	/// 値が false かを判定する
	bool IsFalse() const;

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
LN_NAMESPACE_END
