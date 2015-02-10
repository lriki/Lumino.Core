
#include "../Internal.h"
#include "../../include/Lumino/Text/UnicodeUtils.h"
#include "UTF16Encoding.h"

namespace Lumino
{
namespace Text
{

//=============================================================================
// UTF16Encoding
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
UTF16Encoding::UTF16Encoding(bool bigEndian, bool byteOrderMark)
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
byte_t* UTF16Encoding::GetPreamble() const
{
	static byte_t bom[] = { 0x00 };
	LN_THROW(0, NotImplementedException);
	return bom;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF16Encoding::UTF16Decoder::ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	/* �o�C�g�X�g���[���� UTF-16 ����A���������R�[�h�� UTF-16 �ւ̕ϊ��ƂȂ�B
	 * �o�C�g�X�g���[���́AUTF-16 �������o�b�t�@�I�[�œr�؂��ꍇ���l�����Ȃ���΂Ȃ�Ȃ��B
	 */

	// �O��r���œr�؂ꂽ�o�C�g������ΐ�ɏ������� (inBuffer �̐擪�o�C�g�ƂȂ��� curLead �Ɋi�[)
	uint16_t curLead = 0x0000;
	if (m_lastLeadByte != 0x00)
	{
		byte_t* word = (byte_t*)&curLead;
		word[0] = m_lastLeadByte;
		word[1] = inBuffer[0];
		inBuffer++;				// ������������o�b�t�@���k�߂�
		inBufferByteCount--;	// ������������o�b�t�@���k�߂�
		m_lastLeadByte = 0x00;
	}

	// ���͂���o�C�g�B�Ō�̃o�C�g������Ă���
	if (inBufferByteCount % 2 != 0) 
	{
		m_lastLeadByte = inBuffer[inBufferByteCount - 1];
		inBufferByteCount--;	// ������������o�b�t�@���k�߂�
	}

	// �ϊ� (�����O��̃o�b�t�@�I�[����ʃT���Q�[�g��������Am_lastLeadWord �ɐ�s�o�C�g�������Ă���)
	size_t inWordPos = (curLead != 0x0000) ? SIZE_MAX : 0;	// MBCS
	size_t outWordPos = 0;									// UTF16
	size_t inWordCount = inBufferByteCount / 2;
	uint16_t* inWords = (uint16_t*)inBuffer;
	size_t charCount = 0;
	size_t usedByteCount = 0;
	for (; inWordPos == SIZE_MAX || inWordPos < inWordCount;)
	{
		uint16_t ch = (inWordPos == SIZE_MAX) ? curLead : inWords[inWordPos];	// �r�؂ꂽ�o�C�g�ƌ��������ŏ���1�������l��
	
		// ��ʃT���Q�[�g��������Ԃ̏ꍇ
		if (m_lastLeadWord == 0x0000)
		{
			if (UnicodeUtils::CheckUTF16HighSurrogate(ch)) {
				m_lastLeadWord = ch;	// ��ʃT���Q�[�g������Ԃɂ���
			}
			else {
				// ���ʂ�UTF16�����B���ʂɊi�[�B
				outBuffer[outWordPos++] = ch;
				++charCount;
				usedByteCount += 2;
			}
		}
		// ���O�̕�������s�o�C�g�̏ꍇ
		else
		{
			// ���ʃT���Q�[�g��������Ίi�[
			if (UnicodeUtils::CheckUTF16LowSurrogate(ch)) 
			{
				outBuffer[outWordPos++] = m_lastLeadWord;
				outBuffer[outWordPos++] = ch;
				++charCount;
				usedByteCount += 4;
				m_lastLeadWord = 0x0000;
			}
			else 
			{
				// ���ʃT���Q�[�g�ȊO�̕�����NG
				LN_THROW(0, EncodingFallbackException);
			}
		}

		// inWordPos ��i�߂�
		if (inWordPos == SIZE_MAX) {
			inWordPos = 0;	// ��ԍŏ��̕����̏������I������BinWords �����n�߂�
		}
		else {
			++inWordPos;
		}
	}

	// �o��
	(*outBytesUsed) = usedByteCount;
	(*outCharsUsed) = charCount;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF16Encoding::UTF16Encoder::ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	/* ���������R�[�h�� UTF-16 ����A�o�C�g�X�g���[���� UTF-16 �ւ̕ϊ��ƂȂ�B
	 * ���������R�[�h�́A�K�� 2byte �P�ʂł���A�o�b�t�@�I�[���o�C�g�P�ʂœr�؂�邱�Ƃ͖����B
	 * ���̂��ߏ�ԕێ�����K�v�͖����A���̂܂܃R�s�[�ł悢�B
	 */

	errno_t err = memcpy_s(outBuffer, outBufferByteCount, inBuffer, inBufferCharCount * sizeof(UTF16));
	LN_THROW(err == 0, ArgumentException);

	// �������̓J�E���g����
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inBufferCharCount, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = inBufferCharCount * sizeof(UTF16);
	*outCharsUsed = count;
}

} // namespace Text
} // namespace Lumino
