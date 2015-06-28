
#include "../Internal.h"
#include "../../include/Lumino/Text/EncodingConverter.h"

namespace Lumino
{
namespace Text
{

//=============================================================================
// EncodingConverter
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EncodingConverter::EncodingConverter()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EncodingConverter::~EncodingConverter()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void EncodingConverter::SetDestinationEncoding(Encoding* encoding)
{
	m_dstEncoding = encoding;
	m_encodingModified = true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Encoding* EncodingConverter::GetDestinationEncoding() const
{
	return m_dstEncoding;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void EncodingConverter::SetSourceEncoding(Encoding* encoding)
{
	m_srcEncoding = encoding;
	m_encodingModified = true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Encoding* EncodingConverter::GetSourceEncoding() const
{
	return m_srcEncoding;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const ByteBuffer& EncodingConverter::Convert(const void* data, size_t byteCount, EncodingConversionResult* outResult)
{
	CheckUpdateEncoderDecoder();

	// 変換するのに必要なバイト数で領域確保
	size_t size = Text::Encoding::GetConversionRequiredByteCount(m_srcEncoding, m_dstEncoding, byteCount);
	m_outputBuffer.Resize(size);

	if (m_srcDecoder->CanRemain())
	{
		ConvertDecoderRemain(data, byteCount, m_srcDecoder, m_outputBuffer.GetData(), size, m_dstEncoder, outResult);
	}
	else {
		LN_THROW(0, NotImplementedException);
	}
	return m_outputBuffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void EncodingConverter::CheckUpdateEncoderDecoder()
{
	if (m_encodingModified)
	{
		m_dstEncoder.Attach(m_dstEncoding->CreateEncoder());
		m_srcDecoder.Attach(m_srcEncoding->CreateDecoder());
		m_encodingModified = false;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void EncodingConverter::ConvertDecoderRemain(
	const void* src_, size_t srcByteCount, Decoder* srcDecoder,
	void* dest_, size_t destByteCount, Encoder* destEncoder,
	EncodingConversionResult* outResult)
{
	LN_VERIFY_RETURN(srcDecoder != NULL);
	LN_VERIFY_RETURN(srcDecoder->CanRemain());
	LN_VERIFY_RETURN(destEncoder != NULL);

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

	if (outResult)
	{
		outResult->BytesUsed = totalBytesUsed;
		outResult->CharsUsed = totalCharsUsed;
		outResult->UsedDefaultChar = (srcDecoder->UsedDefaultCharCount() > 0 || destEncoder->UsedDefaultCharCount() > 0);
	}
}

} // namespace Text
} // namespace Lumino
