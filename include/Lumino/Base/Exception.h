﻿/**
	@file	Exception.h
*/
#pragma once
#include "Common.h"
#include <exception>
#include <assert.h>

// exp の条件が満たされなかった場合、type に指定した例外を throw する
#define LN_THROW( exp, type, ... )	{ if (!(exp)) { type e = type(__VA_ARGS__); e.SetSourceLocationInfo(__FILE__, __LINE__); throw e; } }

// HRESULT を返す関数の呼び出しユーティリティ (Win32 用)
#define LN_COMCALL(exp)				{ HRESULT hr = (exp); if (FAILED(hr)) { LN_THROW(0, COMException, hr); } }

/// 例外クラスの基本的なコンストラクタを宣言する
#define LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(className) \
	className(); \
	className(const char* message, ...); \
	className(const wchar_t* message, ...); \
	virtual ~className() throw()

/// 例外クラスの基本的なコンストラクタを定義する
#define LN_EXCEPTION_BASIC_CONSTRUCTOR_IMPLEMENT(className, captionStringId) \
	className::className() \
	{ \
		SetMessage(InternalResource::GetString(captionStringId).c_str()); \
	} \
	className::className(const char* message, ...) \
	{ \
		va_list args; \
		va_start(args, message); \
		SetMessage(InternalResource::GetString(captionStringId).c_str(), message, args); \
		va_end(args); \
	} \
	className::className(const wchar_t* message, ...) \
	{ \
		va_list args; \
		va_start(args, message); \
		SetMessage(InternalResource::GetString(captionStringId).c_str(), message, args); \
		va_end(args); \
	} \
	className::~className() throw() \
	{ \
	}


#define LN_DO_CHECK	// TODO: とりあえず
#define LN_DO_CHECK_THROW	// TODO: とりあえず

#ifdef LN_DO_CHECK
	#ifdef LN_DO_CHECK_THROW
		#define _LN_VERIFY_ASSERT_INTERNAL(exp, exception)	(AssertInternalThrow<exception>(exp, __FILE__, __LINE__))
	#else
		#define _LN_VERIFY_ASSERT_INTERNAL(exp, exception)	(AssertInternal(exp))
	#endif
#else
	#define _LN_VERIFY_ASSERT_INTERNAL(exp, exception)	(exp)
#endif

#ifdef LN_DO_CHECK_THROW
	#define _LN_ENSURE_ASSERT_INTERNAL(exp, exception)	(AssertInternalThrow<exception>(exp, __FILE__, __LINE__))
#else
	#define _LN_ENSURE_ASSERT_INTERNAL(exp, exception)	(AssertInternal(exp))
#endif

///*
//	LN_VERIFY	… assert と似た動作をするが、Release モードでも表現式は実行される。
//	LN_ENSURE	… Release モードでもエラーが生成される。現在は例外を throw する。
//*/
//
///**
//	@brief	LN_DO_CHECK が有効の場合、表現式を実行し結果が false であればアサートします。LN_DO_CHECK が無効でも表現式は実行されます。
//	@code
//			if (LN_VERIFY(stream != NULL))
//			{
//				stream->Read(...);
//			}
//	@endcode
//*/
//#define LN_VERIFY(exp)	_LN_VERIFY_ASSERT_INTERNAL(exp, Exception)
//
///**
//	@brief	LN_DO_CHECK が有効の場合、表現式を実行し結果が true であればアサートします。LN_DO_CHECK が無効でも表現式は実行されます。
//	@code
//			if (LN_VERIFY_ASSERT(stream != NULL)) { return; }
//			stream->Read(...);
//	@endcode
//*/
//#define LN_VERIFY_ASSERT(exp)	(!(_LN_VERIFY_ASSERT_INTERNAL(exp, Exception)))

/**
	@brief		式の結果が false であれば return により処理を中断します。
	@details	assert と似ていますが、DEBUG シンボルの定義有無にかかわらず式は必ず実行されます。
				LN_DO_CHECK が有効の場合、式の結果が false であればアサートします。
	@code
				void ToInt(const char* text, int** outValue)
				{
					LN_VERIFY_RETURN(text != NULL);
					LN_VERIFY_RETURN(outPos != NULL, "outValue is NULL.");
					...
					*outValue = XXXX;
				}
	@endcode
*/
#define LN_VERIFY_RETURN(exp, ...)					if (!_LN_VERIFY_ASSERT_INTERNAL(exp, ::ln::VerifyException)) { return; }

