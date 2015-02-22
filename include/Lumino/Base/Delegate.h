
#pragma once

#include "RefObject.h"

namespace Lumino
{

/**
	@brief		関数ポインタのラッパークラス
	@details
				デリゲートはクラスインスタンスへのポインタとメンバ関数のポインタを保持し、
				関数ポインタ経由の呼び出しを簡略化します。
				メンバ関数だけでなく、static 関数もデリゲートでラップすることができます。
				設定できる関数は、戻り値が void で引数が 0～8 個の関数です。
				例)
				・Delegate00						… void Func();
				・Delegate01<int>					… void Func(int p1);
				・Delegate02<const char*, Test*>	… void Func(const char* p1, Test* p2);
	@code
				class Test
				{
					void Run(Delegate01<int> func) {
						func.Call(10);
					}
					void Callback(int value) {
						printf("%d", value);
					}
					void Main() {
						Run(LN_CreateDelegate(this, &Test::Thread_StoreAllTestPoints));
					}
				}
	@endcode
 */
class LUMINO_EXPORT Delegate00
{
private:

	/// 関数ポインタを保持する内部クラスの基底
	class DelegateHolderBase
		: public RefObject
	{
	public:
		virtual void call() const = 0;
	};

	/// DelegateHolderStatic (static 関数呼び出し時の実体)
	class DelegateHolderStatic
		: public DelegateHolderBase
	{
	private:
		typedef void (LN_STDCALL *StaticFunction)();

	private:
		StaticFunction	mFunction;

	public:
		DelegateHolderStatic( StaticFunction function )
			: mFunction	( function )
        {}

		virtual void call() const { mFunction(); }
	};

	/// DelegateHolderDynamics (メンバ関数呼び出し時の実体)
	template < typename T >
	class DelegateHolderDynamics
		: public DelegateHolderBase
	{
	private:
		typedef void (T::*ClassMethod)();

	private:
		T*			mClass;
		ClassMethod	mMethod;

	public:
		DelegateHolderDynamics( T* objPtr, ClassMethod method )
			: mClass		( objPtr )
			, mMethod		( method )
		{}
		virtual void call() const { (mClass->*mMethod)(); }
	};

private:
	DelegateHolderBase* mDelegate;

public:

	Delegate00()
		: mDelegate ( NULL )
	{}

	Delegate00( const Delegate00& d )
		: mDelegate ( NULL )
	{
		*this = d;
	}

    Delegate00( void (LN_STDCALL* method)() )
		: mDelegate ( LN_NEW DelegateHolderStatic( method ) )
	{}

	template < typename T >
	Delegate00( T* objPtr, void (T::*method)() )
		: mDelegate ( LN_NEW DelegateHolderDynamics<T>( objPtr, method ) )
	{}

	~Delegate00() { ReleaseMethod(); }

public:

	void ReleaseMethod() { LN_SAFE_RELEASE( mDelegate ); }

	bool IsEmpty() const { return mDelegate == NULL; }

	void Call() const { if ( mDelegate != NULL ) mDelegate->call(); }

	void operator ()() const { Call(); }

	Delegate00& operator = ( const Delegate00& d )
	{
        LN_SAFE_ADDREF( d.mDelegate );
		ReleaseMethod();
		mDelegate = d.mDelegate;
		return *this;
	}
};

/// CreateDelegate
template < typename T >
Delegate00 LN_CreateDelegate( T* objPtr, void (T::*method)() )
{
	return Delegate00( objPtr, method );
}

//==============================================================================
// Delegate xx
//==============================================================================
#define LN_DELEGATE_CLASS_NAME		Delegate01
#define LN_DELEGATE_TEMPLATE_ARGS 	typename A1
#define LN_DELEGATE_ARGS_DECL 		A1 a1
#define LN_DELEGATE_CALL_ARGS 		a1
#define LN_DELEGATE_TEMPLATE_TYPES  A1
#include "Delegate.inl"

#define LN_DELEGATE_CLASS_NAME		Delegate02
#define LN_DELEGATE_TEMPLATE_ARGS 	typename A1, typename A2
#define LN_DELEGATE_ARGS_DECL 		A1 a1, A2 a2
#define LN_DELEGATE_CALL_ARGS 		a1, a2
#define LN_DELEGATE_TEMPLATE_TYPES  A1, A2
#include "Delegate.inl"

#define LN_DELEGATE_CLASS_NAME		Delegate03
#define LN_DELEGATE_TEMPLATE_ARGS 	typename A1, typename A2, typename A3
#define LN_DELEGATE_ARGS_DECL 		A1 a1, A2 a2, A3 a3
#define LN_DELEGATE_CALL_ARGS 		a1, a2, a3
#define LN_DELEGATE_TEMPLATE_TYPES  A1, A2, A3
#include "Delegate.inl"

#define LN_DELEGATE_CLASS_NAME		Delegate04
#define LN_DELEGATE_TEMPLATE_ARGS 	typename A1, typename A2, typename A3, typename A4
#define LN_DELEGATE_ARGS_DECL 		A1 a1, A2 a2, A3 a3, A4 a4
#define LN_DELEGATE_CALL_ARGS 		a1, a2, a3, a4
#define LN_DELEGATE_TEMPLATE_TYPES  A1, A2, A3, A4
#include "Delegate.inl"

#define LN_DELEGATE_CLASS_NAME		Delegate05
#define LN_DELEGATE_TEMPLATE_ARGS 	typename A1, typename A2, typename A3, typename A4, typename A5
#define LN_DELEGATE_ARGS_DECL 		A1 a1, A2 a2, A3 a3, A4 a4, A5 a5
#define LN_DELEGATE_CALL_ARGS 		a1, a2, a3, a4, a5
#define LN_DELEGATE_TEMPLATE_TYPES  A1, A2, A3, A4, A5
#include "Delegate.inl"

#define LN_DELEGATE_CLASS_NAME		Delegate06
#define LN_DELEGATE_TEMPLATE_ARGS 	typename A1, typename A2, typename A3, typename A4, typename A5, typename A6
#define LN_DELEGATE_ARGS_DECL 		A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6
#define LN_DELEGATE_CALL_ARGS 		a1, a2, a3, a4, a5, a6
#define LN_DELEGATE_TEMPLATE_TYPES  A1, A2, A3, A4, A5, A6
#include "Delegate.inl"

#define LN_DELEGATE_CLASS_NAME		Delegate07
#define LN_DELEGATE_TEMPLATE_ARGS 	typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7
#define LN_DELEGATE_ARGS_DECL 		A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7
#define LN_DELEGATE_CALL_ARGS 		a1, a2, a3, a4, a5, a6, a7
#define LN_DELEGATE_TEMPLATE_TYPES  A1, A2, A3, A4, A5, A6, A7
#include "Delegate.inl"

#define LN_DELEGATE_CLASS_NAME		Delegate08
#define LN_DELEGATE_TEMPLATE_ARGS 	typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8
#define LN_DELEGATE_ARGS_DECL 		A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8
#define LN_DELEGATE_CALL_ARGS 		a1, a2, a3, a4, a5, a6, a7, a8
#define LN_DELEGATE_TEMPLATE_TYPES  A1, A2, A3, A4, A5, A6, A7, A8
#include "Delegate.inl"

} // namespace Lumino