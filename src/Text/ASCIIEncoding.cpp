
#include "../Internal.h"
#include "ASCIIEncoding.h"

namespace Lumino
{
namespace Text
{

//=============================================================================
// ASCIIEncoding
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ASCIIEncoding::ASCIIEncoding()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASCIIEncoding::ASCIIDecoder::ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	for (size_t i = 0; i < inBufferByteCount; ++i)
	{
		byte_t ch = inBuffer[i];

		// 普通の ASCII 文字
		if (ch <= 0x7F) {
			outBuffer[i] = static_cast<UTF16>(ch);
		}
		// 代替文字が指定されていればそれを使う
		else if (mFallbackReplacementChar != 0x00) {
			outBuffer[i] = static_cast<UTF16>(mFallbackReplacementChar);
		}
		// ASCII 外で代替文字も無ければエラー
		else {
			LN_THROW(0, EncodingFallbackException);
		}
	}

	(*outBytesUsed) = inBufferByteCount * sizeof(UTF16);
	(*outCharsUsed) = inBufferByteCount;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ASCIIEncoding::ASCIIEncoder::ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	for (size_t i = 0; i < inBufferCharCount; ++i)
	{
		UTF16 ch = inBuffer[i];

		// 普通の ASCII 文字
		if (ch <= 0x7F) {
			outBuffer[i] = static_cast<byte_t>(ch);
		}
		// 代替文字が指定されていればそれを使う
		else if (mFallbackReplacementChar != 0x00) {
			outBuffer[i] = static_cast<byte_t>(mFallbackReplacementChar);
		}
		// ASCII 外で代替文字も無ければエラー
		else {
			LN_THROW(0, EncodingFallbackException);
		}
	}

	(*outBytesUsed) = inBufferCharCount;	// 文字数がそのままバイト数
	(*outCharsUsed) = inBufferCharCount;
}

} // namespace Text
} // namespace Lumino
