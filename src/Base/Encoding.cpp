
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
	LN_THROW(srcByteCount >= (size_t)decoder->GetMinByteCount(), ArgumentException);	// バッファのバイト数は、そのバッファのエンコーディングの最低バイト数以上でなければならない

	// src に入っている最悪パターンの文字数
	int srcMaxCharCount = srcByteCount / decoder->GetMinByteCount();

	// 中間バッファに必要な最大バイト数
	int utf16MaxByteCount = srcMaxCharCount * 4;	// UTF16 は1文字最大4バイト

	// 出力バッファに必要な最大バイト数
	int outputMaxByteCount = srcMaxCharCount * encoder->GetMaxByteCount();

	// 中間バッファ作成
	RefPtr<RefBuffer> tmpBuf(LN_NEW RefBuffer());
	tmpBuf->Reserve(utf16MaxByteCount + sizeof(uint16_t));	// 終端 \0 考慮 (mbstowcs_s は \0 を書き込もうとする)

	// 変換先バッファを、最大要素数で確保
	RefPtr<RefBuffer> targetBuf(LN_NEW RefBuffer());
	targetBuf->Reserve(outputMaxByteCount + encoder->GetMaxByteCount());	// 終端 \0 考慮 (mbstowcs_s は \0 を書き込もうとする)

	// 変換実行
	size_t bytesUsed;
	size_t charsUsed;
	// ソースフォーマットから中間フォーマットへ
	decoder->ConvertToUTF16(
		(const byte_t*)src,
		srcByteCount,
		tmpBuf->GetPointer(),
		utf16MaxByteCount,			// \0 強制格納に備え、1文字分余裕のあるサイズを指定する
		&bytesUsed,
		&charsUsed);
	// 中間フォーマットからターゲットフォーマットへ
	encoder->ConvertFromUTF16(
		(const byte_t*)tmpBuf->GetPointer(),
		bytesUsed,
		(byte_t*)targetBuf->GetPointer(),
		targetBuf->GetSize(),		// \0 強制格納に備え、1文字分余裕のあるサイズを指定する
		&bytesUsed,
		&charsUsed);

	if (result)
	{
		result->BytesUsed = bytesUsed;
		result->CharsUsed = charsUsed;
		result->UsedDefaultChar = (decoder->UsedDefaultCharCount() > 0 || encoder->UsedDefaultCharCount() > 0);
	}
	targetBuf->Resize(bytesUsed);	// 出力バッファの見かけ上のサイズを、実際に使用したバイト数にする
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
	// mbstowcs は setlocale() によって動作が変わるが、
	// どんなマルチバイトコードが使われるのか、そのコードの最大バイト数はいくつかは得ることができない。
	// ※ WinAPI では GetCPInfoEx() でコードページごとの最大バイト数を得ることができるが、
	//    setlocale() を使用しても _getmbcp や CP_THREAD_ACP で得られるコードページに変化が無い。
	// 代わりに、最悪のサイズとして UTF-8 の最大サイズを想定する。
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
#ifdef LN_WIN32	/* Windows 環境で setlocale しなくても使えるようにする */
	// 入力が 0 文字の場合は何もしない
	// (MultiByteToWideChar の戻り値がエラーなのか成功なのかわからなくなる)
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

	// mbstowcs じゃ文字数カウントはできないので UnicodeUtils を使う
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)outBuffer, len, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = len * sizeof(wchar_t);
	*outCharsUsed = count;
#else
#ifdef LN_WCHAR_16
	// mbstowcs_s は変換したいサイズを指定することができず、基本的に \0 まで変換することになる。
	// そのため、一度別バッファに移して \0 を付ける
	RefBuffer tmpInBuffer;
	tmpInBuffer.Reserve(inByteCount + sizeof(char));	// NULL 文字分 + 1
	tmpInBuffer.Copy(inBuffer, inByteCount);
	char* tmpStr = (char*)tmpInBuffer.GetPointer();
	tmpStr[inByteCount] = '\0';

	// Multi → Wide
	size_t len;
	errno_t err = mbstowcs_s(
		&len,										// 変換された文字数
		(wchar_t*)outBuffer,						// 出力先バッファ
		(outByteCount / sizeof(wchar_t)) + 1,// 出力先バッファのサイズ (文字数 = wchar_t としての要素数)
		tmpStr,										// 変換元バッファ
		outByteCount / sizeof(wchar_t));		// outBuffer に格納する wchar_t の最大数
	LN_THROW(err == 0, EncodingFallbackException);

	// 終端には \0 が強制的に付加される。純粋な文字部分のサイズが欲しいので -1 する。
	len--;

	// mbstowcs じゃ文字数カウントはできないので UnicodeUtils を使う
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
#ifdef LN_WIN32	/* Windows 環境で setlocale しなくても使えるようにする */
	// 入力が 0 文字の場合は何もしない
	// (MultiByteToWideChar の戻り値がエラーなのか成功なのかわからなくなる)
	if (inByteCount == 0) {
		if (outByteCount > 0) {
			outBuffer[0] = '\0';
		}
		*outBytesUsed = 0;
		*outCharsUsed = 0;
		return;
	}

	// マッピングできない文字をデフォルト文字に変換する設定をいろいろ
	DWORD dwFlags = 0;
	char chDefault = (char)mFallbackReplacementChar;
	BOOL bUsedDefaultChar = FALSE;
	LPCSTR pDefault = NULL;
	if (chDefault != '\0') {
		dwFlags = WC_NO_BEST_FIT_CHARS;
		pDefault = &chDefault;
	}

	// 変換
	int len = ::WideCharToMultiByte(
		CP_THREAD_ACP,
		dwFlags,
		(const wchar_t*)inBuffer,
		inByteCount / sizeof(wchar_t),
		(LPSTR )outBuffer,
		outByteCount,
		pDefault,
		&bUsedDefaultChar);
	LN_THROW(!(chDefault == '\0' && bUsedDefaultChar == TRUE), EncodingFallbackException);	// デフォルト文字未指定でマッピングできない文字があった
	LN_THROW(len > 0, EncodingFallbackException);

	// 文字数カウント
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inByteCount / sizeof(UnicodeUtils::UTF16), true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = len;		// len は「書き込まれたバイト数」なのでこれでOK
	*outCharsUsed = count;
