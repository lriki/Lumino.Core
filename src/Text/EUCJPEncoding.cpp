/*

	
	EUC-JP
		00-7F            - ASCII
		80-8D & 90-9F    - Control.
		A1-FE, A1-FE     - 2 byte JIS X 0208 (SJIS �Ɋ܂܂��)
		8E, A1-DF        - 2 byte ���p�J�^�J�i
		8F, A1-FE, A1-FE - 3 byte JIX X 0212 (�⏕�����BSJIS �ɂ͊܂܂�Ȃ��BUnicode�ɂ͊܂܂��)


	ConvertTable_EUCJP_EXT �̃e�[�u���ɃA�N�Z�X����ɂ́A�⏕�����̐�s�o�C�g 0x8F ����菜���A
	���2�o�C�g�͂��ꂼ�� -0x80 ����B

*/
#include "../Internal.h"
#include "../../include/Lumino/Text/UnicodeUtils.h"
#include "EUCJPEncoding.h"

LN_NAMESPACE_BEGIN

// EUCGroup �̊e�v�ɂ��āA�ǉ��œǂޕK�v������o�C�g��
static int g_eucLeadExtraLength[] =
{
	0, 0, 1, 1, 2, 0,	
};

// SJIS
extern "C" const unsigned char g_SJISLeadBytePairs[];
extern "C" const unsigned short g_SJISToUTF16Table[];
extern "C" const unsigned short g_UTF16ToSJISTable[];

// EUC Ext
extern "C" const unsigned short g_EUCJPEXTToUTF16Table[];
extern "C" const unsigned short g_UTF16ToEUCJPEXTTable[];

// �_�u���o�C�g JIS ������ SJIS �����֕ϊ�����
static inline uint16_t ConvertDB_JISToSJIS(uint16_t jis)
{
	byte_t hi = (jis >> 8) & 0xff;
	byte_t low = jis & 0xff;
	if (hi < 0x21 || hi > 0x7e || low < 0x21 || low > 0x7e) {
		return 0;
	}

	if (hi & 0x01) {
		if (low <= 0x5f) {
			low += 0x1f;
		}
		else {
			low += 0x20;
		}
	}
	else {
		low += 0x7e;
	}

	hi = ((hi - 0x21) >> 1) + 0x81;
	if (hi > 0x9f) {
		hi += 0x40;
	}

	return (hi << 8) | low;
}

// �_�u���o�C�g SJIS ������ JIS �����֕ϊ�����
static inline uint16_t ConvertDB_SJISToJIS(uint16_t sjis)
{
	byte_t hi = (sjis >> 8) & 0xff;
	byte_t low = sjis & 0xff;
	uint16_t jis;

	hi -= (hi >= 0xa0) ? 0xc1 : 0x81;
	if (low >= 0x9f) {
		jis = (hi << 9) + 0x2200;
		jis |= low - 0x7e;
	}
	else {
		jis = (hi << 9) + 0x2100;
		jis |= low - ((low <= 0x7e) ? 0x1f : 0x20);
	}

	if ((jis > 0x7E7E) || (jis < 0x2121) || ((jis & 0xFF) > 0x7E) || ((jis & 0xFF) < 0x21)) {
		return 0;
	}
	return jis;
}

//==============================================================================
// EUCJPEncoding
//==============================================================================

//------------------------------------------------------------------------------
EUCJPEncoding::EUCJPEncoding()
{
}

//------------------------------------------------------------------------------
EUCJPEncoding::~EUCJPEncoding()
{
}

