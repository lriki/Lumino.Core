
#include "../Internal.h"
#include <memory>
#include <algorithm>
#include "../../include/Lumino/Base/RefObject.h"
#include "../../include/Lumino/Base/ByteBuffer.h"
#include "../../include/Lumino/Text/UnicodeUtils.h"
#include "../../include/Lumino/Text/Encoding.h"
#include "ASCIIEncoding.h"
#include "UTF8Encoding.h"
#include "UTF16Encoding.h"
#include "UTF32Encoding.h"
#include "DBCSEncoding.h"
#include "EUCJPEncoding.h"
#ifdef LN_OS_WIN32
#include "Win32CodePageEncoding.h"
#endif

LN_NAMESPACE_BEGIN

//=============================================================================
// Encoding
//=============================================================================

//static const size_t CommonMaxBytes = 6;	///< 全Encoding中、最大の文字バイト数 (UTF8)

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Encoding* Encoding::GetSystemMultiByteEncoding()
{
#ifdef LN_OS_WIN32
	static Win32CodePageEncoding systemEncoding(CP_THREAD_ACP);
	return &systemEncoding;
#else
	static UTF8Encoding systemEncoding(false);
	return &systemEncoding;
#endif
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
Encoding* Encoding::GetUTF32Encoding()
{
	static UTF32Encoding encoding(false, false);
	return &encoding;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Encoding* Encoding::GetEncoding(EncodingType type)
{
	switch (type)
	{
		case EncodingType::ASCII:
		{
			static ASCIIEncoding asciiEncoding;
			return &asciiEncoding;
		}
		case EncodingType::SJIS:
		{
			static DBCSEncoding sjisEncoding(EncodingType::SJIS);
			return &sjisEncoding;
		}
		case EncodingType::EUCJP:
		{
			static EUCJPEncoding eucjpEncoding;
			return &eucjpEncoding;
		}
		case EncodingType::GB2312:
		{
			static DBCSEncoding gb2312Encoding(EncodingType::GB2312);
			return &gb2312Encoding;
		}
		case EncodingType::EUCKR:
		{
			static DBCSEncoding euckrEncoding(EncodingType::EUCKR);
			return &euckrEncoding;
		}
		case EncodingType::BIG5:
		{
			static DBCSEncoding big5Encoding(EncodingType::BIG5);
			return &big5Encoding;
		}
		case EncodingType::UTF8:
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
#ifdef LN_OS_WIN32
Encoding* Encoding::GetWin32DefaultCodePageEncoding()
{
	static Win32CodePageEncoding encoding(CP_THREAD_ACP);
	return &encoding;
}
#endif

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
size_t Encoding::GetConversionRequiredByteCount(Encoding* from, Encoding* to, size_t fromByteCount)
{
	LN_THROW(from != NULL, ArgumentException);
	LN_THROW(to != NULL, ArgumentException);

	// from に入っている最悪パターンの文字数
	size_t srcMaxCharCount = fromByteCount / from->GetMinByteCount();
	srcMaxCharCount += 1;	// Decoder・Encoder の状態保存により前回の余り文字が1つ追加されるかもしれない

	// 出力バッファに必要な最大バイト数
	return srcMaxCharCount * to->GetMaxByteCount();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ByteBuffer Encoding::Convert(
	const void* src, size_t srcByteCount, const Encoding* srcEncoding,
	const Encoding* targetEncoding,
	const EncodingConversionOptions& options,
	EncodingConversionResult* result)
{
	std::unique_ptr<Decoder> decoder(srcEncoding->CreateDecoder());
	std::unique_ptr<Encoder> encoder(targetEncoding->CreateEncoder());
	return Convert(src, srcByteCount, decoder.get(), encoder.get(), options, result);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ByteBuffer Encoding::Convert(
	const void* src, size_t srcByteCount, Decoder* decoder,
	Encoder* encoder,
	const EncodingConversionOptions& options,
	EncodingConversionResult* result)
{
	LN_THROW(src != NULL, ArgumentException);
	LN_THROW(decoder != NULL, ArgumentException);
	LN_THROW(encoder != NULL, ArgumentException);

	// src に入っている最悪パターンの文字数
	size_t srcMaxCharCount = srcByteCount / decoder->GetMinByteCount();
	srcMaxCharCount += 1;									// Decoder・Encoder の状態保存により前回の余り文字が1つ追加されるかもしれない
	if (options.NullTerminated) { srcMaxCharCount += 1; }	// \0 の分

	// 中間バッファに必要な最大バイト数
	size_t utf16MaxByteCount = srcMaxCharCount * 4;	// UTF16 は1文字最大4バイト

	// 出力バッファに必要な最大バイト数
	size_t outputMaxByteCount = srcMaxCharCount * encoder->GetMaxByteCount();

	// 中間バッファ作成
	ByteBuffer tmpBuf(utf16MaxByteCount + sizeof(uint16_t), false);	// 終端 \0 考慮 (mbstowcs_s は \0 を書き込もうとする)

	// 変換先バッファを、最大要素数で確保
	ByteBuffer targetBuf(outputMaxByteCount + encoder->GetMaxByteCount(), false);	// 終端 \0 考慮 (mbstowcs_s は \0 を書き込もうとする)

	// 変換実行
	size_t bytesUsed;
	size_t charsUsed;
	// ソースフォーマットから中間フォーマットへ
	decoder->ConvertToUTF16(
		(const byte_t*)src,
		srcByteCount,
		(UTF16*)tmpBuf.GetData(),
		utf16MaxByteCount / sizeof(UTF16),			// \0 強制格納に備え、1文字分余裕のあるサイズを指定する
		&bytesUsed,
		&charsUsed);
	// 中間フォーマットからターゲットフォーマットへ
	encoder->ConvertFromUTF16(
		(const UTF16*)tmpBuf.GetData(),
		bytesUsed / sizeof(UTF16),
		(byte_t*)targetBuf.GetData(),
		targetBuf.GetSize(),		// \0 強制格納に備え、1文字分余裕のあるサイズを指定する
		&bytesUsed,
		&charsUsed);

	// \0 終端文字
	if (options.NullTerminated)
	{
		size_t nullBytes = encoder->GetMinByteCount();
		byte_t* buf = (byte_t*)targetBuf.GetData();
		for (size_t i = 0; i < nullBytes; ++i) {
			buf[bytesUsed + i] = 0x00;
		}
		bytesUsed += nullBytes;
	}

	if (result)
	{
		result->BytesUsed = bytesUsed;
		result->CharsUsed = charsUsed;
		result->UsedDefaultChar = (decoder->UsedDefaultCharCount() > 0 || encoder->UsedDefaultCharCount() > 0);
	}
	targetBuf.Resize(bytesUsed);	// 出力バッファの見かけ上のサイズを、実際に使用したバイト数にする
	return targetBuf;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Encoding::Convert(
	const void* src, size_t srcByteCount, Encoding* srcEncoding,
	void* dest, size_t destByteCount, Encoding* destEncoding,
	EncodingConversionResult* result)
{
	// TODO: できればメモリ確保はしたくないが…
	std::unique_ptr<Decoder> decoder(srcEncoding->CreateDecoder());
	std::unique_ptr<Encoder> encoder(destEncoding->CreateEncoder());
	return Convert(src, srcByteCount, decoder.get(), dest, destByteCount, encoder.get(), result);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Encoding::Convert(
	const void* src_, size_t srcByteCount, Decoder* srcDecoder,
	void* dest_, size_t destByteCount, Encoder* destEncoder,
	EncodingConversionResult* result)
{
	LN_VERIFY_RETURN(srcDecoder != NULL);
	LN_VERIFY_RETURN(srcDecoder->CanRemain());
	LN_VERIFY_RETURN(destEncoder != NULL);
	LN_VERIFY_RETURN(destEncoder->CanRemain());

	const size_t BufferingElements = 512;
	UTF16 utf16[BufferingElements];
	size_t totalBytesUsed = 0;
	size_t totalCharsUsed = 0;
	size_t bytesUsed;
	size_t charsUsed;
	const byte_t* src = (const byte_t*)src_;
	byte_t* dest = (byte_t*)dest_;
	size_t srcPos = 0;
	size_t destPos = 0;

	for (;;)
	{
		if (srcPos >= srcByteCount || destPos >= destByteCount) {
			break;
		}

		// UTF16 へ
		size_t srcBytes = std::min(srcByteCount - srcPos, BufferingElements);
		srcDecoder->ConvertToUTF16(
			&src[srcPos],
			srcBytes,
			utf16,
			BufferingElements,
			&bytesUsed,
			&charsUsed);
		srcPos += srcBytes;

		// UTF16 文字をターゲットへ
		destEncoder->ConvertFromUTF16(
			utf16,
			bytesUsed / sizeof(UTF16),
			&dest[destPos],
			destByteCount - destPos,
			&bytesUsed,
			&charsUsed);
		destPos += bytesUsed;

		totalBytesUsed += bytesUsed;
		totalCharsUsed += charsUsed;
	}

	if (result)
	{
		result->BytesUsed = totalBytesUsed;
		result->CharsUsed = totalCharsUsed;
		result->UsedDefaultChar = (srcDecoder->UsedDefaultCharCount() > 0 || destEncoder->UsedDefaultCharCount() > 0);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
size_t Encoding::CheckPreamble(const void* buffer, size_t bufferSize) const
{
	LN_VERIFY_RETURNV(buffer != NULL, 0);

	const char* bom = (const char*)GetPreamble();
	size_t bomLen = strlen(bom);
	if (bufferSize < bomLen) {
		return 0;
	}

	if (memcmp(buffer, bom, bomLen) == 0) {
		return bomLen;
	}
	return 0;
}

#if 0
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
#ifdef LN_OS_WIN32
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
int SystemMultiByteEncoding::GetCharacterCount(const void* buffer, size_t bufferSize) const
{

}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void SystemMultiByteEncoding::SystemMultiByteDecoder::ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
#ifdef LN_OS_WIN32	/* Windows 環境で setlocale しなくても使えるようにする */
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
		(int)inBufferByteCount,		// lpMultiByteStr が指す文字列のサイズをバイト単位で渡します。
		(LPWSTR)outBuffer, 
		(int)outBufferCharCount);	// lpWideCharStr が指すバッファのサイズをワイド文字数の単位で指定します。
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
	ByteBuffer tmpUTF32Buffer(sizeof(wchar_t) * inBufferByteCount, true);

	// wchar_t (UTF-32) へ変換する
	const char* str_ptr = (const char*)inBuffer;
	mbstate_t state;
	memset(&state, 0, sizeof(state));
	size_t len = mbsrtowcs((wchar_t*)tmpUTF32Buffer.GetData(), &str_ptr, inBufferByteCount, &state);
	LN_THROW(len != -1, EncodingFallbackException);

	// UTF-32 から UTF-16 へ変換する
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;
	UTFConversionResult result = UnicodeUtils::ConvertUTF32toUTF16(
		(UnicodeUtils::UTF32*)tmpUTF32Buffer.GetData(),
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
	ByteBuffer tmpInBuffer;
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
	ByteBuffer tmpInBuffer;
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
#ifdef LN_OS_WIN32	/* Windows 環境で setlocale しなくても使えるようにする */
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
	ByteBuffer tmpUTF32Buffer(sizeof(wchar_t) * (inBufferCharCount * 2), true);

	// UTF-32 へ変換する
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;
	UTFConversionResult result = UnicodeUtils::ConvertUTF16toUTF32(
		inBuffer,
		inBufferCharCount,
		(UnicodeUtils::UTF32*)tmpUTF32Buffer.GetData(),
		tmpUTF32Buffer.GetSize(),
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	// UTF-32 を char[] へ変換する (wcsrtombs() は出力バッファにあまりがあるときは '\0' をつけるが、一杯の時はつけない)
	const wchar_t* wstr_ptr = (const wchar_t*)tmpUTF32Buffer.GetData();
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
	ByteBuffer tmpWideBuffer;
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
#endif

LN_NAMESPACE_END
