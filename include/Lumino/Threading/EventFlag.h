
#pragma once

#include "../Base/Common.h"
#include "../Base/NonCopyable.h"

namespace Lumino
{
namespace Threading
{
/**
	@brief		�҂����킹�@�\�����X���b�h�Z�[�t�� bool �^�ϐ��̃N���X
*/
class LUMINO_EXPORT EventFlag
    : private NonCopyable
{
public:
	/// �f�t�H���g�R���X�g���N�^ (�����l�� false)
    EventFlag();

	/// �R���X�g���N�^ (�����l���w�肷��) 
	explicit EventFlag(bool initFlag);

	/// �f�X�g���N�^
	virtual ~EventFlag();

public:

    /// �l�� true �ɂ���
	void SetTrue();

	/// �l�� false �ɂ���
	void SetFalse();

	/// �l�� true ���𔻒肷��
	bool IsTrue() const;

	/// �l�� true �ɂȂ�܂őҋ@����
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