//------------------------------------------------------------------------------
inline EUCJPEncoding::EUCGroup EUCJPEncoding::CheckEUCGroup(const byte_t* pos, int len)
{
	// ASCII
	if ((len >= 1) &&
		(0x00 <= pos[0] && pos[0] <= 0x7F)) {
		return EUCGroup_ASCII;
	}
	// �}���`�o�C�g����
	if ((len >= 2) &&
		(0xA1 <= pos[0] && pos[0] <= 0xFE) &&
		(0xA1 <= pos[1] && pos[1] <= 0xFE)) {
		return EUCGroup_MultiByte;
	}
	// 2Byte ���p�J�^�J�i
	if ((len >= 2) &&
		(pos[0] == 0x8E) &&
		(0xA1 <= pos[1] && pos[1] <= 0xDF)) {
		return EUCGroup_Katakana2Byte;
	}
	// 3Byte �⏕����
	if ((len >= 3) &&
		(pos[0] == 0x8F) &&
		(0xA1 <= pos[1] && pos[1] <= 0xFE) &&
		(0xA1 <= pos[2] && pos[2] <= 0xFE)) {
		return EUCGroup_Kanji3Byte;
	}
	// Control (�o���p�x�͒Ⴂ�̂Ŕ��ʂ���񂵂ɂ��đ��x�����_��)
	if ((len >= 1) &&
		(0x80 <= pos[0] && pos[0] <= 0x8D) || (0x90 <= pos[0] && pos[0] <= 0x9F)) {
		return EUCGroup_Control;
	}
	// �s������
	return EUCGroup_Unknown;
}

//------------------------------------------------------------------------------
int EUCJPEncoding::GetCharacterCount(const void* buffer, size_t bufferSize) const
{
	int count = 0;
	const byte_t* pos = (const byte_t*)buffer;
	const byte_t* end = pos + bufferSize;
	while (pos < end)
	{
		pos += g_eucLeadExtraLength[CheckEUCGroup(pos, end - pos)];
		pos++;
		count++;
	}
	return count;
}

//------------------------------------------------------------------------------
int EUCJPEncoding::GetLeadExtraLength(const void* buffer, size_t bufferSize) const
{
	const byte_t* pos = (const byte_t*)buffer;
	const byte_t* end = pos + bufferSize;
	return g_eucLeadExtraLength[CheckEUCGroup(pos, end - pos)];
}

//==============================================================================
// EUCJPDecoder
//==============================================================================
//------------------------------------------------------------------------------
void EUCJPEncoding::EUCJPDecoder::ConvertToUTF16(const byte_t* input, size_t inputByteSize, UTF16* output, size_t outputElementSize, size_t* outBytesUsed, size_t* outCharsUsed)
{
	const byte_t* srcPos = input;
	const byte_t* srcEnd = input + inputByteSize;
	UTF16* dstPos = output;
	UTF16* dstEnd = dstPos + outputElementSize;

	while (srcPos < srcEnd)
	{
		// �R�[�h�|�C���g�̎�ނ𒲂ׂ�
		if (m_currentGroup == EUCGroup_Unknown)
		{
			m_currentGroup = CheckEUCGroup(srcPos, srcEnd - srcPos);
		}

		switch (m_currentGroup)
		{
		default:
		case EUCGroup_ASCII:
		case EUCGroup_Control:
			// ASCII, Control �͂��̂܂܃R�s�[���邾����OK
			dstPos[0] = srcPos[0];
			dstPos++;
			m_currentGroup = EUCGroup_Unknown;
			break;
		case EUCGroup_MultiByte:
			// �}���`�o�C�g�����̏ꍇ�͎��̃o�C�g������܂Ő�s�o�C�g��ۑ����đ҂�
			if (m_lastLeadBytesCount == 0)
			{
				m_lastLeadBytes[0] = srcPos[0];	// ��s�o�C�g�ۑ�
				++m_lastLeadBytesCount;
			}
			else
			{
				byte_t b1 = m_lastLeadBytes[0] & 0x7f;
				byte_t b2 = srcPos[0] & 0x7f;
				dstPos[0] = g_SJISToUTF16Table[ConvertDB_JISToSJIS((b1 << 8) | b2)];
				dstPos++;
				m_lastLeadBytesCount = 0;
				m_currentGroup = EUCGroup_Unknown;
			}
			break;
		case EUCGroup_Katakana2Byte:
			// ���p�J�^�J�i�̏ꍇ�͎��̃o�C�g������܂Ő�s�o�C�g��ۑ����đ҂�
			if (m_lastLeadBytesCount == 0)
			{
				m_lastLeadBytes[0] = srcPos[0];	// ��s�o�C�g�ۑ�
				++m_lastLeadBytesCount;
			}
			else
			{
				dstPos[0] = g_SJISToUTF16Table[srcPos[0] & 0xFF];	// ��s�o�C�g�������� SJIS��UTF16 �e�[�u���ɂ�����
				dstPos++;
				m_lastLeadBytesCount = 0;
				m_currentGroup = EUCGroup_Unknown;
			}
			break;
		case EUCGroup_Kanji3Byte:
			// �⏕�����̏ꍇ�͐�s�o�C�g��2����܂ő҂�
			if (m_lastLeadBytesCount <= 1)
			{
				m_lastLeadBytes[m_lastLeadBytesCount] = srcPos[0];	// ��s�o�C�g�ۑ�
				++m_lastLeadBytesCount;
			}
			else
			{
				byte_t b1 = m_lastLeadBytes[1] & 0x7f;
				byte_t b2 = srcPos[0] & 0x7f;
				dstPos[0] = g_EUCJPEXTToUTF16Table[(b1 << 8) | b2];
				dstPos++;
				m_lastLeadBytesCount = 0;
				m_currentGroup = EUCGroup_Unknown;
			}
			break;
		}

		++srcPos;
	}

	*outBytesUsed = srcEnd - input;
	*outCharsUsed = dstEnd - output;	// �T���Q�[�g�͖����̂ŃR���ŗǂ�
}

