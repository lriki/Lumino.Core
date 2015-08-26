
#include "../Internal.h"
#include "../../include/Lumino/Text/UnicodeUtils.h"
#include "UTF32Encoding.h"

namespace Lumino
{
namespace Text
{

//=============================================================================
// UTF32Encoding
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
UTF32Encoding::UTF32Encoding(bool bigEndian, bool byteOrderMark)
	: m_bigEndian(bigEndian)
	, m_byteOrderMark(byteOrderMark)
{
	if (m_bigEndian) {
		LN_THROW(0, NotImplementedException);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
byte_t* UTF32Encoding::GetPreamble() const
{
	static byte_t bom[] = { 0x00 };
	LN_THROW(0, NotImplementedException);
	return bom;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int UTF32Encoding::GetCharacterCount(const byte_t* buffer, size_t bufferSize) const
{
	return bufferSize / sizeof(UTF32);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF32Encoding::UTF32Decoder::ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	*outBytesUsed = 0;
	*outCharsUsed = 0;

	// 変換設定
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;

	// 前回途中で途切れたバイトがあれば先に変換する
	if (m_lastLeadBytesCount > 0) 
	{
		size_t req = 4 - m_lastLeadBytesCount;	// 追加要求バイト数
		byte_t buf[4];
		memcpy_s(buf, 4, m_lastLeadBytes, m_lastLeadBytesCount);
		memcpy_s(buf + m_lastLeadBytesCount, req, inBuffer, req);

		// 変換 (1文字だけ)
		UTFConversionResult result = UnicodeUtils::ConvertUTF32toUTF16((UnicodeUtils::UTF32*)buf, 1, (UnicodeUtils::UTF16*)outBuffer, outBufferCharCount, &options);
		LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

		// バッファ先頭は消費した分だけ進め、バッファサイズは消費した分だけ縮める
		inBuffer += req;
		inBufferByteCount -= req;
		outBuffer += options.ConvertedTargetLength / sizeof(UnicodeUtils::UTF16);
		outBufferCharCount -= options.ConvertedTargetLength / sizeof(UnicodeUtils::UTF16);
		(*outBytesUsed) += options.ConvertedTargetLength * sizeof(UnicodeUtils::UTF16);
		(*outCharsUsed) += 1;
	}

	// バッファ終端で文字が途切れている場合は次回の変換に回すため記憶しておく
	m_lastLeadBytesCount = inBufferByteCount % 4;
	if (m_lastLeadBytesCount > 0)
	{
		memcpy_s(m_lastLeadBytes, 4, &inBuffer[inBufferByteCount - m_lastLeadBytesCount], m_lastLeadBytesCount);
		inBufferByteCount -= m_lastLeadBytesCount;
	}
	
	// 変換
	UTFConversionResult result = UnicodeUtils::ConvertUTF32toUTF16(
		(UnicodeUtils::UTF32*)inBuffer, 
		inBufferByteCount / sizeof(UnicodeUtils::UTF32),
		(UnicodeUtils::UTF16*)outBuffer, 
		outBufferCharCount,
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	// 出力
	(*outBytesUsed) += options.ConvertedTargetLength * sizeof(UnicodeUtils::UTF16);
	(*outCharsUsed) += options.CharCount;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF32Encoding::UTF32Encoder::ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	*outBytesUsed = 0;
	*outCharsUsed = 0;

	// 変換設定
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;

	// 前回途中で途切れたワードがあれば先に変換する
	if (m_lastLeadWord != 0x0000)
	{
		uint16_t buf[2] = { m_lastLeadWord, inBuffer[0] };

		// 変換 (サロゲートペアで1文字だけ)
		UTFConversionResult result = UnicodeUtils::ConvertUTF16toUTF32((UnicodeUtils::UTF16*)buf, 2, (UnicodeUtils::UTF32*)outBuffer, outBufferByteCount, &options);
		LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

		// バッファ先頭は消費した分だけ進め、バッファサイズは消費した分だけ縮める
		size_t usedTargetBytes = options.ConvertedTargetLength * sizeof(UnicodeUtils::UTF32);
		inBuffer += 1;
		inBufferCharCount -= 1;
		outBuffer += usedTargetBytes;
		outBufferByteCount -= usedTargetBytes;
		(*outBytesUsed) += usedTargetBytes;
		(*outCharsUsed) += options.CharCount;
	}

	// 前回途中で途切れたワードがあれば先に変換する (とりあえずエンディアン考慮せず、サロゲートなら取っておく)
	if (UnicodeUtils::CheckUTF16HighSurrogate(inBuffer[inBufferCharCount - 1])/* || UnicodeUtils::CheckUTF16LowSurrogate(inBuffer[inBufferCharCount])*/)
	{
		m_lastLeadWord = inBuffer[inBufferCharCount];
		inBufferCharCount -= 1;
	}
	
	// 変換
	UTFConversionResult result = UnicodeUtils::ConvertUTF16toUTF32(
		(UnicodeUtils::UTF16*)inBuffer, 
		inBufferCharCount,
		(UnicodeUtils::UTF32*)outBuffer, 
		outBufferByteCount / sizeof(UnicodeUtils::UTF32),
		&options);
	LN_THROW(result == UTFConversionResult_Success, EncodingFallbackException);

	// 出力
	(*outBytesUsed) += options.ConvertedTargetLength * sizeof(UnicodeUtils::UTF32);
	(*outCharsUsed) += options.CharCount;
}

} // namespace Text
} // namespace Lumino
