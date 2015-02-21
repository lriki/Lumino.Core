
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
	//LN_THROW(srcByteCount >= (size_t)decoder->GetMinByteCount(), ArgumentException);	// バッファのバイト数は、そのバッファのエンコーディングの最低バイト数以上でなければならない

	// src に入っている最悪パターンの文字数
	int srcMaxCharCount = srcByteCount / decoder->GetMinByteCount();
	srcMaxCharCount += 1;	// Decoder・Encoder の状態保存により前回のあまり文字が1つ追加されるかもしれない

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
		(UTF16*)tmpBuf->GetPointer(),
		utf16MaxByteCount / sizeof(UTF16),			// \0 強制格納に備え、1文字分余裕のあるサイズを指定する
		&bytesUsed,
		&charsUsed);
	// 中間フォーマットからターゲットフォーマットへ
	encoder->ConvertFromUTF16(
		(const UTF16*)tmpBuf->GetPointer(),
		bytesUsed / sizeof(UTF16),
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
void SystemMultiByteEncoding::SystemMultiByteDecoder::ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
#ifdef LN_WIN32	/* Windows 環境で setlocale しなくても使えるようにする */
	// 入力が 0 文字の場合は何もしない
	// (MultiByteToWideChar の戻り値がエラーなのか成功なのかわからなくなる)
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
		inBufferByteCount,		// lpMultiByteStr が指す文字列のサイズをバイト単位で渡します。
		(LPWSTR)outBuffer, 
		outBufferCharCount);	// lpWideCharStr が指すバッファのサイズをワイド文字数の単位で指定します。
	LN_THROW(len > 0, EncodingFallbackException);

	// mbstowcs じゃ文字数カウントはできないので UnicodeUtils を使う
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)outBuffer, len, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = len * sizeof(wchar_t);
	*outCharsUsed = count;
#else

#ifdef LN_WCHAR_16
	LN_THROW(0, NotImplementedException);
#else
	// 一時メモリ確保 (char[] → UTF-8 で最悪のパターンは、すべてASCIIの場合)
	size_t tmpUTF32BufferLen = inBufferByteCount * sizeof(UnicodeUtils::UTF32);
	RefBuffer tmpUTF32Buffer;
	tmpUTF32Buffer.Reserve(inBufferByteCount);

	// wchar_t (UTF-32) へ変換する
	const char* str_ptr = (const char*)inBuffer;
	mbstate_t state;
	memset(&state, 0, sizeof(state));
	size_t len = mbsrtowcs((wchar_t*)tmpUTF32Buffer.GetPointer(), &str_ptr, tmpUTF32Buffer.GetSize(), &state);

	// UTF-32 から UTF-16 へ変換する
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
	*/
	/*

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
	*/
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void SystemMultiByteEncoding::SystemMultiByteEncoder::ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
#ifdef LN_WIN32	/* Windows 環境で setlocale しなくても使えるようにする */
	// 入力が 0 文字の場合は何もしない
	// (MultiByteToWideChar の戻り値がエラーなのか成功なのかわからなくなる)
	if (inBufferCharCount == 0) {
		if (outBufferByteCount > 0) {
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
		inBufferCharCount,		// ワイド文字列の文字数
		(LPSTR )outBuffer,
		outBufferByteCount,		// 新しい文字列を受け取るバッファのサイズ
		pDefault,
		&bUsedDefaultChar);
	LN_THROW(!(chDefault == '\0' && bUsedDefaultChar == TRUE), EncodingFallbackException);	// デフォルト文字未指定でマッピングできない文字があった
	LN_THROW(len > 0, EncodingFallbackException);

	// 文字数カウント
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inBufferCharCount, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = len;		// len は「書き込まれたバイト数」なのでこれでOK
	*outCharsUsed = count;
#else
#ifdef LN_WCHAR_16
	LN_THROW(0, NotImplementedException);
#else
	// UTF-16 のサロゲートを考慮し、最悪パターン(すべてサロゲート)でメモリ確保
	RefBuffer tmpUTF32Buffer;
	tmpUTF32Buffer.Reserve(sizeof(wchar_t) * (inBufferCharCount * 2));

	// UTF-32 へ変換する
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

	// UTF-32 を char[] へ変換する (wcsrtombs() は出力バッファにあまりがあるときは '\0' をつけるが、一杯の時はつけない)
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
	// 変換設定
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;

	// BOM 付きの場合は取り除く (バッファ縮小)
	if (m_byteOrderMark) {
		static byte_t bom[] = { 0xEF, 0xBB, 0xBF };
		int r = memcmp(inBuffer, bom, 3);
		LN_THROW(r == 0, EncodingFallbackException);
		inBuffer += 3;
		inBufferByteCount -= 3;
	}
	
	// 変換
	UTFConversionResult result = UnicodeUtils::ConvertUTF8toUTF16(
		(UnicodeUtils::UTF8*)inBuffer, 
		inBufferByteCount,
		(UnicodeUtils::UTF16*)outBuffer, 
		outBufferCharCount,
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	// 出力
	*outBytesUsed = options.ConvertedTargetLength * sizeof(UnicodeUtils::UTF16);
	*outCharsUsed = options.CharCount;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF8Encoding::UTF8Encoder::ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	// 変換設定
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;
	
	// 変換
	UTFConversionResult result = UnicodeUtils::ConvertUTF16toUTF8(
		(UnicodeUtils::UTF16*)inBuffer, 
		inBufferCharCount,
		(UnicodeUtils::UTF8*)outBuffer, 
		outBufferByteCount,
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	// 出力
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
	// UTF16 から UTF16 への変換。そのままコピーでよい
	errno_t err = memcpy_s(outBuffer, outBufferCharCount * sizeof(UTF16), inBuffer, inBufferByteCount);
	LN_THROW(err == 0, ArgumentException);

	// 文字数はカウントする
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
	// UTF16 から UTF16 への変換。そのままコピーでよい
	errno_t err = memcpy_s(outBuffer, outBufferByteCount, inBuffer, inBufferCharCount * sizeof(UTF16));
	LN_THROW(err == 0, ArgumentException);

	// 文字数はカウントする
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inBufferCharCount, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = inBufferCharCount * sizeof(UTF16);
	*outCharsUsed = count;
}
#endif

} // namespace Text
} // namespace Lumino