//==============================================================================
// EUCJPEncoder
//==============================================================================
//------------------------------------------------------------------------------
void EUCJPEncoding::EUCJPEncoder::ConvertFromUTF16(const UTF16* input, size_t inputElementSize, byte_t* output, size_t outputByteSize, size_t* outBytesUsed, size_t* outCharsUsed)
{
	// ���͂� 0 �����̏ꍇ�͉������Ȃ� (�ϊ��̕K�v�Ȃ�)
	if (inputElementSize == 0) { return; }

	// �ϊ�
	size_t inBufPos = 0;	// UTF16
	size_t outBufPos = 0;	// MBCS
	for (; inBufPos < inputElementSize; ++inBufPos)
	{
		UTF16 ch = input[inBufPos];

		// �T���Q�[�g�̓G���[
		if (UnicodeUtils::CheckUTF16HighSurrogate(ch) || UnicodeUtils::CheckUTF16LowSurrogate(ch)) {
			LN_THROW(0, EncodingException);
		}

		// 3Byte �⏕�������H
		if (g_UTF16ToEUCJPEXTTable[ch] != 0x0000)
		{
			byte_t hi  = (g_UTF16ToEUCJPEXTTable[ch] >> 8) & 0xFF;
			byte_t low = g_UTF16ToEUCJPEXTTable[ch] & 0xFF;
			output[outBufPos] = 0x8F;
			++outBufPos;
			output[outBufPos] = hi | 0x80;
			++outBufPos;
			output[outBufPos] = low | 0x80;
			++outBufPos;
		}
		// 2Byte �ȉ�����
		else
		{
			uint16_t sjis = g_UTF16ToSJISTable[ch];

			// ���p�J�^�J�i
			if (0xA0 <= sjis && sjis <= 0xDF)
			{
				output[0] = 0x8E;
				output[1] = sjis & 0xFF;
				outBufPos += 2;
			}
			// �V���O���o�C�g����
			else if ((sjis & 0xFF00) == 0x0000)
			{
				output[outBufPos] = sjis & 0xFF;
				++outBufPos;
			}
			// �}���`�o�C�g����
			else
			{
				uint16_t jis = ConvertDB_SJISToJIS(sjis);
				byte_t b1 = (jis >> 8) & 0xFF;
				byte_t b2 = jis & 0xFF;

				output[outBufPos] = b1 | 0x80;
				++outBufPos;
				output[outBufPos] = b2 | 0x80;
				++outBufPos;
			}
		}
	}

	*outBytesUsed = outBufPos;
	*outCharsUsed = inBufPos;
}

LN_NAMESPACE_END
