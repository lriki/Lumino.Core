
#include <stdio.h>
#include <string.h>
#include "../Internal.h"
#include "../../include/Lumino/Base/StringUtils.h"
#include "../../include/Lumino/Text/UnicodeUtils.h"
#include "EncodingDetector.h"

namespace Lumino
{

//=============================================================================
// Encoding
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EncodingType EncodingDetector::Detect(const void* bytes, size_t bytesSize)
{
	if (bytes == NULL || bytesSize == 0) {
		return EncodingType_Unknown;
	}
	m_buffer = (const Byte*)bytes;
	m_bufferSize = bytesSize;

	// UTF 系の BOM チェック
	EncodingType type = CheckUTFBom();
	if (type != EncodingType_Unknown) {
		return type;	// ★ UTF系確定
	}

	// マルチバイト文字コードのチェック
	SJISDetector sjisDetector;
	UTF8NDetector utf8NDetector;
	IMBSCodeDetector* mbsDetectors[] = {
		&utf8NDetector,
		&sjisDetector,
	};
	const int mbsDetectorsCount = LN_ARRAY_SIZE_OF(mbsDetectors);
	for (int i = 0; i < mbsDetectorsCount; ++i)
	{
		mbsDetectors[i]->Detect(m_buffer, m_bufferSize);
	}

	// 有効 Detector の中で一番スコアの大きいものを選択する
	int maxScore = 0;
	IMBSCodeDetector* maxScoreDetector = NULL;
	for (int i = 0; i < mbsDetectorsCount; ++i)
	{
		if (!mbsDetectors[i]->IsUnMatch())
		{
			// まだひとつも見つかっていなければ、とりあえずマッチしたものを保持する
			if (maxScoreDetector == NULL)
			{
				maxScoreDetector = mbsDetectors[i];
			}
			// 現在保持しているものよりスコアが大きければそれを保持する
			else if (mbsDetectors[i]->GetScore() > maxScoreDetector->GetScore())
			{
				maxScoreDetector = mbsDetectors[i];
			}
		}
	}
	if (maxScoreDetector != NULL) {
		return maxScoreDetector->GetEncodingType();
	}

	return EncodingType_Unknown;	// 判別失敗
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
EncodingType EncodingDetector::CheckUTFBom()
{
	/* UTF8		: 0xEF, 0xBB, 0xBF
	 * UTF16	: 0xFF, 0xFE
	 * UTF16Big	: 0xFE, 0xFF
	 * UTF32	: 0xFF, 0xFE, 0x00, 0x00
	 * UTF32Big	: 0x00, 0x00, 0xFE, 0xFF
	 */

	struct BomData
	{
		EncodingType	Type;
		Byte			Bom[4];
		size_t			Length;
	};
	BomData data[] = 
	{
		{ EncodingType_UTF8,	{ 0xEF, 0xBB, 0xBF, 0x00 }, 3 },
		{ EncodingType_UTF32L,	{ 0xFF, 0xFE, 0x00, 0x00 }, 4 },	// UTF16Little と同じなので先に判別しないとダメ
		{ EncodingType_UTF32B,	{ 0x00, 0x00, 0xFE, 0xFF }, 4 },
		{ EncodingType_UTF16L,	{ 0xFF, 0xFE, 0x00, 0x00 }, 2 },
		{ EncodingType_UTF16B,	{ 0xFE, 0xFF, 0x00, 0x00 }, 2 },
	};
	for (int i = 0; i < 5; i++)
	{
		if (data[i].Length <= m_bufferSize &&
			memcmp(m_buffer, data[i].Bom, data[i].Length) == 0)
		{
			return data[i].Type;
		}
	}

	return EncodingType_Unknown;	// BOM が無かった
}


//=============================================================================
// UTF8NDetector
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void UTF8NDetector::Detect(const void* bytes, size_t bytesSize)
{
	byte_t* data = (byte_t*)bytes;
	byte_t* dataEnd = data + bytesSize;
	int pos = 0;
	m_lineNum = 0;
	m_score = 0;
	m_unmatch = false;

	for (size_t pos = 0; pos < bytesSize; ++pos)
	{
		byte_t b1 = data[pos];
		if (b1 <= 0x7F)	// ASCII (0x00-0x7F)
		{
			int n = StringUtils::CheckNewLineSequence(&data[pos], dataEnd);
			if (n > 0) {
				pos += n - 1;
				++m_lineNum;
			}
		}
		else
		{
			int extra;	// 追加で読むべきバイト数。pos の分は含まない
			if (UnicodeUtils::CheckUTF8TrailingBytes(&data[pos], dataEnd, true, &extra) == UTFConversionResult_Success)
			{
				pos += extra;
				m_score += 1 + extra;
			}
			else {
				m_unmatch = true;
			}
		}

		// 不正文字が見つかったので終了
		if (m_unmatch) { break; }
	}
}


//=============================================================================
// SJISDetector
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void SJISDetector::Detect(const void* bytes, size_t bytesSize)
{
	byte_t* data = (byte_t*)bytes;
	byte_t* dataEnd = data + bytesSize;
	int pos = 0;
	m_lineNum = 0;
	m_score = 0;
	m_unmatch = false;

	for (size_t pos = 0; pos < bytesSize; ++pos)
	{
		byte_t b1 = data[pos];
		if (b1 <= 0x7F)	// ASCII (0x00-0x7F)
		{
			int n = StringUtils::CheckNewLineSequence(&data[pos], dataEnd);
			if (n > 0) {
				pos += n - 1;
				++m_lineNum;
			}
		}
		else if (b1 >= 0xA1 && b1 <= 0xDF) // 1バイト半角カナ
		{
			++m_score;
		}
		else if (pos + 1 < bytesSize)
		{
			byte_t b2 = data[pos];
			if ((0x81 <= b1 && b1 <= 0x9F) ||		// 先行バイト
				(0xE0 <= b1 && b1 <= 0xFC))
			{
				if ((0x40 <= b2 && b2 <= 0x7E) ||	// 2byte目
					(0x80 <= b2 && b2 <= 0xFC))
				{
					++pos;
				}
				else {
					m_unmatch = true;
				}
			}
			else {
				m_unmatch = true;
			}
		}
		else { // バッファ末尾でマルチバイト文字が途切れている
			m_unmatch = true;
		}

		// 不正文字が見つかったので終了
		if (m_unmatch) { break; }
	}
}

} // namespace Lumino

