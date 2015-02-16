
#include "../Internal.h"
#include "../../include/Lumino/Base/RefBuffer.h"
#include "../../include/Lumino/Text/UnicodeUtils.h"
#include "DBCSEncoding.h"

namespace Lumino
{
namespace Text
{

//=============================================================================
// DBCSEncoding
//=============================================================================

// SJIS
extern "C" const unsigned char g_SJISLeadBytePairs[];
extern "C" const unsigned short g_SJISToUTF16Table[];
extern "C" const unsigned short g_UTF16ToSJISTable[];

// GB2312
extern "C" const unsigned char g_GB2312LeadBytePairs[];
extern "C" const unsigned short g_GB2312ToUTF16Table[];
extern "C" const unsigned short g_UTF16ToGB2312Table[];

// EUC-KR
extern "C" const unsigned char g_EUCKRLeadBytePairs[];
extern "C" const unsigned short g_EUCKRToUTF16Table[];
extern "C" const unsigned short g_UTF16ToEUCKRTable[];

// Big5
extern "C" const unsigned char g_Big5LeadBytePairs[];
extern "C" const unsigned short g_Big5ToUTF16Table[];
extern "C" const unsigned short g_UTF16ToBig5Table[];

const DBCSEncoding::TableInfo DBCSEncoding::Tables[EncodingType_Max] =
{
	{ NULL, NULL, NULL },	// EncodingType_Unknown
	{ NULL, NULL, NULL },	// EncodingType_ASCII
	{ NULL, NULL, NULL },	// EncodingType_UTF8,
	{ NULL, NULL, NULL },	// EncodingType_UTF8N,
	{ NULL, NULL, NULL },	// EncodingType_UTF16L,
	{ NULL, NULL, NULL },	// EncodingType_UTF16B,
	//{ NULL, NULL, NULL },	// EncodingType_UTF16LN,
	//{ NULL, NULL, NULL },	// EncodingType_UTF16BN,
	{ NULL, NULL, NULL },	// EncodingType_UTF32L,
	{ NULL, NULL, NULL },	// EncodingType_UTF32B,
	//{ NULL, NULL, NULL }	// EncodingType_UTF32LN,
	//{ NULL, NULL, NULL }	// EncodingType_UTF32BN,

	{ g_SJISLeadBytePairs, g_SJISToUTF16Table, g_UTF16ToSJISTable },		// EncodingType_SJIS,
	{ g_GB2312LeadBytePairs, g_GB2312ToUTF16Table, g_UTF16ToGB2312Table },	// EncodingType_GB2312
	{ g_EUCKRLeadBytePairs, g_EUCKRToUTF16Table, g_UTF16ToEUCKRTable },		// EncodingType_EUCKR
	{ g_Big5LeadBytePairs, g_Big5ToUTF16Table, g_UTF16ToBig5Table },		// EncodingType_BIG5
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static bool CheckDBCSLeadByte(const DBCSEncoding::TableInfo* info, byte_t byte)
{
	for (int i = 0; i < DBCSEncoding::LeadBytePairsSize / 2; ++i)
	{
		int pair = i * 2;
		if (info->LeadBytePairs[pair] == 0x00) {
			return false;
		}
		// 先行バイトの範囲内であるか
		if (info->LeadBytePairs[pair] <= byte && byte <= info->LeadBytePairs[pair + 1]) {
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DBCSEncoding::DBCSEncoding(EncodingType type)
	: m_encodingType(EncodingType_Unknown)
{
	LN_THROW(Tables[type].LeadBytePairs != NULL, ArgumentException);	// DBEncoding としては使えない
	m_encodingType = type;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DBCSEncoding::DBCSDecoder::ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	if (outBufferCharCount > 0) { outBuffer[0] = '\0'; }
	*outBytesUsed = 0;
	*outCharsUsed = 0;

	// 入力が 0 文字の場合は何もしない (変換の必要なし)
	if (inBufferByteCount == 0) { return; }

	// 変換 (もし前回のバッファ終端が先行バイトだったら、m_lastLeadByte に先行バイトが入っている)
	size_t inBufPos = 0;	// MBCS
	size_t outBufPos = 0;	// UTF16
	for (; inBufPos < inBufferByteCount; ++inBufPos)
	{
		byte_t b = inBuffer[inBufPos];

		// 先行バイト未発見状態の場合
		if (m_lastLeadByte == 0x00)
		{
			if (CheckDBCSLeadByte(m_tableInfo, b)) {
				m_lastLeadByte = b;	// 先行バイト発見状態にする
			}
			else {
				// シングルバイト文字。普通に変換する。
				outBuffer[outBufPos] = m_tableInfo->DBCSToUTF16Table[b];
				LN_THROW(outBuffer[outBufPos] != 0x0000, EncodingFallbackException);	// 不正文字
				++outBufPos;
			}
		}
		// 直前の文字が先行バイトの場合
		else
		{
			// 先行バイトが連続することもある。"＝" は 0x81 0x81 である。
			//if (CheckDBCSLeadByte(m_tableInfo, b)) {	// 先行バイトが連続で来るはず無い
			//	LN_THROW(0, EncodingFallbackException);	// 不正文字
			//}

			// マルチバイト文字。先行バイトを上位バイトにして変換する。
			outBuffer[outBufPos] = m_tableInfo->DBCSToUTF16Table[(m_lastLeadByte << 8) | (b & 0xFF)];
			LN_THROW(outBuffer[outBufPos] != 0x0000, EncodingFallbackException);	// 不正文字
			++outBufPos;
			m_lastLeadByte = 0x00;
		}
	}

	// もしバッファ終端が先行バイトであれば、この時点で m_lastLeadByte に先行バイトが入っている

	// DBCSEncoding ではサロゲートを持たないことが決まっているのでそのまま変換した文字数を出力できる。
	*outBytesUsed = outBufPos * sizeof(UTF16);	// 変換後のバイト数
	*outCharsUsed = outBufPos;					// 変換後の文字数
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DBCSEncoding::DBCSEncoder::ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed)
{
	if (outBufferByteCount > 0) { outBuffer[0] = '\0'; }
	*outBytesUsed = 0;
	*outCharsUsed = 0;

	// 入力が 0 文字の場合は何もしない (変換の必要なし)
	if (inBufferCharCount == 0) { return; }

	// 変換
	size_t inBufPos = 0;	// UTF16
	size_t outBufPos = 0;	// MBCS
	for (; inBufPos < inBufferCharCount; ++inBufPos)
	{
		UTF16 ch = inBuffer[inBufPos];

		// サロゲートはエラー
		if (UnicodeUtils::CheckUTF16HighSurrogate(ch) || UnicodeUtils::CheckUTF16LowSurrogate(ch)) {
			LN_THROW(0, EncodingFallbackException);
		}

		uint16_t dbBytes = m_tableInfo->UTF16ToDBCSTable[ch];

		// シングルバイト文字
		if ((dbBytes & 0xFF00) == 0x0000) {
			outBuffer[outBufPos] = dbBytes & 0xFF;
			++outBufPos;
		}
		// マルチバイト文字
		else {
			outBuffer[outBufPos] = ((dbBytes & 0xFF00) >> 8);
			++outBufPos;
			outBuffer[outBufPos] = (dbBytes & 0xFF);
			++outBufPos;
		}
	}

	*outBytesUsed = outBufPos;
	*outCharsUsed = inBufPos;
}

} // namespace Text
} // namespace Lumino
