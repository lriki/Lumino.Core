
#pragma once

#include "String.h"

// exp の条件が満たされなかった場合、type に指定した例外を throw する
#define LN_THROW( exp, type, ... )	{ if (!(exp)) { type e = type(__VA_ARGS__); e.SetSourceLocationInfo(__FILE__, __LINE__); throw e; } }

/// 例外クラスの基本的なコンストラクタを宣言する
#define LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(className) \
	className(const char* message, ...) \
	{ \
		va_list args; \
		va_start(args, message); \
		SetMessage(message, args); \
		va_end(args); \
	} \
	className(const wchar_t* message, ...) \
	{ \
		va_list args; \
		va_start(args, message); \
		SetMessage(message, args); \
		va_end(args); \
	}

namespace Lumino
{

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
	
	/**
		@brief	例外の詳細メッセージを取得します。
	*/
	virtual const TCHAR* GetMessage() const { return mMessage; }

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
	virtual const char* what() const  throw() { return mSymbolBuffer; }

protected:
	void SetMessage(const char* format, va_list args);
	void SetMessage(const wchar_t* format, va_list args);
	void SetMessage(const char* format, ...);
	void SetMessage(const wchar_t* format, ...);

private:
	static const int MaxMessageBufferSize = 1024;

	TCHAR		mSourceFilePath[LN_MAX_PATH];
	int			mSourceFileLine;
	void*		mStackBuffer[32];
	int			mStackBufferSize;
	char		mSymbolBuffer[2048];
	TCHAR		mMessage[MaxMessageBufferSize];
};

/**
	@brief	メモリ不足例外

*/
class OutOfMemoryException 
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(OutOfMemoryException);
	OutOfMemoryException() {}
	virtual ~OutOfMemoryException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW OutOfMemoryException( *this ); }
};

/**
	@brief		その他のIO例外
	@details	読み取り属性のファイルを書き込みモードでオープンしようとした時等。
*/
class IOException 
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(IOException);
	IOException() {}
	virtual ~IOException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW IOException( *this ); }
};

/**
	@brief	メソッドに渡された引数のいずれかが無効な場合にスローされる例外。
*/
class ArgumentException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(ArgumentException);
	ArgumentException() {}
	virtual ~ArgumentException() throw() {}

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
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(InvalidOperationException);
	InvalidOperationException() {}
	virtual ~InvalidOperationException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW InvalidOperationException(*this); }
};

/**
	@brief		サポートされない機能を呼び出そうとした
	@details	読み取り専用ストリームに対して書き込みを行った場合等。
*/
class NotSupportedException 
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(NotSupportedException);
	NotSupportedException() {}
	virtual ~NotSupportedException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW NotSupportedException( *this ); }
};

/**
	@brief	ディスク上に存在しないファイルにアクセスしようとして失敗したときにスローされる例外
*/
class FileNotFoundException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(FileNotFoundException);
	FileNotFoundException() {}
	virtual ~FileNotFoundException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW FileNotFoundException( *this ); }
};

/**
	@brief	無効なディレクトリにアクセスしようとしたときにスローされる例外
*/
class DirectoryNotFoundException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(DirectoryNotFoundException);
	DirectoryNotFoundException() {}
	virtual ~DirectoryNotFoundException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW DirectoryNotFoundException( *this ); }
};

/**
	@brief	ファイルや文字列等の形式が不正
*/
class InvalidFormatException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(InvalidFormatException);
	InvalidFormatException() {}
	virtual ~InvalidFormatException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW InvalidFormatException(*this); }
};

/**
	@brief	未実装の機能を呼び出した
*/
class NotImplementedException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(NotImplementedException);
	NotImplementedException() {}
	virtual ~NotImplementedException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW NotImplementedException( *this ); }
};

/**
	@brief	C/C++ランタイムAPI等でエラーが発生した
*/
class RuntimeException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(RuntimeException);
	RuntimeException() {}
	virtual ~RuntimeException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW RuntimeException(*this); }
};

/**
	@brief	文字コードの変換中、マッピングできない文字または不正シーケンスが見つかった
*/
class EncodingFallbackException
	: public Exception
{
public:
	LN_EXCEPTION_BASIC_CONSTRUCTOR_DECL(EncodingFallbackException);
	EncodingFallbackException() {}
	virtual ~EncodingFallbackException() throw() {}

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW EncodingFallbackException(*this); }
};


#ifdef LN_WIN32
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
#endif

} // namespace Lumino
