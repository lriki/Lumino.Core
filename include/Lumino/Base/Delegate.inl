
template < LN_DELEGATE_TEMPLATE_ARGS >
class LN_DELEGATE_CLASS_NAME
{
private:
	/// 関数ポインタを保持する内部クラスの基底
	class DelegateHolderBase
		: public RefObject
	{
	public:
		virtual void call( LN_DELEGATE_ARGS_DECL ) const = 0;
	};

	/// DelegateHolderStatic (static 関数呼び出し時の実体)
	class DelegateHolderStatic
		: public DelegateHolderBase
	{
	private:
		typedef void (LN_STDCALL *StaticFunction)( LN_DELEGATE_TEMPLATE_TYPES );

	private:
		StaticFunction	mFunction;

	public:
		DelegateHolderStatic( StaticFunction function )
			: mFunction	( function )
		{}

		virtual void call( LN_DELEGATE_ARGS_DECL ) const
		{
			mFunction( LN_DELEGATE_CALL_ARGS );
		}
	};

	/// DelegateHolderDynamics (メンバ関数呼び出し時の実体)
	template < typename T >
	class DelegateHolderDynamics
		: public DelegateHolderBase
	{
	private:
		typedef void (T::*ClassMethod)( LN_DELEGATE_ARGS_DECL );

	private:
		T*			mObjPtr;
		ClassMethod	mMethod;

	public:
		DelegateHolderDynamics( T* objPtr, ClassMethod method )
			: mObjPtr		( objPtr )
			, mMethod		( method )
		{}

		virtual void call( LN_DELEGATE_ARGS_DECL ) const
		{
			(mObjPtr->*mMethod)( LN_DELEGATE_CALL_ARGS );
		}
	};

private:
	DelegateHolderBase*		mDelegate;

public:
	LN_DELEGATE_CLASS_NAME()
		: mDelegate	( NULL )
	{
	}

	LN_DELEGATE_CLASS_NAME( const LN_DELEGATE_CLASS_NAME& value )
		: mDelegate	( NULL )
	{
		*this = value;
	}

	template < typename T >
	LN_DELEGATE_CLASS_NAME( T* objPtr, void (T::*method)(LN_DELEGATE_ARGS_DECL) )
		: mDelegate	( new DelegateHolderDynamics<T>( objPtr, method ) )
	{
	}

	LN_DELEGATE_CLASS_NAME( void (LN_STDCALL *function)(LN_DELEGATE_ARGS_DECL) )
		: mDelegate ( new DelegateHolderStatic( function ) )
	{
	}

	~LN_DELEGATE_CLASS_NAME()
	{
		Reset();
	}

public:
	void Reset()
	{
		LN_SAFE_RELEASE( mDelegate );
	}

	bool IsEmpty() const
	{
		return mDelegate == NULL;
	}

	void Call( LN_DELEGATE_ARGS_DECL ) const
	{
		if ( mDelegate != NULL )
		{
			mDelegate->call( LN_DELEGATE_CALL_ARGS );
		}
	}

	void operator ()( LN_DELEGATE_ARGS_DECL ) const
	{
		Call( LN_DELEGATE_CALL_ARGS );
	}

	LN_DELEGATE_CLASS_NAME& operator = ( const LN_DELEGATE_CLASS_NAME& value )
	{
		Reset();
		mDelegate = value.mDelegate;
		if ( mDelegate != NULL )
			mDelegate->AddRef();

		return *this;
	}
};

template < typename T, LN_DELEGATE_TEMPLATE_ARGS >
LN_DELEGATE_CLASS_NAME< LN_DELEGATE_TEMPLATE_TYPES > LN_CreateDelegate( T* objPtr, void (T::*method)(LN_DELEGATE_TEMPLATE_TYPES) )
{
	return LN_DELEGATE_CLASS_NAME< LN_DELEGATE_TEMPLATE_TYPES >( objPtr, method );
}

template < LN_DELEGATE_TEMPLATE_ARGS >
LN_DELEGATE_CLASS_NAME< LN_DELEGATE_TEMPLATE_TYPES > LN_CreateDelegate( void (LN_STDCALL *method)(LN_DELEGATE_TEMPLATE_TYPES) )
{
	return LN_DELEGATE_CLASS_NAME< LN_DELEGATE_TEMPLATE_TYPES >( method );
}

#undef LN_DELEGATE_CLASS_NAME
#undef LN_DELEGATE_TEMPLATE_ARGS
#undef LN_DELEGATE_ARGS_DECL
#undef LN_DELEGATE_CALL_ARGS
#undef LN_DELEGATE_TEMPLATE_TYPES