#else
#ifdef LN_WCHAR_16
	// wcsrtombs_s は変換したいサイズを指定することができず、基本的に \0 まで変換することになる。
	// そのため、一度別バッファに移して \0 を付ける
	RefBuffer tmpWideBuffer;
	tmpWideBuffer.Reserve(inByteCount + sizeof(wchar_t));	// NULL 文字分
	tmpWideBuffer.Copy(inBuffer, inByteCount);
	wchar_t* wideStr = (wchar_t*)tmpWideBuffer.GetPointer();
	wideStr[inByteCount / sizeof(wchar_t)] = L'\0';

	// Wide → Multi
#if 1
	size_t convertedLen;
	errno_t err = wcstombs_s(
		&convertedLen,
		(char*)outBuffer,			// 出力先バッファ
		outByteCount,		// 出力先バッファのサイズ (バイト数)
		wideStr,					// 変換元
		_TRUNCATE);					// 出力先バッファに格納できる最大バイト数 (実質↑↑と同じ。_TRUNCATE で \0 まで)
	LN_THROW(err == 0, EncodingFallbackException);	// ここで引っかかる場合は setlocale していないかもしれない
#else
	size_t convertedLen;
	mbstate_t mbstate = 0;
	::memset((void*)&mbstate, 0, sizeof(mbstate));
	errno_t err = wcsrtombs_s(
		&convertedLen,				// 変換された文字数が格納される (Multi への変換なので、バイト数)
		(char*)outBuffer,			// 出力先バッファ
		outByteCount,		// 出力先バッファのサイズ (バイト数)
		(const wchar_t**)&wideStr,	// 変換元
		_TRUNCATE,					// 出力先バッファに格納できる最大バイト数 (実質↑↑と同じ。_TRUNCATE で \0 まで)
		&mbstate);
	LN_THROW(err == 0, ArgumentException);
#endif

	// 終端には \0 が強制的に付加される。純粋な文字部分のサイズが欲しいので -1 する。
	// TODO: つまり、出力バッファは ↑の第2引数 +1ワード分 確保しておく必要がある
	convertedLen--;

	// wcsrtombs_s じゃ文字数カウントはできないので UnicodeUtils を使う
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
	// 変換設定
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;
	
	// 変換
	UTFConversionResult result = UnicodeUtils::ConvertUTF8toUTF16(
		(UnicodeUtils::UTF8*)inBuffer, 
		inByteCount / sizeof(UnicodeUtils::UTF8),
		(UnicodeUtils::UTF16*)outBuffer, 
		outByteCount / sizeof(UnicodeUtils::UTF16),
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	// 出力
	*outBytesUsed = options.ConvertedTargetLength * sizeof(UnicodeUtils::UTF16);
	*outCharsUsed = options.CharCount;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF8Encoding::UTF8Encoder::ConvertFromUTF16(const byte_t* inBuffer, size_t inByteCount, byte_t* outBuffer, size_t outByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	// 変換設定
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;
	
	// 変換
	UTFConversionResult result = UnicodeUtils::ConvertUTF16toUTF8(
		(UnicodeUtils::UTF16*)inBuffer, 
		inByteCount / sizeof(UnicodeUtils::UTF16),
		(UnicodeUtils::UTF8*)outBuffer, 
		outByteCount / sizeof(UnicodeUtils::UTF8),
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	// 出力
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
	// UTF16 から UTF16 への変換。そのままコピーでよい
	errno_t err = memcpy_s(outBuffer, outByteCount, inBuffer, inByteCount);
	LN_THROW(err == 0, ArgumentException);

	// 文字数はカウントする
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
	// UTF16 から UTF16 への変換。そのままコピーでよい
	errno_t err = memcpy_s(outBuffer, outByteCount, inBuffer, inByteCount);
	LN_THROW(err == 0, ArgumentException);

	// 文字数はカウントする
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inByteCount / sizeof(UnicodeUtils::UTF16), true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = inByteCount;
	*outCharsUsed = count;
}

} // namespace Lumino
