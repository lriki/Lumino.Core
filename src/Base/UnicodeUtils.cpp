
#include <stdio.h>
#include "../../include/Lumino/Base/UnicodeUtils.h"

namespace Lumino
{

// UTF-8 �̂���擪�o�C�g�ɑ΂���1�������̌㑱�o�C�g��
// (���ݖ{�\�[�X�ł� 4 �o�C�g�܂ł��������Ȃ����A�o���Ƃ��Ďc���Ă���)
static const char TrailingBytesForUTF8[256] = 
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5,
};

// UTF-8 �� UTF-32 �̕ϊ����Ɏg�p���AUTF-8 �̃o�C�g�}�[�N��ł�����
static const UnicodeUtils::UTF32 OffsetsFromUTF8[6] = 
{ 
	0x00000000UL, 0x00003080UL, 0x000E2080UL, 
	0x03C82080UL, 0xFA082080UL, 0x82082080UL,
};

// UTF-8 �̐�s�o�C�g�ɕt���o�C�g�}�[�N
// 00000000, 00000000, 11000000, 11100000, 11110000, 11111000, 11111100
static const UnicodeUtils::UTF8 UTF8FirstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::ConvertUTF8toUTF16 (
	const UTF8*				sourceStart, 
	int						sourceLength,
	UTF16*					targetStart,
	int						targetLength,
	UTFConversionOptions*	options)
{
	UTFConversionResult result = UTFConversionResult_Success;
    const UTF8*			source = sourceStart;
	const UTF8*			sourceEnd = source + sourceLength;
    UTF16*				target = targetStart;
    UTF16*				targetEnd = target + targetLength;

	while (source < sourceEnd)
	{
		// ��x UTF32 ������
		UTF32 ch;
		result = ConvertCharUTF8toUTF32(&source, sourceEnd, options, &ch);
		if (result != UTFConversionResult_Success) {
			break;
		}

		// UTF32 ���� UTF16 �� 
		result = ConvertCharUTF32toUTF16(ch, &target, targetEnd, options);
		if (result != UTFConversionResult_Success) {
			break;
		}

		// �������J�E���g
		options->CharCount++;
	}
	
	options->ConvertedSourceLength = source - sourceStart;
	options->ConvertedTargetLength = target - targetStart;
	return result;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::ConvertUTF8toUTF32(
	const UTF8*				sourceStart,
	int						sourceLength,
	UTF32*					targetStart,
	int						targetLength,
	UTFConversionOptions*	options)
{
    UTFConversionResult	result = UTFConversionResult_Success;
    const UTF8*			source = sourceStart;
	const UTF8*			sourceEnd = source + sourceLength;
    UTF32*				target = targetStart;
    UTF32*				targetEnd = target + targetLength;

	while (source < sourceEnd)
	{
		 const UTF8* oldSource = source;	// backup

		// UTF32 ������
		UTF32 ch;
		result = ConvertCharUTF8toUTF32(&source, sourceEnd, options, &ch);
		if (result != UTFConversionResult_Success) {
			break;
		}

		// target �o�b�t�@�͈̔̓`�F�b�N
		if (target >= targetEnd) {
			source = oldSource;
			result = UTFConversionResult_TargetExhausted;
			break;
		}

		bool isIllegal = false;
		if (ch <= MaxLegalUTF32) 
		{
			// UTF16�T���Q�[�g�l�� UTF32 �ł͕s������ (��17 (>0x10FFFF) ����͕s������)
			if (SurrogateHighStart <= ch && ch <= SurrogateLowEnd)
			{
				isIllegal = true;
			}
		}
		// UTF32 �͈̔͊O
		else {
			isIllegal= true;
		}
		// �s������
		if (isIllegal)
		{
			if (IsStrictConversion(options)) {
				// �G���[�Ƃ���
				source = oldSource;
				result = UTFConversionResult_SourceIllegal;
				break;
			} 
			else {
				// �u������
				*target = options->ReplacementChar;
				target++;
				options->IllegalCharCount++;
			}
		}
		// ���K����
		else {
			*target++ = ch;
		}	

		// �������J�E���g
		options->CharCount++;
	}

	options->ConvertedSourceLength = source - sourceStart;
	options->ConvertedTargetLength = target - targetStart;
    return result;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::ConvertUTF16toUTF8(
	const UTF16*			sourceStart,	// �ϊ��ł��Ȃ����������������ꍇ�A�������w���Ă���
	int						sourceLength,
	UTF8*					targetStart, 
	int						targetLength,
	UTFConversionOptions*	options)
{
	UTFConversionResult	result = UTFConversionResult_Success;
    const UTF16*		source = sourceStart;
	const UTF16*		sourceEnd = source + sourceLength;
    UTF8*				target = targetStart;
    UTF8*				targetEnd = target + targetLength;

	while (source < sourceEnd)
	{
		const UTF16* oldSource = source;

		// ��x UTF-32 ������
		UTF32 ch;
		result = ConvertCharUTF16toUTF32(&source, sourceEnd, options, &ch);
		if (result != UTFConversionResult_Success) {
			break;
		}

		// UTF-8 ������
		result = ConvertCharUTF32toUTF8(ch, &target, targetEnd, options);
		if (result != UTFConversionResult_Success) {
			source = oldSource;
			break;
		}

		// �������J�E���g
		options->CharCount++;
	}

	options->ConvertedSourceLength = source - sourceStart;
	options->ConvertedTargetLength = target - targetStart;
	return result;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::ConvertUTF16toUTF32(
	const UTF16*			sourceStart, 
	int						sourceLength,
	UTF32*					targetStart, 
	int						targetLength,
	UTFConversionOptions*	options)
{
	UTFConversionResult	result = UTFConversionResult_Success;
    const UTF16*		source = sourceStart;
	const UTF16*		sourceEnd = source + sourceLength;
    UTF32*				target = targetStart;
    UTF32*				targetEnd = target + targetLength;

	while (source < sourceEnd)
	{
		const UTF16* oldSource = source;

		// UTF-32 ������ (��������ƁAsource �͎��̕����ֈړ�����)
		UTF32 ch;
		result = ConvertCharUTF16toUTF32(&source, sourceEnd, options, &ch);
		if (result != UTFConversionResult_Success) {
			break;
		}

		// �o�̓o�b�t�@�̃T�C�Y�`�F�b�N
		if (target >= targetEnd) {
			source = oldSource;
			result = UTFConversionResult_TargetExhausted;
			break;
		}

		// �i�[
		*target = ch;
		target++;

		// �������J�E���g
		options->CharCount++;
	}

	options->ConvertedSourceLength = source - sourceStart;
	options->ConvertedTargetLength = target - targetStart;
	return result;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::ConvertUTF32toUTF8(
	const UTF32*			sourceStart, 
	int						sourceLength,
	UTF8*					targetStart, 
	int						targetLength,
	UTFConversionOptions*	options)
{
	UTFConversionResult	result = UTFConversionResult_Success;
    const UTF32*		source = sourceStart;
	const UTF32*		sourceEnd = source + sourceLength;
    UTF8*				target = targetStart;
    UTF8*				targetEnd = target + targetLength;

    while (source < sourceEnd)
	{
		const UTF32* oldSource = source;	// backup
		UTF32 ch = *source;
		source++;

		// UTF16 �T���Q�[�g�� UTF32 �ł͕s������
		if (SurrogateHighStart <= ch && ch <= SurrogateLowEnd)
		{
			if (IsStrictConversion(options)) {	// �G���[�Ƃ���	
				source = oldSource;
				result = UTFConversionResult_SourceIllegal;
				break;
			} 
			else {								// �u������
				ch = options->ReplacementChar;
				options->IllegalCharCount++;
			}
		}

		// UTF8 �֕ϊ�
		result = ConvertCharUTF32toUTF8(ch, &target, targetEnd, options);
		if (result != UTFConversionResult_Success) {
			break;
		}

		// �������J�E���g
		options->CharCount++;
	}

	options->ConvertedSourceLength = source - sourceStart;
	options->ConvertedTargetLength = target - targetStart;
	return result;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::ConvertUTF32toUTF16(
	const UTF32*			sourceStart, 
	int						sourceLength,
	UTF16*					targetStart, 
	int						targetLength,
	UTFConversionOptions*	options)
{
	UTFConversionResult	result = UTFConversionResult_Success;
    const UTF32*		source = sourceStart;
	const UTF32*		sourceEnd = source + sourceLength;
    UTF16*				target = targetStart;
    UTF16*				targetEnd = target + targetLength;

    while (source < sourceEnd)
	{
		// �o�̓o�b�t�@�̃T�C�Y�`�F�b�N
		if (target >= targetEnd) {
			result = UTFConversionResult_TargetExhausted;
			break;
		}

		// UTF32 �ǂݎ��
		UTF32 ch = *source;
		source++;

		// UTF16 �� (��������ƁAtarget �͎��̊i�[����w��)
		result = ConvertCharUTF32toUTF16(ch, &target, targetEnd, options);
		if (result != UTFConversionResult_Success) {
			break;
		}

		// �������J�E���g
		options->CharCount++;
	}

	options->ConvertedSourceLength = source - sourceStart;
	options->ConvertedTargetLength = target - targetStart;
	return result;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::GetUTF16CharCount(
	const UTF16*			sourceStart,
	int						sourceLength,
	bool					isStrict,
	int*					count)
{
	const UTF16*		source = sourceStart;
	const UTF16*		sourceEnd = source + sourceLength;
	UTF16 ch1, ch2;
	(*count) = 0;

	while (source < sourceEnd)
	{
		ch1 = *source;
		source++;

		// �T���Q�[�g�y�A�������Ă���ꍇ
		if (SurrogateHighStart <= ch1 && ch1 <= SurrogateHighEnd)
		{
			if (source < sourceEnd)		// �o�b�t�@�I�[�`�F�b�N
			{
				ch2 = *source;	// 2�����ړǂݎ��
				// 2�����ڂ����ʃT���Q�[�g�ł���
				if (ch2 >= SurrogateLowStart && ch2 <= SurrogateLowEnd) {
					++source;
				}
				else if (isStrict) {
					// ��ʃT���Q�[�g������̂ɁA���ʃT���Q�[�g��������Ȃ�����
					return UTFConversionResult_SourceIllegal;
				}
			}
			else {
				// ���ʃT���Q�[�g��T���Ă���r���Ƀo�b�t�@�I�[�ɓ��B���Ă��܂���
				return UTFConversionResult_SourceExhausted;
			}
		}
		// �T���Q�[�g�y�A�ł͂Ȃ��ꍇ
		else
		{
			// �����Ȃ艺�ʃT���Q�[�g�l����������
			if (ch1 >= SurrogateLowStart && ch1 <= SurrogateLowEnd) {
				return UTFConversionResult_SourceIllegal;
			}
		}

		// �������J�E���g
		(*count)++;
	}

	return UTFConversionResult_Success;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::CheckUTF8TrailingBytes(const UTF8* sourceStart, const UTF8* sourceEnd, bool strict, int* outExtraByteCount)
{
	*outExtraByteCount = 0;
	int extra = TrailingBytesForUTF8[*sourceStart];

	// �͈̓`�F�b�N
	if (sourceStart + extra >= sourceEnd) {
		return UTFConversionResult_SourceExhausted;
	}

	// ����� UTF8 �����ł��邩�H
	if (strict && !IsLegalUTF8(sourceStart, extra + 1)) {
		return UTFConversionResult_SourceIllegal;
	}

	*outExtraByteCount = extra;
	return UTFConversionResult_Success;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::CheckUTF16Surrogate(const UTF16* sourceStart, const UTF16* sourceEnd, bool strict, bool* outSurrogate)
{
	*outSurrogate = false;

	UTF32 ch = *sourceStart;
	// �T���Q�[�g�y�A�������Ă���ꍇ�� UTF32 �ւ̕ϊ��������s��
	if (SurrogateHighStart <= ch && ch <= SurrogateHighEnd) 
	{
		if (sourceStart + 1 < sourceEnd)		// �o�b�t�@�I�[�`�F�b�N
		{
			UTF32 ch2 = *(sourceStart + 1);	// 2�����ړǂݎ��
			// ���ʃT���Q�[�g�`�F�b�N
			if (SurrogateLowStart <= ch2 && ch2 <= SurrogateLowEnd) {
				*outSurrogate = true;
			}
			else if (strict) {
				// ��ʃT���Q�[�g������̂ɁA���ʃT���Q�[�g��������Ȃ�����
				return UTFConversionResult_SourceIllegal;
			}
		} 
		else 
		{
			// ���ʃT���Q�[�g��T���Ă���r���Ƀo�b�t�@�I�[�ɓ��B���Ă��܂���
			return UTFConversionResult_SourceExhausted;
		}
	}
	// �����Ȃ艺�ʃT���Q�[�g�l����������
	else if (strict && ch >= SurrogateLowStart && ch <= SurrogateLowEnd)
	{
		return UTFConversionResult_SourceIllegal;
	}
	return UTFConversionResult_Success;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::ConvertCharUTF8toUTF32(
	const UTF8**			sourceStart,
	const UTF8*				sourceEnd,
	UTFConversionOptions*	options,
	UTF32*					outChar)
{
	const UTF8* source = *sourceStart;
	UTF32 ch = 0;

	// �ǉ��œǂރo�C�g��
	unsigned short extraBytesToRead = TrailingBytesForUTF8[*source];
	if (source + extraBytesToRead >= sourceEnd) {
		return UTFConversionResult_SourceExhausted;
	}
	// ���K�� UTF-8 �����̃o�C�g�V�[�P���X�ł��邩���m�F
	if (!IsLegalUTF8(source, extraBytesToRead+1)) {
		return UTFConversionResult_SourceIllegal;
	}
	// �ǉ��o�C�g���ɉ����ăt�H�[���X���[�ŗ���ǂ�ł���
	switch (extraBytesToRead)
	{
		case 5: ch += *source++; ch <<= 6;	// ����
		case 4: ch += *source++; ch <<= 6;	// ����
		case 3: ch += *source++; ch <<= 6;
		case 2: ch += *source++; ch <<= 6;
		case 1: ch += *source++; ch <<= 6;
		case 0: ch += *source++;
	}
	ch -= OffsetsFromUTF8[extraBytesToRead];

	*sourceStart = source;
	*outChar = ch;
	return UTFConversionResult_Success;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::ConvertCharUTF32toUTF8(
	UTF32					ch,
	UTF8**					targetStart, // ���ۂɂ�����炸�A�����̂ЂƂO�܂ŕ�����������Ă���B
	UTF8*					targetEnd, 
	UTFConversionOptions*	options)
{
	UTFConversionResult result = UTFConversionResult_Success;
	UTF8*				target = *targetStart;

	// �ϊ���A���o�C�g�K�v��
	unsigned short bytesToWrite = 0;
	if (ch < (UTF32)0x80)			{ bytesToWrite = 1; }
	else if (ch < (UTF32)0x800)	{ bytesToWrite = 2; }
	else if (ch < (UTF32)0x10000)	{ bytesToWrite = 3; }
	else if (ch <= MaxLegalUTF32)	{ bytesToWrite = 4; }
	else 
	{
		// �s����������������
		bytesToWrite = 3;
		if (IsStrictConversion(options)) {
			return UTFConversionResult_SourceIllegal;
		}
		else {
			ch = options->ReplacementChar;
			options->IllegalCharCount++;
		}
	}

	if (target + bytesToWrite > targetEnd) {
		return UTFConversionResult_TargetExhausted;
	}
	else
	{
		const UTF32 byteMask = 0xBF;
		const UTF32 byteMark = 0x80; 

		// target �͏������ݔ͈͂̈�Ԍ����w���Ă����A
		// switch �t�H�[���X���[�Ō�납��O�֕�������������ł���
		target += bytesToWrite;
		switch (bytesToWrite)
		{
		case 4:
			target[-1] = (UTF8)((ch | byteMark) & byteMask); 
			target--;
			ch >>= 6;
		case 3:
			target[-1] = (UTF8)((ch | byteMark) & byteMask); 
			target--;
			ch >>= 6;
		case 2:
			target[-1] = (UTF8)((ch | byteMark) & byteMask); 
			target--;
			ch >>= 6;
		case 1:
			target[-1] = (UTF8)(ch | UTF8FirstByteMark[bytesToWrite]);
			target--;
		}
		target += bytesToWrite;
	}

    *targetStart = target;
	return result;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::ConvertCharUTF16toUTF32(
	const UTF16**			sourceStart,	// ���s�����ꍇ�͕ω����Ȃ�
	const UTF16*			sourceEnd,
	UTFConversionOptions*	options,
	UTF32*					outChar)
{
	UTFConversionResult	result = UTFConversionResult_Success;
	const UTF16*		source = *sourceStart;
	UTF32				ch, ch2;

	ch = *source;
	source++;

	// �T���Q�[�g�y�A�������Ă���ꍇ�� UTF32 �ւ̕ϊ��������s��
	if (SurrogateHighStart <= ch && ch <= SurrogateHighEnd) 
	{
		if (source < sourceEnd)		// �o�b�t�@�I�[�`�F�b�N
		{
			ch2 = *source;	// 2�����ړǂݎ��
			// 2�����ڂ����ʃT���Q�[�g�ł���΁AUTF-32�ɕϊ�����
			if (ch2 >= SurrogateLowStart && ch2 <= SurrogateLowEnd) {
				ch = ((ch - SurrogateHighStart) << UTF16HalfShift) + (ch2 - SurrogateLowStart) + UTF16HalfBase;
				++source;
			}
			else if (IsStrictConversion(options)) {
				// ��ʃT���Q�[�g������̂ɁA���ʃT���Q�[�g��������Ȃ�����
				result = UTFConversionResult_SourceIllegal;
			}
		} 
		else 
		{
			// ���ʃT���Q�[�g��T���Ă���r���Ƀo�b�t�@�I�[�ɓ��B���Ă��܂���
			result = UTFConversionResult_SourceExhausted;
		}
	}
	// �T���Q�[�g�y�A�ł͂Ȃ��ꍇ
	else
	{
		// �����Ȃ艺�ʃT���Q�[�g�l����������
		if (ch >= SurrogateLowStart && ch <= SurrogateLowEnd) {
			result = UTFConversionResult_SourceIllegal;
		}
	}
	
	// �������Ă���΂��ꂼ��X�V (���s�����ꍇ�AsourceStart �͕s���������w���Ă���)
	if (result == UTFConversionResult_Success) {
		*sourceStart = source;
		*outChar = ch;	
	}
	return result;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
UTFConversionResult UnicodeUtils::ConvertCharUTF32toUTF16(
	UTF32					ch,
	UTF16**					targetStart, 
	UTF16*					targetEnd, 
	UTFConversionOptions*	options)
{
	UTF16* target = *targetStart;

	// 0xFFFF �ȉ��B���ʂ� UTF-16 �\������
	if (ch <= MaxBMP) 
	{
		// UTF-16�̃T���Q�[�g�́AUTF-32�ł͖����ȕ����ł���
		if (SurrogateHighStart <= ch && ch <= SurrogateLowEnd) 
		{
			// �s�������̏���
			if (IsStrictConversion(options)) {
				return UTFConversionResult_SourceExhausted;
			} else {
				*target = (UTF16)options->ReplacementChar;
				target++;
				options->IllegalCharCount++;
			}
		} 
		else 
		{
			// �T���Q�[�g�łȂ���Ε��ʂɊi�[
			*target = (UTF16)ch;
			target++;
		}
	}
	// �K�i�O
	else if (ch > MaxLegalUTF32) 
	{
		// �s�������̏���
		if (IsStrictConversion(options)) {
			return UTFConversionResult_SourceExhausted;
		} else {
			*target = (UTF16)options->ReplacementChar;
			target++;
			options->IllegalCharCount++;
		}
	}
	// 0xFFFF �` 0x10FFFF �̊� (�T���Q�[�g�g�p)
	else 
	{
		if (target + 1 >= targetEnd) {
			return UTFConversionResult_TargetExhausted;	// �o�b�t�@�I�[�o�[
		}
		ch -= UTF16HalfBase;
		*target = (UTF16)((ch >> UTF16HalfShift) + SurrogateHighStart);
		target++;
		*target = (UTF16)((ch & UTF16HalfMask) + SurrogateLowStart);
		target++;
	}

	*targetStart = target;
	return UTFConversionResult_Success;
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
bool UnicodeUtils::IsLegalUTF8(const UTF8 *source, int length) 
{
	// length �� UTF-8 �̐�s�o�C�g�ɂ���Ď��O�Ɍ��肳���B
	// length > 4 �̏ꍇ�� false ��Ԃ��B

    UTF8 a;

	// �ŏ��Ɉ�Ԍ����w���A�t�H�[���X���[�łЂƂ��O�֌��Ă���
    const UTF8 *srcptr = source+length;
    switch (length) 
	{
		default: return false;
		case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
		case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
		case 2: if ((a = (*--srcptr)) > 0xBF) return false;

		switch (*source) 
		{
			// �����̓t�H�[���X���[���Ȃ��̂Œ���
			case 0xE0: if (a < 0xA0) return false; break;
			case 0xED: if (a > 0x9F) return false; break;
			case 0xF0: if (a < 0x90) return false; break;
			case 0xF4: if (a > 0x8F) return false; break;
			default:   if (a < 0x80) return false;
		}

		case 1: if (*source >= 0x80 && *source < 0xC2) return false;
    }
    if (*source > 0xF4) return false;
    return true;
}

} // namespace Lumino
