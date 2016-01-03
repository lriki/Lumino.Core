
#pragma once

#include "NonCopyable.h"
#include "../Threading/Atomic.h"

LN_NAMESPACE_BEGIN

/// 参照カウントのインクリメント
#ifndef LN_SAFE_ADDREF
	#define LN_SAFE_ADDREF( p ) { if ( p ) { (p)->AddRef(); } }
#endif

/// 参照カウントのデクリメント
#ifndef LN_SAFE_RELEASE
	#define LN_SAFE_RELEASE( p ) { if ( p ) { (p)->Release(); (p)= NULL; } }
#endif

/// a に b を格納するユーティリティ
#define LN_REFOBJ_SET( a, b ) \
{ \
    LN_SAFE_ADDREF( b ); \
	if (a) (a)->Release(); \
    (a) = (b); \
}

/**
	@brief	参照カウントを持つクラスの基底
*/
class RefObject
    : private NonCopyable
{
public:
	RefObject();
protected:
	virtual ~RefObject();

public:

	/// 参照カウントの取得
	virtual int32_t GetRefCount() const;

	/// 参照カウントをインクリメントする
	virtual int32_t AddRef();

	/// 参照カウントをデクリメントする
	virtual int32_t Release();

	void TryGCAddRef();
	void GCRelease();

protected:
    Threading::Atomic	mReferenceCount;	///< 参照カウント
	Threading::Atomic	m_refPtrReferenced;
};

class RefPtrCore {};

/**
	@brief		RefObject 用 スマートポインタ
	@attention	CArray や std::vector 等のコンテナへの格納は未チェック
*/
template <class T>
class RefPtr : public RefPtrCore
{
public:
	typedef T* PtrType;

public:

	template<typename... TArgs>
	static RefPtr<T> Create(TArgs... args)
	{
		return RefPtr<T>(LN_NEW T(args...), false);
	}

	template<typename... TArgs>
	static RefPtr<T> Construct(TArgs... args)
	{
		return RefPtr<T>(LN_NEW T(args...), false);
	}

	/**
		@brief		コンストラクタ
	*/
	RefPtr()
		: mPtr( NULL )
	{ }

	/**
		@brief		コンストラクタ
		@param[in]	ptr		: 管理対象としてセットする ReferenceObject インスタンスのポインタ
		@param[in]	addRef	: true の場合、セットされた ReferenceObject の参照カウントをインクリメントする
	*/
	RefPtr( T* ptr, bool addRef = true/* = false*/)
		: mPtr( ptr )
	{
		if ( addRef ) {
			LN_SAFE_ADDREF( mPtr );
		}
	}

	/**
		@brief		コピーコンストラクタ
		@param[in]	obj		:
	*/
	RefPtr( const RefPtr<T>& obj )
		: mPtr( obj.mPtr )
	{
		LN_SAFE_ADDREF( mPtr );
	}

	/**
		@brief		デストラクタ
	*/
	virtual ~RefPtr()
	{
		LN_SAFE_RELEASE( mPtr );
	}

public:

	/**
		@brief		ReferenceObject インスタンスのポインタを管理対象としてセットする
		@param[in]	ptr		: 管理対象としてセットする ReferenceObject インスタンスのポインタ
		@param[in]	addRef	: true の場合、セットされた ReferenceObject の参照カウントをインクリメントする
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
		@brief		管理対象オブジェクトの参照カウントをインクリメントする
	*/
	void SafeAddRef()
	{ 
		LN_SAFE_ADDREF( mPtr );
	}

	/**
		@brief		管理対象オブジェクトの参照カウントをデクリメントし、管理対象から外す
	*/
    void SafeRelease()
	{
		LN_SAFE_RELEASE( mPtr );
	}

	T* DetachMove()
	{
		T* ptr = mPtr;
		mPtr = nullptr;
		return ptr;
	}

	/**
		@brief		管理対象オブジェクトへのポインタが NULL であるかを確認する
	*/
	bool IsNull() const { return (mPtr == NULL); }

	/**
		@brief		管理対象オブジェクトへのポインタを取得する
	*/
	const T* GetObjectPtr() const	{ return mPtr; }

	/**
		@brief		管理対象オブジェクトへのポインタを取得する
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
	
	T** operator&() throw()
	{
		//LN_ASSERT(mPtr == NULL);
		return &mPtr;
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
	operator T*			() const { return mPtr; }
    operator const T*	() const { return mPtr; }

protected:
	T* mPtr;
};

LN_NAMESPACE_END
