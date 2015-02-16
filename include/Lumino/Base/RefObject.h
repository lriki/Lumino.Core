
#pragma once

#include "NonCopyable.h"
#include "../Threading/Atomic.h"

namespace Lumino
{

/// �Q�ƃJ�E���g�̃C���N�������g
#ifndef LN_SAFE_ADDREF
	#define LN_SAFE_ADDREF( p ) { if ( p ) { p->AddRef(); } }
#endif

/// �Q�ƃJ�E���g�̃f�N�������g
#ifndef LN_SAFE_RELEASE
	#define LN_SAFE_RELEASE( p ) { if ( p ) { ( p )->Release(); ( p ) = NULL; } }
#endif

/// a �� b ���i�[���郆�[�e�B���e�B
#define LN_REFOBJ_SET( a, b ) \
{ \
    LN_SAFE_ADDREF( b ); \
    LN_SAFE_RELEASE( a ); \
    a = b; \
}

/**
	@brief	�Q�ƃJ�E���g�����N���X�̊��
*/
class RefObject
    : private NonCopyable
{
public:
	RefObject();
protected:
	virtual ~RefObject();

public:

	/// �Q�ƃJ�E���g�̎擾
	virtual int32_t GetRefCount() const;

	/// �Q�ƃJ�E���g���C���N�������g����
	virtual int32_t AddRef();

	/// �Q�ƃJ�E���g���f�N�������g����
	virtual int32_t Release();

protected:
    Threading::Atomic	mReferenceCount;	///< �Q�ƃJ�E���g
};

/**
	@brief		RefObject �p �X�}�[�g�|�C���^
	@attention	CArray �� std::vector ���̃R���e�i�ւ̊i�[�͖��`�F�b�N
*/
template <class T>
class RefPtr
{
public:

	/**
		@brief		�R���X�g���N�^
	*/
	RefPtr()
		: mPtr( NULL )
	{ }

	/**
		@brief		�R���X�g���N�^
		@param[in]	ptr		: �Ǘ��ΏۂƂ��ăZ�b�g���� ReferenceObject �C���X�^���X�̃|�C���^
		@param[in]	addRef	: true �̏ꍇ�A�Z�b�g���ꂽ ReferenceObject �̎Q�ƃJ�E���g���C���N�������g����
	*/
	explicit RefPtr( T* ptr, bool addRef = false)
		: mPtr( ptr )
	{
		if ( addRef ) {
			LN_SAFE_ADDREF( mPtr );
		}
	}

	/**
		@brief		�R�s�[�R���X�g���N�^
		@param[in]	obj		:
	*/
	RefPtr( const RefPtr<T>& obj )
		: mPtr( obj.mPtr )
	{
		LN_SAFE_ADDREF( mPtr );
	}

	/**
		@brief		�f�X�g���N�^
	*/
	virtual ~RefPtr()
	{
		LN_SAFE_RELEASE( mPtr );
	}

public:

	/**
		@brief		ReferenceObject �C���X�^���X�̃|�C���^���Ǘ��ΏۂƂ��ăZ�b�g����
		@param[in]	ptr		: �Ǘ��ΏۂƂ��ăZ�b�g���� ReferenceObject �C���X�^���X�̃|�C���^
		@param[in]	addRef	: true �̏ꍇ�A�Z�b�g���ꂽ ReferenceObject �̎Q�ƃJ�E���g���C���N�������g����
	*/
	void Attach( T* ptr, bool addRef = false )
    { 
        SafeRelease();
        mPtr = ptr; 
		if ( addRef ) {
			SafeAddRef();
		}
    }

	/**
		@brief		�Ǘ��ΏۃI�u�W�F�N�g�̎Q�ƃJ�E���g���C���N�������g����
	*/
	void SafeAddRef()
	{ 
		LN_SAFE_ADDREF( mPtr );
	}

	/**
		@brief		�Ǘ��ΏۃI�u�W�F�N�g�̎Q�ƃJ�E���g���f�N�������g���A�Ǘ��Ώۂ���O��
	*/
    void SafeRelease()
	{
		LN_SAFE_RELEASE( mPtr );
	}

	/**
		@brief		�Ǘ��ΏۃI�u�W�F�N�g�ւ̃|�C���^�� NULL �ł��邩���m�F����
	*/
	bool IsNull() const { return (mPtr == NULL); }

	/**
		@brief		�Ǘ��ΏۃI�u�W�F�N�g�ւ̃|�C���^���擾����
	*/
	const T* GetObjectPtr() const	{ return mPtr; }

	/**
		@brief		�Ǘ��ΏۃI�u�W�F�N�g�ւ̃|�C���^���擾����
	*/
	T* GetObjectPtr() { return mPtr; }

public:

	/// operator=
	RefPtr<T>& operator =(const RefPtr<T>& ptr)
	{
		LN_REFOBJ_SET(mPtr, ptr.mPtr);
		return *this;
	}

	/// operator=
	RefPtr<T>& operator =( T* ptr )
	{
		LN_REFOBJ_SET( mPtr, ptr );
		return *this;
	}

	/// operator!
    bool operator ! () const { return ( mPtr == NULL ); }
    
    /// operator== 
    bool operator == ( const T* ptr ) const { return ( mPtr == ptr ); }

    /// operator!=
    bool operator != ( const T* ptr ) const { return ( mPtr != ptr ); }

    // operator< (for STL cmp)
    bool operator < ( const T* ptr ) const { return mPtr < ptr; }

	/// operator*
    T& operator* ()
    {
        LN_ASSERT( mPtr != NULL );
        return *mPtr;
    }

	/// operator*
    const T& operator* () const
    {
        LN_ASSERT( mPtr != NULL );
        return *mPtr;
    }

    /// ->
    T* operator -> ()
    {
        LN_ASSERT( mPtr != NULL );
        return mPtr;
    }

	/// ->
    const T* operator -> () const
    {
        LN_ASSERT( mPtr != NULL );
        return mPtr;
    }

    /// convert
    operator T*			() { return mPtr; }
    operator const T*	() const { return mPtr; }

private:
	T* mPtr;
};

} // namespace Lumino
