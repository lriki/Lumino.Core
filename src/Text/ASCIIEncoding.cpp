
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

		// ���ʂ� ASCII ����
		if (ch <= 0x7F) {
			outBuffer[i] = static_cast<UTF16>(ch);
		}
		// ��֕������w�肳��Ă���΂�����g��
		else if (mFallbackReplacementChar != 0x00) {
			outBuffer[i] = static_cast<UTF16>(mFallbackReplacementChar);
		}
		// ASCII �O�ő�֕�����������΃G���[
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

		// ���ʂ� ASCII ����
		if (ch <= 0x7F) {
			outBuffer[i] = static_cast<byte_t>(ch);
		}
		// ��֕������w�肳��Ă���΂�����g��
		else if (mFallbackReplacementChar != 0x00) {
			outBuffer[i] = static_cast<byte_t>(mFallbackReplacementChar);
		}
		// ASCII �O�ő�֕�����������΃G���[
		else {
			LN_THROW(0, EncodingFallbackException);
		}
	}

	(*outBytesUsed) = inBufferCharCount;	// �����������̂܂܃o�C�g��
	(*outCharsUsed) = inBufferCharCount;
}

} // namespace Text
} // namespace Lumino
