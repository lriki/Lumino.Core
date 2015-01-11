
#include <time.h>
#include "../Internal.h"
#include <Base/CRT.h>
#include <Base/Exception.h>

#if defined(LN_WIN32)	// Cygwin ��������
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

	// �o�b�N�g���[�X�L�^
	mStackBufferSize = BackTrace::GetInstance()->Backtrace(mStackBuffer, LN_ARRAY_SIZE_OF(mStackBuffer));

	// �o�b�N�g���[�X������擾
	BackTrace::GetInstance()->AddressToFullSymbolString(mStackBuffer, mStackBufferSize, mSymbolBuffer, LN_ARRAY_SIZE_OF(mSymbolBuffer));

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
