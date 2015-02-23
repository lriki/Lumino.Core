
#include <time.h>
#include <algorithm>
#include "../Internal.h"
#include "../../include/Lumino/Base/CRT.h"
#include "../../include/Lumino/Base/Exception.h"
#include "../../include/Lumino/Base/StringUtils.h"

#ifdef LN_EXCEPTION_BACKTRACE
	#ifdef LN_WIN32	// Cygwin ��������
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
	// �o�b�N�g���[�X�L�^
	mStackBufferSize = BackTrace::GetInstance()->Backtrace(mStackBuffer, LN_ARRAY_SIZE_OF(mStackBuffer));

	// �o�b�N�g���[�X������擾
	BackTrace::GetInstance()->AddressToFullSymbolString(
		mStackBuffer, 
		std::min(mStackBufferSize, 32),
		mSymbolBuffer, 
		LN_ARRAY_SIZE_OF(mSymbolBuffer));
#endif

	// �t�@�C���ɕۑ�
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
	// �����o�b�N�g���[�X�����Ă��Ȃ������炻��p�̕�����o�b�t�@�ɓ���Ă��܂�
	if (mSymbolBuffer[0] == 0x00)
	{
		sprintf_s(mSymbolBuffer, LN_ARRAY_SIZE_OF(mSymbolBuffer), "File:%s Line:%d", filePath, fileLine);
	}

	// ���C�h������֕ϊ� (�����R�[�h�͍l�����Ȃ�)
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
	// �t�@�C���p�X�ێ�
	memset(gDumpFilePath, 0, sizeof(gDumpFilePath));
	strcpy_s(gDumpFilePath, LN_MAX_PATH, filePath);

	// �t�@�C��������
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
		// �o�b�t�@�Ɏ��܂肫��Ȃ��ꍇ�͏I�[�� ... �ɂ��Đ؂�
		buf[BUFFER_SIZE - 4] = '.';
		buf[BUFFER_SIZE - 3] = '.';
		buf[BUFFER_SIZE - 2] = '.';
		buf[BUFFER_SIZE - 1] = '\0';
	}

	// TCHAR �ɍ��킹�ă����o�Ɋi�[
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
		// �o�b�t�@�Ɏ��܂肫��Ȃ��ꍇ�͏I�[�� ... �ɂ��Đ؂�
		buf[BUFFER_SIZE - 4] = L'.';
		buf[BUFFER_SIZE - 3] = L'.';
		buf[BUFFER_SIZE - 2] = L'.';
		buf[BUFFER_SIZE - 1] = L'\0';
	}

	// TCHAR �ɍ��킹�ă����o�Ɋi�[
#ifdef LN_UNICODE
	wcscpy_s(mMessage, BUFFER_SIZE, buf);
#else
	size_t mbcslen;
	wcstombs_s(&mbcslen, mMessage, BUFFER_SIZE, buf, _TRUNCATE);
#endif
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
	// �G���[���b�Z�[�W�擾

	memset(m_pFormatMessage, 0, sizeof(m_pFormatMessage));
    :: FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, m_dwLastErrorCode,
        0, m_pFormatMessage, sizeof(m_pFormatMessage)/sizeof(m_pFormatMessage[0]), NULL);
}
#endif

} // namespace Lumino
