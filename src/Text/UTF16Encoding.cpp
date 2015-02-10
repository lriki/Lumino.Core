
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
	/* バイトストリームの UTF-16 から、内部文字コードの UTF-16 への変換となる。
	 * バイトストリームは、UTF-16 文字がバッファ終端で途切れる場合も考慮しなければならない。
	 */

	// 前回途中で途切れたバイトがあれば先に処理する (inBuffer の先頭バイトとつなげて curLead に格納)
	uint16_t curLead = 0x0000;
	if (m_lastLeadByte != 0x00)
	{
		byte_t* word = (byte_t*)&curLead;
		word[0] = m_lastLeadByte;
		word[1] = inBuffer[0];
		inBuffer++;				// 消費した分だけバッファを縮める
		inBufferByteCount--;	// 消費した分だけバッファを縮める
		m_lastLeadByte = 0x00;
	}

	// 入力が奇数バイト。最後のバイトを取っておく
	if (inBufferByteCount % 2 != 0) 
	{
		m_lastLeadByte = inBuffer[inBufferByteCount - 1];
		inBufferByteCount--;	// 消費した分だけバッファを縮める
	}

	// 変換 (もし前回のバッファ終端が上位サロゲートだったら、m_lastLeadWord に先行バイトが入っている)
	size_t inWordPos = (curLead != 0x0000) ? SIZE_MAX : 0;	// MBCS
	size_t outWordPos = 0;									// UTF16
	size_t inWordCount = inBufferByteCount / 2;
	uint16_t* inWords = (uint16_t*)inBuffer;
	size_t charCount = 0;
	size_t usedByteCount = 0;
	for (; inWordPos == SIZE_MAX || inWordPos < inWordCount;)
	{
		uint16_t ch = (inWordPos == SIZE_MAX) ? curLead : inWords[inWordPos];	// 途切れたバイトと結合した最初の1文字を考慮
	
		// 上位サロゲート未発見状態の場合
		if (m_lastLeadWord == 0x0000)
		{
			if (UnicodeUtils::CheckUTF16HighSurrogate(ch)) {
				m_lastLeadWord = ch;	// 上位サロゲート発見状態にする
			}
			else {
				// 普通のUTF16文字。普通に格納。
				outBuffer[outWordPos++] = ch;
				++charCount;
				usedByteCount += 2;
			}
		}
		// 直前の文字が先行バイトの場合
		else
		{
			// 下位サロゲートが見つかれば格納
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
				// 下位サロゲート以外の文字はNG
				LN_THROW(0, EncodingFallbackException);
			}
		}

		// inWordPos を進める
		if (inWordPos == SIZE_MAX) {
			inWordPos = 0;	// 一番最初の文字の処理が終わった。inWords を見始める
		}
		else {
			++inWordPos;
		}
	}

	// 出力
	(*outBytesUsed) = usedByteCount;
	(*outCharsUsed) = charCount;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF16Encoding::UTF16Encoder::ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	/* 内部文字コードの UTF-16 から、バイトストリームの UTF-16 への変換となる。
	 * 内部文字コードは、必ず 2byte 単位であり、バッファ終端がバイト単位で途切れることは無い。
	 * そのため状態保持する必要は無く、そのままコピーでよい。
	 */

	errno_t err = memcpy_s(outBuffer, outBufferByteCount, inBuffer, inBufferCharCount * sizeof(UTF16));
	LN_THROW(err == 0, ArgumentException);

	// 文字数はカウントする
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)inBuffer, inBufferCharCount, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingFallbackException);

	*outBytesUsed = inBufferCharCount * sizeof(UTF16);
	*outCharsUsed = count;
}

} // namespace Text
} // namespace Lumino
