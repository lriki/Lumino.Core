
#include <time.h>
#include <algorithm>
#include "../Internal.h"
#include "../../include/Lumino/Base/CRT.h"
#include "../../include/Lumino/Base/Exception.h"
#include "../../include/Lumino/Base/StringUtils.h"

#ifdef LN_EXCEPTION_BACKTRACE
	#ifdef LN_WIN32	// Cygwin もこっち
		#include "Win32/BackTrace.h"
	#else
		#include "Unix/BackTrace.h"
	#endif
#endif

namespace Lumino
{

//=============================================================================
// Exception
//=============================================================================

static char gDumpFilePath[LN_MAX_PATH] = { 0 }; 

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Exception::Exception()
	: mSourceFileLine(0)
	, mStackBufferSize(0)
{
	memset(mStackBuffer, 0, sizeof(mStackBuffer));
	memset(mSymbolBuffer, 0, sizeof(mSymbolBuffer));

#ifdef LN_EXCEPTION_BACKTRACE
	// バックトレース記録
	mStackBufferSize = BackTrace::GetInstance()->Backtrace(mStackBuffer, LN_ARRAY_SIZE_OF(mStackBuffer));

	// バックトレース文字列取得
	BackTrace::GetInstance()->AddressToFullSymbolString(
		mStackBuffer, 
		std::min(mStackBufferSize, 32),
		mSymbolBuffer, 
		LN_ARRAY_SIZE_OF(mSymbolBuffer));
#endif

	// ファイルに保存
	if (strlen(gDumpFilePath) > 0) 
	{
		FILE* fp;
		if (fopen_s(&fp, gDumpFilePath, "w") == 0) 
		{
			time_t timer;
			timer = ::time(NULL);

			char str[256];
			ctime_s(str, 256, &timer);
			
			fprintf(fp, "%s\n%s\n\n", str, mSymbolBuffer);
			fclose(fp);
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Exception::~Exception() throw()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Exception& Exception::SetSourceLocationInfo(const char* filePath, int fileLine)
{
	// もしバックトレースが取れていなかったらそれ用の文字列バッファに入れてしまう
	if (mSymbolBuffer[0] == 0x00)
	{
		sprintf_s(mSymbolBuffer, LN_ARRAY_SIZE_OF(mSymbolBuffer), "File:%s Line:%d", filePath, fileLine);
	}

	// ワイド文字列へ変換 (文字コードは考慮しない)
	memset(mSourceFilePath, 0, sizeof(mSourceFilePath));
	size_t size;
	errno_t err = mbstowcs_s(&size, mSourceFilePath, LN_MAX_PATH, filePath, LN_MAX_PATH - 1);
	if (err != 0) {
		return *this;
	}

	mSourceFileLine = fileLine;
	return *this;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Exception::InitDumpFile(const char* filePath)
{
	// ファイルパス保持
	memset(gDumpFilePath, 0, sizeof(gDumpFilePath));
	strcpy_s(gDumpFilePath, LN_MAX_PATH, filePath);

	// ファイル初期化
	FILE* fp;
	if (fopen_s(&fp, filePath, "w") == 0) {
		fclose(fp);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Exception::SetMessage(const char* format, va_list args)
{
	static const int BUFFER_SIZE = MaxMessageBufferSize;
	char buf[BUFFER_SIZE];

	int len = StringUtils::VSPrintf(buf, BUFFER_SIZE, format, args);
	if (len >= BUFFER_SIZE)
	{
		// バッファに収まりきらない場合は終端を ... にして切る
		buf[BUFFER_SIZE - 4] = '.';
		buf[BUFFER_SIZE - 3] = '.';
		buf[BUFFER_SIZE - 2] = '.';
		buf[BUFFER_SIZE - 1] = '\0';
	}

	// TCHAR に合わせてメンバに格納
#ifdef LN_UNICODE
	size_t wlen;
	mbstowcs_s(&wlen, mMessage, BUFFER_SIZE, buf, _TRUNCATE);
#else
	strcpy_s(mMessage, BUFFER_SIZE, buf);
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Exception::SetMessage(const wchar_t* format, va_list args)
{
	static const int BUFFER_SIZE = MaxMessageBufferSize;
	wchar_t buf[BUFFER_SIZE];

	int len = StringUtils::VSPrintf(buf, BUFFER_SIZE, format, args);
	if (len >= BUFFER_SIZE)
	{
		// バッファに収まりきらない場合は終端を ... にして切る
		buf[BUFFER_SIZE - 4] = L'.';
		buf[BUFFER_SIZE - 3] = L'.';
		buf[BUFFER_SIZE - 2] = L'.';
		buf[BUFFER_SIZE - 1] = L'\0';
	}

	// TCHAR に合わせてメンバに格納
#ifdef LN_UNICODE
	wcscpy_s(mMessage, BUFFER_SIZE, buf);
#else
	size_t mbcslen;
	wcstombs_s(&mbcslen, mMessage, BUFFER_SIZE, buf, _TRUNCATE);
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Exception::SetMessage(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	SetMessage(format, args);
	va_end(args);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Exception::SetMessage(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	SetMessage(format, args);
	va_end(args);
}


#ifdef LN_WIN32
//=============================================================================
// Win32Exception
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32Exception::Win32Exception(uint32_t/*DWORD*/ dwLastError)
	: m_dwLastErrorCode( dwLastError )
{
	// エラーメッセージ取得
	memset(m_pFormatMessage, 0, sizeof(m_pFormatMessage));
    :: FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, m_dwLastErrorCode,
        0, m_pFormatMessage, sizeof(m_pFormatMessage)/sizeof(m_pFormatMessage[0]), NULL);
}
#endif

} // namespace Lumino
