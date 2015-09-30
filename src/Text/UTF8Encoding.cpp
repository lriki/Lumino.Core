
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
	static byte_t bom[] = { 0xEF, 0xBB, 0xBF, 0x00 };
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
	*outBytesUsed = 0;
	*outCharsUsed = 0;
	int bytesUsed = 0;
	int charsUsed = 0;

	// 変換設定
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;

	// BOM 付きの場合は取り除く (バッファ縮小)
	if (m_byteOrderMark)
	{
		if (m_bomPhase == 0)
		{
			if (inBuffer[0] == 0xEF)
			{
				m_bomPhase = 1;
				++inBuffer;
				--inBufferByteCount;
				++bytesUsed;
				if (inBufferByteCount == 0) { *outBytesUsed = bytesUsed; return; }
			}
		}
		if (m_bomPhase == 1)
		{
			if (inBuffer[0] == 0xBB)
			{
				m_bomPhase = 2;
				++inBuffer;
				--inBufferByteCount;
				++bytesUsed;
				if (inBufferByteCount == 0) { *outBytesUsed = bytesUsed; return; }
			}
			else {
				LN_THROW(0, EncodingException, _T("bom is required."));
			}
		}
		if (m_bomPhase == 2)
		{
			if (inBuffer[0] == 0xBF)
			{
				m_bomPhase = 0;
				++inBuffer;
				--inBufferByteCount;
				++bytesUsed;
				if (inBufferByteCount == 0) { *outBytesUsed = bytesUsed; return; }
			}
			else {
				LN_THROW(0, EncodingException, _T("bom is required."));
			}
		}
	}

	const byte_t* srcPos = inBuffer;
	const byte_t* srcEnd = inBuffer + inBufferByteCount;
	UTF16* dstPos = outBuffer;
	UTF16* dstEnd = dstPos + outBufferCharCount;
	while (srcPos < srcEnd)
	{
		LN_THROW(m_lastLeadBytesCount < 5, EncodingException, _T("Illegal UTF-8 char."));

		// 先行バイトの確認
		if (m_requestLeadBytesCount == 0)
		{
			UnicodeUtils::CheckUTF8TrailingBytes(srcPos, srcEnd, false, &m_requestLeadBytesCount);
			//m_requestLeadBytesCount++;	// ↓で今回の文字も含めて m_lastLeadBytes に保存したい
		}
		
		if (m_requestLeadBytesCount > 0)
		{
			// 先行バイトの保存
			m_lastLeadBytes[m_lastLeadBytesCount] = srcPos[0];
			++m_lastLeadBytesCount;
			--m_requestLeadBytesCount;
		}
		else
		{
			m_lastLeadBytes[m_lastLeadBytesCount] = srcPos[0];
			++m_lastLeadBytesCount;

			// 一度 UTF32 文字へ
			UTFConversionResult result;
			UnicodeUtils::UTF32 ch;
			const UnicodeUtils::UTF8* start = m_lastLeadBytes;
			const UnicodeUtils::UTF8* end = start + m_lastLeadBytesCount;
			result = UnicodeUtils::ConvertCharUTF8toUTF32(&start, end, NULL, &ch);
			LN_THROW(result == UTFConversionResult_Success, EncodingException);

			// UTF32 から UTF16 へ 
			result = UnicodeUtils::ConvertCharUTF32toUTF16(ch, &dstPos, dstEnd, &options);
			LN_THROW(result == UTFConversionResult_Success, EncodingException);

			++charsUsed;
			m_lastLeadBytesCount = 0;
		}

		++bytesUsed;
		++srcPos;
	}

	
	
	
	// 変換
	//UTFConversionResult result = UnicodeUtils::ConvertUTF8toUTF16(
	//	(UnicodeUtils::UTF8*)inBuffer, 
	//	inBufferByteCount,
	//	(UnicodeUtils::UTF16*)outBuffer, 
	//	outBufferCharCount,
	//	&options);
	//LN_THROW(result == UTFConversionResult_Success, EncodingException);

	// 出力
	*outBytesUsed = bytesUsed;
	*outCharsUsed = charsUsed;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//int UTF8Encoding::UTF8Encoder::PollingChar(UTF8 ch, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed)
//{
//	
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF8Encoding::UTF8Encoder::ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	*outCharsUsed = 0;
	*outBytesUsed = 0;

	// 変換設定
	UTFConversionOptions options;
	memset(&options, 0, sizeof(options));
	options.ReplacementChar = mFallbackReplacementChar;

	const UTF16* srcPos = inBuffer;
	const UTF16* srcEnd = inBuffer + inBufferCharCount;
	UTF8* dstPos = outBuffer;
	UTF8* dstEnd = dstPos + outBufferByteCount;

	while (srcPos < srcEnd)
	{
		// サロゲートの確認
		if (UnicodeUtils::CheckUTF16HighSurrogate(*srcPos))
		{
			if (m_lastBufferCount == 0)
			{
				m_lastBuffer[m_lastBufferCount] = *srcPos;
				++m_lastBufferCount;
			}
			else {
				// High が連続している
				LN_THROW(0, EncodingException);
			}
		}
		else
		{
			if (UnicodeUtils::CheckUTF16LowSurrogate(*srcPos))
			{
				if (m_lastBufferCount == 1) {
				}
				else {
					// いきなり Low が来た
					LN_THROW(0, EncodingException);
				}
			}
			else // 普通の文字
			{
				if (m_lastBufferCount == 0) {
				}
				else {
					// Low が無かった
					LN_THROW(0, EncodingException);
				}
			}
			m_lastBuffer[m_lastBufferCount] = *srcPos;
			++m_lastBufferCount;


			// 一度 UTF-32 文字へ
			UTFConversionResult result;
			const UTF16* bufStart = m_lastBuffer;
			const UTF16* bufEnd = m_lastBuffer + m_lastBufferCount;
			UnicodeUtils::UTF32 ch;
			result = UnicodeUtils::ConvertCharUTF16toUTF32(&bufStart, bufEnd, &options, &ch);
			LN_THROW(result == UTFConversionResult_Success, EncodingException);

			// UTF-8 文字へ
			result = UnicodeUtils::ConvertCharUTF32toUTF8(ch, &dstPos, dstEnd, &options);
			LN_THROW(result == UTFConversionResult_Success, EncodingException);

			m_lastBufferCount = 0;
			(*outCharsUsed) += 1;
		}

		(*outBytesUsed) += sizeof(UTF16);
		++srcPos;
	}


	// 変換設定
	//UTFConversionOptions options;
	//memset(&options, 0, sizeof(options));
	//options.ReplacementChar = mFallbackReplacementChar;
	//
	//// 変換
	//UTFConversionResult result = UnicodeUtils::ConvertUTF16toUTF8(
	//	(UnicodeUtils::UTF16*)inBuffer, 
	//	inBufferCharCount,
	//	(UnicodeUtils::UTF8*)outBuffer, 
	//	outBufferByteCount,
	//	&options);
	//LN_THROW(result == UTFConversionResult_Success, EncodingException);

	// 出力
	//*outBytesUsed = options.ConvertedTargetLength;
	//*outCharsUsed = options.CharCount;
}

} // namespace Lumino
