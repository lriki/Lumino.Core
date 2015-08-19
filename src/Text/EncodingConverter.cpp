
#include <algorithm>
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

	// �ϊ�����̂ɕK�v�ȃo�C�g���ŗ̈�m��
	size_t size = Text::Encoding::GetConversionRequiredByteCount(m_srcEncoding, m_dstEncoding, byteCount);
	m_outputBuffer.Resize(size);

	if (m_srcDecoder->CanRemain())
	{
		//EncodingConversionResult localResult;
		//if (outResult == NULL) { outResult = &localResult; }	// outResult �� NULL �ł����ʂ��󂯎�肽��

		ConvertDecoderRemain(data, byteCount, m_srcDecoder, m_outputBuffer.GetData(), size, m_dstEncoder, &m_lastResult);
		m_outputBuffer.Resize(m_lastResult.BytesUsed);	// �]���Ɋm�ۂ���Ă���̂ŁA��������̃T�C�Y�����ۂɕ����̂���T�C�Y�ɂ���

		if (outResult != NULL) { *outResult = m_lastResult; }
	}
	// �f�R�[�_���ϊ���Ԃ�ێ��ł��Ȃ��ꍇ�͂�ނ𓾂Ȃ��̂ňꎞ���������m�ۂ��A�\�[�X�o�b�t�@�S�̂���x�ɕϊ�����B
	else
	{
		// TCHAR �� UTF16 �֑S���ϊ�����̂ɕK�v�ȃo�C�g���ňꎞ�������m��
		size_t totalByteCount = Text::Encoding::GetConversionRequiredByteCount(m_srcEncoding, m_dstEncoding, byteCount);
		m_tmpBuffer.Resize(totalByteCount);

		// ��̃R�[�h���L���X�g���炯�ɂȂ�Ȃ��悤��
		Text::UTF16* utf16Buf = (Text::UTF16*)m_tmpBuffer.GetData();
		int utf16ElementCount = m_tmpBuffer.GetSize() / sizeof(Text::UTF16);

		// TCHAR �𒆊ԃR�[�h(UTF16) ��
		size_t outBytesUsed, outCharsUsed;
		m_srcDecoder->ConvertToUTF16((const byte_t*)data, byteCount, utf16Buf, utf16ElementCount, &outBytesUsed, &outCharsUsed);

		// ���ԃR�[�h(UTF16)���o�̓R�[�h��
		m_dstEncoder->ConvertFromUTF16(utf16Buf, outCharsUsed, m_outputBuffer.GetData(), m_outputBuffer.GetSize(), &outBytesUsed, &outCharsUsed);

		// �]���Ɋm�ۂ���Ă���̂ŁA��������̃T�C�Y�����ۂɕ����̂���T�C�Y�ɂ���
		m_outputBuffer.Resize(outBytesUsed);

		m_lastResult.BytesUsed = outBytesUsed;
		m_lastResult.CharsUsed = outCharsUsed;
		m_lastResult.UsedDefaultChar = (m_srcDecoder->UsedDefaultCharCount() > 0 || m_dstEncoder->UsedDefaultCharCount() > 0);
		if (outResult != NULL) 
		{
			outResult->BytesUsed = m_lastResult.BytesUsed;
			outResult->CharsUsed = m_lastResult.CharsUsed;
			outResult->UsedDefaultChar = m_lastResult.UsedDefaultChar;
		}
	}
	return m_outputBuffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const ByteBuffer& EncodingConverter::GetLastBuffer() const
{
	return m_outputBuffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const EncodingConversionResult& EncodingConverter::GetLastResult() const
{
	return m_lastResult;
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

		// UTF16 ��
		size_t srcBytes = std::min(srcByteCount - srcPos, BufferingElements);
		srcDecoder->ConvertToUTF16(
			&src[srcPos],
			srcBytes,
			utf16,
			BufferingElements,
			&bytesUsed,
			&charsUsed);
		srcPos += srcBytes;

		// UTF16 �������^�[�Q�b�g��
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
