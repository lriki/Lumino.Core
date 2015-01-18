
#include "../Internal.h"
#include "../../include/Lumino/Base/RefObject.h"
#include "../../include/Lumino/Base/RefBuffer.h"
#include "../../include/Lumino/Base/UnicodeUtils.h"
#include "../../include/Lumino/Base/Encoding.h"

namespace Lumino
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
	static UTF16Encoding wideEncoding;
	return &wideEncoding;
#elif defined(LN_WCHAR_32)
	static UTF32Encoding wideEncoding;
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
	static UTF8Encoding encoding;
	return &encoding;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Encoding* Encoding::GetUTF16Encoding()
{
	static UTF16Encoding encoding;
	return &encoding;
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
	return Convert(src, srcByteCount, decoder.GetPtr(), encoder.GetPtr(), result);
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
	LN_THROW(srcByteCount >= (size_t)decoder->GetMinByteCount(), ArgumentException);	// �o�b�t�@�̃o�C�g���́A���̃o�b�t�@�̃G���R�[�f�B���O�̍Œ�o�C�g���ȏ�łȂ���΂Ȃ�Ȃ�

	// src �ɓ����Ă���ň��p�^�[���̕�����
	int srcMaxCharCount = srcByteCount / decoder->GetMinByteCount();

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
		tmpBuf->GetPointer(),
		utf16MaxByteCount,			// \0 �����i�[�ɔ����A1�������]�T�̂���T�C�Y���w�肷��
		&bytesUsed,
		&charsUsed);
	// ���ԃt�H�[�}�b�g����^�[�Q�b�g�t�H�[�}�b�g��
	encoder->ConvertFromUTF16(
		(const byte_t*)tmpBuf->GetPointer(),
		bytesUsed,
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
void SystemMultiByteEncoding::SystemMultiByteDecoder::ConvertToUTF16(const byte_t* inBuffer, size_t inByteCount, byte_t* outBuffer, size_t outByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
#ifdef LN_WIN32	/* Windows ���� setlocale ���Ȃ��Ă��g����悤�ɂ��� */
	// ���͂� 0 �����̏ꍇ�͉������Ȃ�
	// (MultiByteToWideChar �̖߂�l���G���[�Ȃ̂������Ȃ̂��킩��Ȃ��Ȃ�)
	if (inByteCount == 0) {
		if (outByteCount > 0) {
			outBuffer[0] = '\0';
		}
		*outBytesUsed = 0;
		*outCharsUsed = 0;
		return;
	}

	int len = ::MultiByteToWideChar(CP_THREAD_ACP, MB_ERR_INVALID_CHARS, (LPCSTR)inBuffer, inByteCount, (LPWSTR)outBuffer, outByteCount);
	LN_THROW(len > 0, EncodingFallbackException);

	// mbstowcs ���ᕶ�����J�E���g�͂ł��Ȃ��̂� UnicodeUtils ���g��
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)outBuffer, len, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = len * sizeof(wchar_t);
	*outCharsUsed = count;
#else
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
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void SystemMultiByteEncoding::SystemMultiByteEncoder::ConvertFromUTF16(const byte_t* inBuffer, size_t inByteCount, byte_t* outBuffer, size_t outByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
#ifdef LN_WIN32	/* Windows ���� setlocale ���Ȃ��Ă��g����悤�ɂ��� */
	// ���͂� 0 �����̏ꍇ�͉������Ȃ�
	// (MultiByteToWideChar �̖߂�l���G���[�Ȃ̂������Ȃ̂��킩��Ȃ��Ȃ�)
	if (inByteCount == 0) {
		if (outByteCount > 0) {
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
		inByteCount / sizeof(wchar_t),
		(LPSTR )outBuffer,
		outByteCount,
		pDefault,
		&bUsedDefaultChar);
	LN_THROW(!(chDefault == '\0' && bUsedDefaultChar == TRUE), EncodingFallbackException);	// �f�t�H���g�������w��Ń}�b�s���O�ł��Ȃ�������������
	LN_THROW(len > 0, EncodingFallbackException);

	// �������J�E���g
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inByteCount / sizeof(UnicodeUtils::UTF16), true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = len;		// len �́u�������܂ꂽ�o�C�g���v�Ȃ̂ł����OK
	*outCharsUsed = count;
#else
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
}

//=============================================================================
// UTF8Encoding
//=============================================================================

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
void UTF8Encoding::UTF8Decoder::ConvertToUTF16(const byte_t* inBuffer, size_t inByteCount, byte_t* outBuffer, size_t outByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	// �ϊ��ݒ�
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;
	
	// �ϊ�
	UTFConversionResult result = UnicodeUtils::ConvertUTF8toUTF16(
		(UnicodeUtils::UTF8*)inBuffer, 
		inByteCount / sizeof(UnicodeUtils::UTF8),
		(UnicodeUtils::UTF16*)outBuffer, 
		outByteCount / sizeof(UnicodeUtils::UTF16),
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	// �o��
	*outBytesUsed = options.ConvertedTargetLength * sizeof(UnicodeUtils::UTF16);
	*outCharsUsed = options.CharCount;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF8Encoding::UTF8Encoder::ConvertFromUTF16(const byte_t* inBuffer, size_t inByteCount, byte_t* outBuffer, size_t outByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	// �ϊ��ݒ�
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;
	
	// �ϊ�
	UTFConversionResult result = UnicodeUtils::ConvertUTF16toUTF8(
		(UnicodeUtils::UTF16*)inBuffer, 
		inByteCount / sizeof(UnicodeUtils::UTF16),
		(UnicodeUtils::UTF8*)outBuffer, 
		outByteCount / sizeof(UnicodeUtils::UTF8),
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	// �o��
	*outBytesUsed = options.ConvertedTargetLength;
	*outCharsUsed = options.CharCount;
}


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
void UTF16Encoding::UTF16Decoder::ConvertToUTF16(const byte_t* inBuffer, size_t inByteCount, byte_t* outBuffer, size_t outByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	// UTF16 ���� UTF16 �ւ̕ϊ��B���̂܂܃R�s�[�ł悢
	errno_t err = memcpy_s(outBuffer, outByteCount, inBuffer, inByteCount);
	LN_THROW(err == 0, ArgumentException);

	// �������̓J�E���g����
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inByteCount / sizeof(UnicodeUtils::UTF16), true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = inByteCount;
	*outCharsUsed = count;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF16Encoding::UTF16Encoder::ConvertFromUTF16(const byte_t* inBuffer, size_t inByteCount, byte_t* outBuffer, size_t outByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	// UTF16 ���� UTF16 �ւ̕ϊ��B���̂܂܃R�s�[�ł悢
	errno_t err = memcpy_s(outBuffer, outByteCount, inBuffer, inByteCount);
	LN_THROW(err == 0, ArgumentException);

	// �������̓J�E���g����
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inByteCount / sizeof(UnicodeUtils::UTF16), true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = inByteCount;
	*outCharsUsed = count;
}

} // namespace Lumino
