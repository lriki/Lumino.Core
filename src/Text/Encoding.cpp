
#include "../Internal.h"
#include "../../include/Lumino/Base/RefObject.h"
#include "../../include/Lumino/Base/RefBuffer.h"
#include "../../include/Lumino/Text/UnicodeUtils.h"
#include "../../include/Lumino/Text/Encoding.h"
#include "ASCIIEncoding.h"
#include "DBCSEncoding.h"
#include "UTF16Encoding.h"
#include "UTF32Encoding.h"

namespace Lumino
{
namespace Text
{

//=============================================================================
// Encoding
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Encoding* Encoding::GetSystemMultiByteEncoding()
{
	static SystemMultiByteEncoding systemEncoding;
	return &systemEncoding;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Encoding* Encoding::GetWideCharEncoding()
{
#if defined(LN_WCHAR_16)
	static UTF16Encoding wideEncoding(false, false);
	return &wideEncoding;
#elif defined(LN_WCHAR_32)
	static UTF32Encoding wideEncoding(false, false);
	return &wideEncoding;
#else
#error "Invalid wchar_t size."
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Encoding* Encoding::GetTCharEncoding()
{
#ifdef LN_UNICODE
	return GetWideCharEncoding();
#else
	return GetSystemMultiByteEncoding();
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Encoding* Encoding::GetUTF8Encoding()
{
	static UTF8Encoding encoding(false);
	return &encoding;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Encoding* Encoding::GetUTF16Encoding()
{
	static UTF16Encoding encoding(false, false);
	return &encoding;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Encoding* Encoding::GetEncoding(EncodingType type)
{
	switch (type)
	{
		case EncodingType_ASCII:
		{
			static ASCIIEncoding asciiEncoding;
			return &asciiEncoding;
		}
		case EncodingType_SJIS:
		{
			static DBCSEncoding sjisEncoding(EncodingType_SJIS);
			return &sjisEncoding;
		}
		case EncodingType_GB2312:
		{
			static DBCSEncoding gb2312Encoding(EncodingType_GB2312);
			return &gb2312Encoding;
		}
		case EncodingType_EUCKR:
		{
			static DBCSEncoding euckrEncoding(EncodingType_EUCKR);
			return &euckrEncoding;
		}
		case EncodingType_BIG5:
		{
			static DBCSEncoding big5Encoding(EncodingType_BIG5);
			return &big5Encoding;
		}
		case EncodingType_UTF8:
		{
			static UTF8Encoding utf8BOMEncoding(true);
			return &utf8BOMEncoding;
		}
	}
	LN_THROW(0, ArgumentException);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<>
Encoding* Encoding::GetEncodingTemplate<char>()
{
	return GetSystemMultiByteEncoding();
}
template<>
Encoding* Encoding::GetEncodingTemplate<wchar_t>()
{
	return GetWideCharEncoding();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
RefBuffer* Encoding::Convert(
	const void* src, size_t srcByteCount, const Encoding* srcEncoding,
	const Encoding* targetEncoding,
	EncodingConversionResult* result)
{
	RefPtr<Decoder> decoder(srcEncoding->CreateDecoder());
	RefPtr<Encoder> encoder(targetEncoding->CreateEncoder());
	return Convert(src, srcByteCount, decoder.GetObjectPtr(), encoder.GetObjectPtr(), result);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
RefBuffer* Encoding::Convert(
	const void* src, size_t srcByteCount, Decoder* decoder,
	Encoder* encoder,
	EncodingConversionResult* result)
{
	LN_THROW(src != NULL, ArgumentException);
	LN_THROW(decoder != NULL, ArgumentException);
	LN_THROW(encoder != NULL, ArgumentException);
	//LN_THROW(srcByteCount >= (size_t)decoder->GetMinByteCount(), ArgumentException);	// �o�b�t�@�̃o�C�g���́A���̃o�b�t�@�̃G���R�[�f�B���O�̍Œ�o�C�g���ȏ�łȂ���΂Ȃ�Ȃ�

	// src �ɓ����Ă���ň��p�^�[���̕�����
	int srcMaxCharCount = srcByteCount / decoder->GetMinByteCount();
	srcMaxCharCount += 1;	// Decoder�EEncoder �̏�ԕۑ��ɂ��O��̂��܂蕶����1�ǉ�����邩������Ȃ�

	// ���ԃo�b�t�@�ɕK�v�ȍő�o�C�g��
	int utf16MaxByteCount = srcMaxCharCount * 4;	// UTF16 ��1�����ő�4�o�C�g

	// �o�̓o�b�t�@�ɕK�v�ȍő�o�C�g��
	int outputMaxByteCount = srcMaxCharCount * encoder->GetMaxByteCount();

	// ���ԃo�b�t�@�쐬
	RefPtr<RefBuffer> tmpBuf(LN_NEW RefBuffer());
	tmpBuf->Reserve(utf16MaxByteCount + sizeof(uint16_t));	// �I�[ \0 �l�� (mbstowcs_s �� \0 �������������Ƃ���)

	// �ϊ���o�b�t�@���A�ő�v�f���Ŋm��
	RefPtr<RefBuffer> targetBuf(LN_NEW RefBuffer());
	targetBuf->Reserve(outputMaxByteCount + encoder->GetMaxByteCount());	// �I�[ \0 �l�� (mbstowcs_s �� \0 �������������Ƃ���)

	// �ϊ����s
	size_t bytesUsed;
	size_t charsUsed;
	// �\�[�X�t�H�[�}�b�g���璆�ԃt�H�[�}�b�g��
	decoder->ConvertToUTF16(
		(const byte_t*)src,
		srcByteCount,
		(UTF16*)tmpBuf->GetPointer(),
		utf16MaxByteCount / sizeof(UTF16),			// \0 �����i�[�ɔ����A1�������]�T�̂���T�C�Y���w�肷��
		&bytesUsed,
		&charsUsed);
	// ���ԃt�H�[�}�b�g����^�[�Q�b�g�t�H�[�}�b�g��
	encoder->ConvertFromUTF16(
		(const UTF16*)tmpBuf->GetPointer(),
		bytesUsed / sizeof(UTF16),
		(byte_t*)targetBuf->GetPointer(),
		targetBuf->GetSize(),		// \0 �����i�[�ɔ����A1�������]�T�̂���T�C�Y���w�肷��
		&bytesUsed,
		&charsUsed);

	if (result)
	{
		result->BytesUsed = bytesUsed;
		result->CharsUsed = charsUsed;
		result->UsedDefaultChar = (decoder->UsedDefaultCharCount() > 0 || encoder->UsedDefaultCharCount() > 0);
	}
	targetBuf->Resize(bytesUsed);	// �o�̓o�b�t�@�̌�������̃T�C�Y���A���ۂɎg�p�����o�C�g���ɂ���
	targetBuf.SafeAddRef();
	return targetBuf;
}

//=============================================================================
// SystemMultiByteEncoding
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int SystemMultiByteEncoding::GetMinByteCount() const
{
	return 1;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int SystemMultiByteEncoding::GetMaxByteCount() const
{
#if 1
	// mbstowcs �� setlocale() �ɂ���ē��삪�ς�邪�A
	// �ǂ�ȃ}���`�o�C�g�R�[�h���g����̂��A���̃R�[�h�̍ő�o�C�g���͂������͓��邱�Ƃ��ł��Ȃ��B
	// �� WinAPI �ł� GetCPInfoEx() �ŃR�[�h�y�[�W���Ƃ̍ő�o�C�g���𓾂邱�Ƃ��ł��邪�A
	//    setlocale() ���g�p���Ă� _getmbcp �� CP_THREAD_ACP �œ�����R�[�h�y�[�W�ɕω��������B
	// ����ɁA�ň��̃T�C�Y�Ƃ��� UTF-8 �̍ő�T�C�Y��z�肷��B
	return 6;
#else
#ifdef LN_WIN32
	CPINFOEX info;
	if (!::GetCPInfoEx(CP_THREAD_ACP, 0, &info)) {
		LN_THROW(0, Win32Exception, ::GetLastError());
	}
	return info.MaxCharSize;
#else
	return 6;	// UTF-8
#endif
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void SystemMultiByteEncoding::SystemMultiByteDecoder::ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
#ifdef LN_WIN32	/* Windows ���� setlocale ���Ȃ��Ă��g����悤�ɂ��� */
	// ���͂� 0 �����̏ꍇ�͉������Ȃ�
	// (MultiByteToWideChar �̖߂�l���G���[�Ȃ̂������Ȃ̂��킩��Ȃ��Ȃ�)
	if (inBufferByteCount == 0) {
		if (outBufferCharCount > 0) {
			outBuffer[0] = '\0';
		}
		*outBytesUsed = 0;
		*outCharsUsed = 0;
		return;
	}

	int len = ::MultiByteToWideChar(
		CP_THREAD_ACP, MB_ERR_INVALID_CHARS, 
		(LPCSTR)inBuffer, 
		inBufferByteCount,		// lpMultiByteStr ���w��������̃T�C�Y���o�C�g�P�ʂœn���܂��B
		(LPWSTR)outBuffer, 
		outBufferCharCount);	// lpWideCharStr ���w���o�b�t�@�̃T�C�Y�����C�h�������̒P�ʂŎw�肵�܂��B
	LN_THROW(len > 0, EncodingFallbackException);

	// mbstowcs ���ᕶ�����J�E���g�͂ł��Ȃ��̂� UnicodeUtils ���g��
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)outBuffer, len, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = len * sizeof(wchar_t);
	*outCharsUsed = count;
#else

#ifdef LN_WCHAR_16
	LN_THROW(0, NotImplementedException);
#else
	// �ꎞ�������m�� (char[] �� UTF-8 �ōň��̃p�^�[���́A���ׂ�ASCII�̏ꍇ)
	size_t tmpUTF32BufferLen = inBufferByteCount * sizeof(UnicodeUtils::UTF32);
	RefBuffer tmpUTF32Buffer;
	tmpUTF32Buffer.Reserve(inBufferByteCount);

	// wchar_t (UTF-32) �֕ϊ�����
	const char* str_ptr = (const char*)inBuffer;
	mbstate_t state;
	memset(&state, 0, sizeof(state));
	size_t len = mbsrtowcs((wchar_t*)tmpUTF32Buffer.GetPointer(), &str_ptr, tmpUTF32Buffer.GetSize(), &state);

	// UTF-32 ���� UTF-16 �֕ϊ�����
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;
	UTFConversionResult result = UnicodeUtils::ConvertUTF32toUTF16(
		(UnicodeUtils::UTF32*)tmpUTF32Buffer.GetPointer(),
		len,
		outBuffer,
		outBufferCharCount,
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = options.ConvertedTargetLength * sizeof(UnicodeUtils::UTF16);
	*outCharsUsed = options.CharCount;
#endif

#endif


	/*
	
	// mbstowcs_s �͕ϊ��������T�C�Y���w�肷�邱�Ƃ��ł����A��{�I�� \0 �܂ŕϊ����邱�ƂɂȂ�B
	// ���̂��߁A��x�ʃo�b�t�@�Ɉڂ��� \0 ��t����
	RefBuffer tmpInBuffer;
	tmpInBuffer.Reserve(inByteCount + sizeof(char));	// NULL ������ + 1
	tmpInBuffer.Copy(inBuffer, inByteCount);
	char* tmpStr = (char*)tmpInBuffer.GetPointer();
	tmpStr[inByteCount] = '\0';

	// Multi �� Wide
	size_t len;
	errno_t err = mbstowcs_s(
		&len,										// �ϊ����ꂽ������
		(wchar_t*)outBuffer,						// �o�͐�o�b�t�@
		(outByteCount / sizeof(wchar_t)) + 1,// �o�͐�o�b�t�@�̃T�C�Y (������ = wchar_t �Ƃ��Ă̗v�f��)
		tmpStr,										// �ϊ����o�b�t�@
		outByteCount / sizeof(wchar_t));		// outBuffer �Ɋi�[���� wchar_t �̍ő吔
	LN_THROW(err == 0, EncodingFallbackException);

	// �I�[�ɂ� \0 �������I�ɕt�������B�����ȕ��������̃T�C�Y���~�����̂� -1 ����B
	len--;

	// mbstowcs ���ᕶ�����J�E���g�͂ł��Ȃ��̂� UnicodeUtils ���g��
	int count;
	UTFConversionResult_t r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)outBuffer, len, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = len * sizeof(wchar_t);
	*outCharsUsed = count;
	*/
	/*

	#ifdef LN_WCHAR_16
	// mbstowcs_s �͕ϊ��������T�C�Y���w�肷�邱�Ƃ��ł����A��{�I�� \0 �܂ŕϊ����邱�ƂɂȂ�B
	// ���̂��߁A��x�ʃo�b�t�@�Ɉڂ��� \0 ��t����
	RefBuffer tmpInBuffer;
	tmpInBuffer.Reserve(inByteCount + sizeof(char));	// NULL ������ + 1
	tmpInBuffer.Copy(inBuffer, inByteCount);
	char* tmpStr = (char*)tmpInBuffer.GetPointer();
	tmpStr[inByteCount] = '\0';

	// Multi �� Wide
	size_t len;
	errno_t err = mbstowcs_s(
	&len,										// �ϊ����ꂽ������
	(wchar_t*)outBuffer,						// �o�͐�o�b�t�@
	(outByteCount / sizeof(wchar_t)) + 1,// �o�͐�o�b�t�@�̃T�C�Y (������ = wchar_t �Ƃ��Ă̗v�f��)
	tmpStr,										// �ϊ����o�b�t�@
	outByteCount / sizeof(wchar_t));		// outBuffer �Ɋi�[���� wchar_t �̍ő吔
	LN_THROW(err == 0, EncodingFallbackException);

	// �I�[�ɂ� \0 �������I�ɕt�������B�����ȕ��������̃T�C�Y���~�����̂� -1 ����B
	len--;

	// mbstowcs ���ᕶ�����J�E���g�͂ł��Ȃ��̂� UnicodeUtils ���g��
	int count;
	UTFConversionResult_t r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)outBuffer, len, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = len * sizeof(wchar_t);
	*outCharsUsed = count;
	#else
	LN_THROW(0, NotImplementedException);
	#endif
	*/
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void SystemMultiByteEncoding::SystemMultiByteEncoder::ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
#ifdef LN_WIN32	/* Windows ���� setlocale ���Ȃ��Ă��g����悤�ɂ��� */
	// ���͂� 0 �����̏ꍇ�͉������Ȃ�
	// (MultiByteToWideChar �̖߂�l���G���[�Ȃ̂������Ȃ̂��킩��Ȃ��Ȃ�)
	if (inBufferCharCount == 0) {
		if (outBufferByteCount > 0) {
			outBuffer[0] = '\0';
		}
		*outBytesUsed = 0;
		*outCharsUsed = 0;
		return;
	}

	// �}�b�s���O�ł��Ȃ��������f�t�H���g�����ɕϊ�����ݒ�����낢��
	DWORD dwFlags = 0;
	char chDefault = (char)mFallbackReplacementChar;
	BOOL bUsedDefaultChar = FALSE;
	LPCSTR pDefault = NULL;
	if (chDefault != '\0') {
		dwFlags = WC_NO_BEST_FIT_CHARS;
		pDefault = &chDefault;
	}

	// �ϊ�
	int len = ::WideCharToMultiByte(
		CP_THREAD_ACP,
		dwFlags,
		(const wchar_t*)inBuffer,
		inBufferCharCount,		// ���C�h������̕�����
		(LPSTR )outBuffer,
		outBufferByteCount,		// �V������������󂯎��o�b�t�@�̃T�C�Y
		pDefault,
		&bUsedDefaultChar);
	LN_THROW(!(chDefault == '\0' && bUsedDefaultChar == TRUE), EncodingFallbackException);	// �f�t�H���g�������w��Ń}�b�s���O�ł��Ȃ�������������
	LN_THROW(len > 0, EncodingFallbackException);

	// �������J�E���g
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inBufferCharCount, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = len;		// len �́u�������܂ꂽ�o�C�g���v�Ȃ̂ł����OK
	*outCharsUsed = count;
#else
#ifdef LN_WCHAR_16
	LN_THROW(0, NotImplementedException);
#else
	// UTF-16 �̃T���Q�[�g���l�����A�ň��p�^�[��(���ׂăT���Q�[�g)�Ń������m��
	RefBuffer tmpUTF32Buffer;
	tmpUTF32Buffer.Reserve(sizeof(wchar_t) * (inBufferCharCount * 2));

	// UTF-32 �֕ϊ�����
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;
	UTFConversionResult result = UnicodeUtils::ConvertUTF16toUTF32(
		inBuffer,
		inBufferCharCount,
		(UnicodeUtils::UTF32*)tmpUTF32Buffer.GetPointer(),
		tmpUTF32Buffer.GetSize(),
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	// UTF-32 �� char[] �֕ϊ����� (wcsrtombs() �͏o�̓o�b�t�@�ɂ��܂肪����Ƃ��� '\0' �����邪�A��t�̎��͂��Ȃ�)
	const wchar_t* wstr_ptr = (const wchar_t*)tmpUTF32Buffer.GetPointer();
	mbstate_t state;
	memset(&state, 0, sizeof(state));
	size_t t = wcsrtombs((char*)outBuffer, &wstr_ptr, outBufferByteCount, &state);

	*outBytesUsed = t;//options.ConvertedTargetLength * sizeof(UnicodeUtils::UTF32);
	*outCharsUsed = options.CharCount;
#endif

#endif

	/*
	
#if 0
#ifdef LN_WCHAR_16
	// wcsrtombs_s �͕ϊ��������T�C�Y���w�肷�邱�Ƃ��ł����A��{�I�� \0 �܂ŕϊ����邱�ƂɂȂ�B
	// ���̂��߁A��x�ʃo�b�t�@�Ɉڂ��� \0 ��t����
	RefBuffer tmpWideBuffer;
	tmpWideBuffer.Reserve(inByteCount + sizeof(wchar_t));	// NULL ������
	tmpWideBuffer.Copy(inBuffer, inByteCount);
	wchar_t* wideStr = (wchar_t*)tmpWideBuffer.GetPointer();
	wideStr[inByteCount / sizeof(wchar_t)] = L'\0';

	// Wide �� Multi
#if 1
	size_t convertedLen;
	errno_t err = wcstombs_s(
		&convertedLen,
		(char*)outBuffer,			// �o�͐�o�b�t�@
		outByteCount,		// �o�͐�o�b�t�@�̃T�C�Y (�o�C�g��)
		wideStr,					// �ϊ���
		_TRUNCATE);					// �o�͐�o�b�t�@�Ɋi�[�ł���ő�o�C�g�� (���������Ɠ����B_TRUNCATE �� \0 �܂�)
	LN_THROW(err == 0, EncodingFallbackException);	// �����ň���������ꍇ�� setlocale ���Ă��Ȃ���������Ȃ�
#else
	size_t convertedLen;
	mbstate_t mbstate = 0;
	::memset((void*)&mbstate, 0, sizeof(mbstate));
	errno_t err = wcsrtombs_s(
		&convertedLen,				// �ϊ����ꂽ���������i�[����� (Multi �ւ̕ϊ��Ȃ̂ŁA�o�C�g��)
		(char*)outBuffer,			// �o�͐�o�b�t�@
		outByteCount,		// �o�͐�o�b�t�@�̃T�C�Y (�o�C�g��)
		(const wchar_t**)&wideStr,	// �ϊ���
		_TRUNCATE,					// �o�͐�o�b�t�@�Ɋi�[�ł���ő�o�C�g�� (���������Ɠ����B_TRUNCATE �� \0 �܂�)
		&mbstate);
	LN_THROW(err == 0, ArgumentException);
#endif

	// �I�[�ɂ� \0 �������I�ɕt�������B�����ȕ��������̃T�C�Y���~�����̂� -1 ����B
	// TODO: �܂�A�o�̓o�b�t�@�� ���̑�2���� +1���[�h�� �m�ۂ��Ă����K�v������
	convertedLen--;

	// wcsrtombs_s ���ᕶ�����J�E���g�͂ł��Ȃ��̂� UnicodeUtils ���g��
	int count;
	UTFConversionResult_t r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inByteCount / sizeof(UnicodeUtils::UTF16), true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = convertedLen;
	*outCharsUsed = count;
#else
	LN_THROW(0, NotImplementedException);
#endif
#endif
	*/
}

//=============================================================================
// UTF8Encoding
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
UTF8Encoding::UTF8Encoding(bool byteOrderMark)
	: m_byteOrderMark(byteOrderMark)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
byte_t* UTF8Encoding::GetPreamble() const
{
	static byte_t bom[] = { 0xEF, 0xBB, 0xBF, NULL };
	return bom;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF8Encoding::UTF8Decoder::ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	// �ϊ��ݒ�
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;

	// BOM �t���̏ꍇ�͎�菜�� (�o�b�t�@�k��)
	if (m_byteOrderMark) {
		static byte_t bom[] = { 0xEF, 0xBB, 0xBF };
		int r = memcmp(inBuffer, bom, 3);
		LN_THROW(r == 0, EncodingFallbackException);
		inBuffer += 3;
		inBufferByteCount -= 3;
	}
	
	// �ϊ�
	UTFConversionResult result = UnicodeUtils::ConvertUTF8toUTF16(
		(UnicodeUtils::UTF8*)inBuffer, 
		inBufferByteCount,
		(UnicodeUtils::UTF16*)outBuffer, 
		outBufferCharCount,
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	// �o��
	*outBytesUsed = options.ConvertedTargetLength * sizeof(UnicodeUtils::UTF16);
	*outCharsUsed = options.CharCount;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF8Encoding::UTF8Encoder::ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	// �ϊ��ݒ�
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;
	
	// �ϊ�
	UTFConversionResult result = UnicodeUtils::ConvertUTF16toUTF8(
		(UnicodeUtils::UTF16*)inBuffer, 
		inBufferCharCount,
		(UnicodeUtils::UTF8*)outBuffer, 
		outBufferByteCount,
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	// �o��
	*outBytesUsed = options.ConvertedTargetLength;
	*outCharsUsed = options.CharCount;
}


#if 0
//=============================================================================
// UTF16Encoding
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
byte_t* UTF16Encoding::GetPreamble() const
{
	// UTF-16 little endian byte order
	static byte_t bom[] = { 0xFF, 0xFE, NULL };
	return bom;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF16Encoding::UTF16Decoder::ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	// UTF16 ���� UTF16 �ւ̕ϊ��B���̂܂܃R�s�[�ł悢
	errno_t err = memcpy_s(outBuffer, outBufferCharCount * sizeof(UTF16), inBuffer, inBufferByteCount);
	LN_THROW(err == 0, ArgumentException);

	// �������̓J�E���g����
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inBufferByteCount / sizeof(UnicodeUtils::UTF16), true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = inBufferByteCount;
	*outCharsUsed = count;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF16Encoding::UTF16Encoder::ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	// UTF16 ���� UTF16 �ւ̕ϊ��B���̂܂܃R�s�[�ł悢
	errno_t err = memcpy_s(outBuffer, outBufferByteCount, inBuffer, inBufferCharCount * sizeof(UTF16));
	LN_THROW(err == 0, ArgumentException);

	// �������̓J�E���g����
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inBufferCharCount, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = inBufferCharCount * sizeof(UTF16);
	*outCharsUsed = count;
}
#endif

} // namespace Text
} // namespace Lumino
