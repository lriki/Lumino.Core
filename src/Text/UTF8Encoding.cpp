
#include "../Internal.h"
#include "../../include/Lumino/Text/UnicodeUtils.h"
#include "UTF8Encoding.h"

namespace Lumino
{
	
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
int UTF8Encoding::GetCharacterCount(const byte_t* buffer, size_t bufferSize) const
{
	int count;
	UTFConversionResult result = UnicodeUtils::GetUTF8CharCount(buffer, bufferSize, true, &count);
	LN_THROW(result == UTFConversionResult_Success, EncodingException);
	return count;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int UTF8Encoding::GetLeadExtraLength(const void* buffer, size_t bufferSize) const
{
	int len;
	UTFConversionResult result = UnicodeUtils::CheckUTF8TrailingBytes(((const byte_t*)buffer), ((const byte_t*)buffer) + bufferSize, true, &len);
	LN_THROW(result == UTFConversionResult_Success, EncodingException);
	return len;
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
		LN_THROW(r == 0, EncodingException);
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
	LN_THROW(result == UTFConversionResult_Success, EncodingException);

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
	LN_THROW(result == UTFConversionResult_Success, EncodingException);

	// 出力
	*outBytesUsed = options.ConvertedTargetLength;
	*outCharsUsed = options.CharCount;
}

} // namespace Lumino
