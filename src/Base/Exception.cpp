
#include <time.h>
#include "../Internal.h"
#include <Base/CRT.h>
#include <Base/Exception.h>

#if defined(LN_WIN32)	// Cygwin もこっち
	#include "Win32/BackTrace.h"
#else
	#include "Unix/BackTrace.h"
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

	// バックトレース記録
	mStackBufferSize = BackTrace::GetInstance()->Backtrace(mStackBuffer, LN_ARRAY_SIZE_OF(mStackBuffer));

	// バックトレース文字列取得
	BackTrace::GetInstance()->AddressToFullSymbolString(mStackBuffer, mStackBufferSize, mSymbolBuffer, LN_ARRAY_SIZE_OF(mSymbolBuffer));

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


#ifdef LN_WIN32
//=============================================================================
// Win32Exception
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32Exception::Win32Exception( DWORD dwLastError )
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