/**
	@brief		式の結果が false であれば return により処理を中断します。return で使用する戻り値を指定します。
	@see		LN_VERIFY_RETURN
	@code
				bool ToInt(const char* text, int** outValue)
				{
					LN_VERIFY_RETURN(text != NULL, false);
					LN_VERIFY_RETURN(outValue != NULL, false, "outValue is NULL.");
					...
					*outValue = XXXX;
					return true;
				}
	@endcode
*/
#define LN_VERIFY_RETURNV(exp, returnValue, ...)	if (!_LN_VERIFY_ASSERT_INTERNAL(exp, ::ln::VerifyException)) { return returnValue; }


// 試験運用中
#define LN_VERIFY_ARGS(exp)							_LN_VERIFY_ASSERT_INTERNAL(exp, ::ln::ArgumentException)
#define LN_VERIFY_ARGS_RETURN(exp)					if (!_LN_VERIFY_ASSERT_INTERNAL(exp, ::ln::ArgumentException)) { return; }
#define LN_VERIFY_ARGS_RETURNV(exp, returnValue)	if (!_LN_VERIFY_ASSERT_INTERNAL(exp, ::ln::ArgumentException)) { return returnValue; }

// 試験運用中。Release でも例外を throw する。中身は TODO。必ず throw なら RETURN はいらない気がする・・・
#define LN_CHECK_ARGS(exp)							_LN_VERIFY_ASSERT_INTERNAL(exp, ln::ArgumentException)
#define LN_CHECK_ARGS_RETURN(exp)					if (!_LN_VERIFY_ASSERT_INTERNAL(exp, ::ln::ArgumentException)) { return; }
#define LN_CHECK_ARGS_RETURNV(exp, returnValue)		if (!_LN_VERIFY_ASSERT_INTERNAL(exp, ::ln::ArgumentException)) { return returnValue; }
#define LN_CHECK_STATE(exp, ...)						_LN_VERIFY_ASSERT_INTERNAL(exp, ln::InvalidOperationException)
#define LN_CHECK_STATE_RETURN(exp, ...)					if (!_LN_VERIFY_ASSERT_INTERNAL(exp, ::ln::InvalidOperationException)) { return; }
#define LN_CHECK_STATE_RETURNV(exp, returnValue, ...)	if (!_LN_VERIFY_ASSERT_INTERNAL(exp, ::ln::InvalidOperationException)) { return returnValue; }


LN_NAMESPACE_BEGIN

	inline bool AssertInternal(bool result) 	// result が false であれば assert
	{ 
		assert(result);
		return result;
	}

	template<typename TException>
	inline bool AssertInternalThrow(bool result, const char* file, int line)	// result が false であれば throw
	{ 
		if (!result) {
			TException e = TException();// (__VA_ARGS__)
			e.SetSourceLocationInfo(file, line);
			throw e;
		}
		return result;
	}

/**
	@brief	例外ベースクラス

	@todo	VC++ で本ライブラリを DLL としてビルドするときは、DLLランタイムを使用しないと warning C4275 が発生する。
			これは、staticランタイムだと std::exception に dllexport がつかないため。
*/
class LUMINO_EXPORT Exception : public std::exception
{
public:
	Exception();
	virtual ~Exception() throw();
	Exception& SetSourceLocationInfo( const char* filePath, int fileLine );

public:
	
#pragma push_macro("GetMessage")
#undef GetMessage
	/**
		@brief	例外の詳細メッセージを取得します。
	*/
	const TCHAR* GetMessage() const;
	virtual const TCHAR* LN_AFX_FUNCNAME(GetMessage)() const;
#pragma pop_macro("GetMessage")

	/**
		@brief		例外発生時に詳細情報をダンプするファイルを初期化する
		@param[in]	filePath	: ダンプ先ファイル (文字数は LN_MAX_PATH 以内)
		@return		false の場合、ファイルアクセスに失敗した
		@details	指定されたパスで、ファイルを新規作成します。
					以降、この例外クラスのコンストラクタが呼ばれるたびに、詳細情報が追記されます。
	*/
	static bool InitDumpFile(const char* filePath);

