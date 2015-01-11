
#pragma once

#include "../Base/Common.h"
#include "../Base/NonCopyable.h"

namespace Lumino
{
namespace Threading
{
	
/**
	@brief		�X���b�h�Z�[�t�ȃC���N�������g�E�f�N�������g���s�������ϐ��̃N���X
*/
class LUMINO_EXPORT Atomic
    : private NonCopyable
{
public:
	Atomic( int32_t initialValue = 0 );
	~Atomic();

public:

    /// �l���擾����
	int32_t Get() const;

    /// �l��ݒ肷��
    void Set( int32_t value );

    /// �l���C���N�������g���� (�C���N�������g��̒l ��Ԃ�)
    int32_t Increment();

    /// �l���f�N�������g���� (�f�N�������g��̒l ��Ԃ�)
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