	/**
		@brief		例外のコピーを作成する
		@note		別スレッドで発生した例外を保持するために使用する。
	*/
	virtual Exception* Copy() const { return LN_NEW Exception( *this ); }

public:
	// override std::exception
	virtual const char* what() const  throw() { return m_symbolBuffer; }

protected:
	void SetMessage(const TCHAR* caption);
	void SetMessage(const TCHAR* caption, const char* format, va_list args);
	void SetMessage(const TCHAR* caption, const wchar_t* format, va_list args);
	void SetMessage(const TCHAR* caption, const char* format, ...);
	void SetMessage(const TCHAR* caption, const wchar_t* format, ...);

private:
	void AppendMessage(const char* message, int len);
	void AppendMessage(const wchar_t* message, int len);

private:
	static const int MaxMessageBufferSize = 1024;

	TCHAR		m_sourceFilePath[LN_MAX_PATH];
	int			m_sourceFileLine;
	void*		m_stackBuffer[32];
	int			m_stackBufferSize;
	char		m_symbolBuffer[2048];
	TCHAR		m_message[MaxMessageBufferSize];
};

/**
	@brief	前提条件の検証エラーです。
*/
class VerifyException 
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(VerifyException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW VerifyException(*this); }
};

/**
	@brief	メソッドに渡された引数のいずれかが無効な場合にスローされる例外。
*/
class ArgumentException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(ArgumentException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW ArgumentException( *this ); }
};

/**
	@brief	オブジェクトの現在の状態に対して無効な呼び出しが行われた
*/
class InvalidOperationException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(InvalidOperationException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW InvalidOperationException(*this); }
};

/**
	@brief	未実装の機能を呼び出した
*/
class NotImplementedException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(NotImplementedException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW NotImplementedException( *this ); }
};

/**
	@brief	メモリ不足例外
*/
class OutOfMemoryException 
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(OutOfMemoryException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW OutOfMemoryException( *this ); }
};

/**
	@brief	許容範囲外の値が指定された場合にスローされる例外です。
*/
class OutOfRangeException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(OutOfRangeException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW OutOfRangeException(*this); }
};

/**
	@brief	検索に使用したキーが、コレクションのいずれのキーとも一致しない場合にスローされる例外です。
*/
class KeyNotFoundException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(KeyNotFoundException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW KeyNotFoundException(*this); }
};

/**
	@brief	数値演算によるオーバーフローが発生した。
*/
class OverflowException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(OverflowException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW OverflowException(*this); }
};

/**
	@brief		その他のIO例外
	@details	読み取り属性のファイルを書き込みモードでオープンしようとした時等。
*/
class IOException 
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(IOException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW IOException( *this ); }
};

/**
	@brief	ディスク上に存在しないファイルにアクセスしようとして失敗したときにスローされる例外
*/
class FileNotFoundException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(FileNotFoundException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW FileNotFoundException(*this); }
};

/**
	@brief	無効なディレクトリにアクセスしようとしたときにスローされる例外
*/
class DirectoryNotFoundException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(DirectoryNotFoundException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW DirectoryNotFoundException(*this); }
};

/**
	@brief	ファイルや文字列等の形式が不正
*/
class InvalidFormatException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(InvalidFormatException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW InvalidFormatException(*this); }
};

/**
	@brief		ストリームの末尾を越えて読み込もうとしたときにスローされる例外です。
*/
class EndOfStreamException
	: public IOException
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(EndOfStreamException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW EndOfStreamException(*this); }
};

/**
	@brief	文字コードの変換中、マッピングできない文字または不正シーケンスが見つかった
*/
class EncodingException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECLARE(EncodingException);

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW EncodingException(*this); }
};


#ifdef LN_OS_WIN32
/**
	@brief	WindowsAPI のエラーを示す例外クラス (GetLastError())
*/
class Win32Exception 
	: public Exception
{
public:
	Win32Exception(uint32_t/*DWORD*/ dwLastError);
	virtual ~Win32Exception() throw() {}

public:
	uint32_t/*DWORD*/			GetLastErrorCode() const { return m_dwLastErrorCode; }
	const TCHAR*	GetFormatMessage() const { return m_pFormatMessage; }

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW Win32Exception( *this ); }

private:
	uint32_t/*DWORD*/	m_dwLastErrorCode;
	TCHAR				m_pFormatMessage[512];	///< FormatMessage() で取得したメッセージ
};

/**
	@brief	COM のエラーを示す例外クラス (HRESULT)
*/
class COMException
	: public Exception
{
public:
	COMException(uint32_t hresult);
	virtual ~COMException() throw();

	uint32_t GetHRESULT() const { return m_HRESULT; }

public:
	// override Exception
	virtual Exception* Copy() const;

private:
	uint32_t	m_HRESULT;
};
#endif

LN_NAMESPACE_END
