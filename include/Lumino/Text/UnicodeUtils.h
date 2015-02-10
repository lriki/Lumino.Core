#pragma once

#ifndef _WIN32
#include <stdint.h>
#endif

namespace Lumino
{
namespace Text
{

/// �����R�[�h�ϊ��̏�������
enum UTFConversionResult
{
	UTFConversionResult_Success = 0,		///< �ϊ�����
	UTFConversionResult_SourceExhausted,	///< �\�[�X�o�b�t�@���s�� (�o�b�t�@�I�[�Ń}���`�o�C�g�������r�؂ꂽ)
	UTFConversionResult_TargetExhausted,	///< �^�[�Q�b�g�o�b�t�@������Ȃ�
	UTFConversionResult_SourceIllegal,		///< �\�[�X�o�b�t�@���ɕs���ȕ������������� (UTF8�̕s���o�C�g�V�[�P���X��AUTF16�̃T���Q�[�g�y�A�s��v(��ʂ�����̂ɉ��ʂ�����)�̎��́A�s�������u����L���ɂ��Ă��Ă����̃G���[�Ŏ��s����)
};

/// �����R�[�h�ϊ��̒ǉ����
struct UTFConversionOptions
{
	unsigned long	ReplacementChar;		///< [in] �s�����������������A���̕����ɒu������ (ASCII �Ŏw�肷�邱�ƁB'\0' ���w�肵����Ԃŕs��������������ƁA�ϊ��͎��s����)

	int				ConvertedSourceLength;	///< [out] �ϊ��Ɏg�p�����\�[�X�o�b�t�@�̗v�f�����i�[����� (�o�C�g���╶�����ł͂Ȃ�)
	int				ConvertedTargetLength;	///< [out] �ϊ����ꂽ�^�[�Q�b�g�o�b�t�@�̗v�f�����i�[����� (�o�C�g���╶�����ł͂Ȃ�)
	int				CharCount;				///< [out] ���������i�[����� (�}���`�o�C�g������1�����Ƃ݂Ȃ���������)
	int				IllegalCharCount;		///< [out] �������s�������̐�
};

/**
	@brief		Unicode �̑��ݕϊ������s�����W���[��
	@details	UTF-8 �͍ő� 4 �o�C�g�Ƃ��Ĉ����܂��B
*/
class UnicodeUtils
{
public:
#ifdef _WIN32
	typedef unsigned long	UTF32;
	typedef unsigned short	UTF16;
	typedef unsigned char	UTF8;
#else
	typedef uint32_t		UTF32;
	typedef uint16_t		UTF16;
	typedef uint8_t			UTF8;
#endif

	static const UTF32	SurrogateHighStart	= (UTF32)0xD800;	///< ��ʃT���Q�[�g�l�͈̔� min
	static const UTF32	SurrogateHighEnd	= (UTF32)0xDBFF;	///< ��ʃT���Q�[�g�l�͈̔� max
	static const UTF32	SurrogateLowStart	= (UTF32)0xDC00;	///< ���ʃT���Q�[�g�l�͈̔� min
	static const UTF32	SurrogateLowEnd		= (UTF32)0xDFFF;	///< ���ʃT���Q�[�g�l�͈̔� max
	static const int	UTF16HalfShift		= 10;				///< UTF16��ʃT���Q�[�g�Ɏg�p����V�t�g��
	static const UTF32	UTF16HalfBase		= 0x0010000UL;		///< UTF16���ʃT���Q�[�g�Ɏg�p����x�[�X�l
	static const UTF32	UTF16HalfMask		= 0x3FFUL;			///< UTF32��UTF16�ϊ����́A���ʃ}�X�N
	static const UTF32	MaxLegalUTF32		= 0x0010FFFF;		///< ���̒l�ȉ��� UTF32�Ƃ��ėL��
	static const UTF32	ReplacementChar		= 0x0000FFFD;		///< �s�������������������A����ɒu������
	static const UTF32	MaxBMP				= 0x0000FFFF;		///< Basic Multilingual Plane

public:

	/**
		@brief			UTF8 ���� UTF16 �ւ̕ϊ�
		@param[in]		sourceStart		: �ϊ����̃o�b�t�@�̐擪�A�h���X
		@param[in]		sourceLength	: �ϊ����̃o�b�t�@�̒��� (�����P��(sourceStart�̗v�f��)�B�I�[ \0 �͊܂܂Ȃ��悤�ɂ��邱��)
		@param[in]		targetStart		: �ϊ����ʂ��i�[����o�b�t�@�̐擪�A�h���X (�I�[ \0 �͊i�[����Ȃ�)
		@param[in]		targetLength	: �ϊ����ʂ��i�[����o�b�t�@�̒��� (�����P��(targetStart�̗v�f��))
		@param[in,out]	options			: �ϊ��̒ǉ����
	*/
	static UTFConversionResult ConvertUTF8toUTF16(
		const UTF8*				sourceStart, 
		int						sourceLength,		// �I�[ \0 ���͊܂܂Ȃ�
		UTF16*					targetStart,
		int						targetLength,
		UTFConversionOptions*	options);

	/**
		@brief			UTF8 ���� UTF32 �ւ̕ϊ�
		@param[in]		sourceStart		: �ϊ����̃o�b�t�@�̐擪�A�h���X
		@param[in]		sourceLength	: �ϊ����̃o�b�t�@�̒��� (�����P��(sourceStart�̗v�f��)�B�I�[ \0 �͊܂܂Ȃ��悤�ɂ��邱��)
		@param[in]		targetStart		: �ϊ����ʂ��i�[����o�b�t�@�̐擪�A�h���X (�I�[ \0 �͊i�[����Ȃ�)
		@param[in]		targetLength	: �ϊ����ʂ��i�[����o�b�t�@�̒��� (�����P��(targetStart�̗v�f��))
		@param[in,out]	options			: �ϊ��̒ǉ����
	*/
	static UTFConversionResult ConvertUTF8toUTF32(
		const UTF8*				sourceStart,
		int						sourceLength,
		UTF32*					targetStart,
		int						targetLength,
		UTFConversionOptions*	options);

	/**
		@brief			UTF16 ���� UTF8 �ւ̕ϊ�
		@param[in]		sourceStart		: �ϊ����̃o�b�t�@�̐擪�A�h���X
		@param[in]		sourceLength	: �ϊ����̃o�b�t�@�̒��� (�����P��(sourceStart�̗v�f��)�B�I�[ \0 �͊܂܂Ȃ��悤�ɂ��邱��)
		@param[in]		targetStart		: �ϊ����ʂ��i�[����o�b�t�@�̐擪�A�h���X (�I�[ \0 �͊i�[����Ȃ�)
		@param[in]		targetLength	: �ϊ����ʂ��i�[����o�b�t�@�̒��� (�����P��(targetStart�̗v�f��))
		@param[in,out]	options			: �ϊ��̒ǉ����
	*/
	static UTFConversionResult ConvertUTF16toUTF8(
		const UTF16*			sourceStart, 
		int						sourceLength, 
		UTF8*					targetStart, 
		int						targetLength,
		UTFConversionOptions*	options);

	/**
		@brief			UTF16 ���� UTF32 �ւ̕ϊ�
		@param[in]		sourceStart		: �ϊ����̃o�b�t�@�̐擪�A�h���X
		@param[in]		sourceLength	: �ϊ����̃o�b�t�@�̒��� (�����P��(sourceStart�̗v�f��)�B�I�[ \0 �͊܂܂Ȃ��悤�ɂ��邱��)
		@param[in]		targetStart		: �ϊ����ʂ��i�[����o�b�t�@�̐擪�A�h���X (�I�[ \0 �͊i�[����Ȃ�)
		@param[in]		targetLength	: �ϊ����ʂ��i�[����o�b�t�@�̒��� (�����P��(targetStart�̗v�f��))
		@param[in,out]	options			: �ϊ��̒ǉ����
	*/
	static UTFConversionResult ConvertUTF16toUTF32(
		const UTF16*			sourceStart, 
		int						sourceLength, 
		UTF32*					targetStart, 
		int						targetLength,
		UTFConversionOptions*	options);

	/**
		@brief			UTF32 ���� UTF8 �ւ̕ϊ�
		@param[in]		sourceStart		: �ϊ����̃o�b�t�@�̐擪�A�h���X
		@param[in]		sourceLength	: �ϊ����̃o�b�t�@�̒��� (�����P��(sourceStart�̗v�f��)�B�I�[ \0 �͊܂܂Ȃ��悤�ɂ��邱��)
		@param[in]		targetStart		: �ϊ����ʂ��i�[����o�b�t�@�̐擪�A�h���X (�I�[ \0 �͊i�[����Ȃ�)
		@param[in]		targetLength	: �ϊ����ʂ��i�[����o�b�t�@�̒��� (�����P��(targetStart�̗v�f��))
		@param[in,out]	options			: �ϊ��̒ǉ����
	*/
	static UTFConversionResult ConvertUTF32toUTF8(
		const UTF32*			sourceStart, 
		int						sourceLength, 
		UTF8*					targetStart, 
		int						targetLength,
		UTFConversionOptions*	options);

	/**
		@brief			UTF32 ���� UTF16 �ւ̕ϊ�
		@param[in]		sourceStart		: �ϊ����̃o�b�t�@�̐擪�A�h���X
		@param[in]		sourceLength	: �ϊ����̃o�b�t�@�̒��� (�����P��(sourceStart�̗v�f��)�B�I�[ \0 �͊܂܂Ȃ��悤�ɂ��邱��)
		@param[in]		targetStart		: �ϊ����ʂ��i�[����o�b�t�@�̐擪�A�h���X (�I�[ \0 �͊i�[����Ȃ�)
		@param[in]		targetLength	: �ϊ����ʂ��i�[����o�b�t�@�̒��� (�����P��(targetStart�̗v�f��))
		@param[in,out]	options			: �ϊ��̒ǉ����
	*/
	static UTFConversionResult ConvertUTF32toUTF16(
		const UTF32*			sourceStart, 
		int						sourceLength, 
		UTF16*					targetStart, 
		int						targetLength,
		UTFConversionOptions*	options);

	/**
		@brief			UTF16 �̃T���Q�[�g�y�A���l���������������擾����
		@param[in]		sourceStart		: �J�E���g���̃o�b�t�@�̐擪�A�h���X
		@param[in]		sourceLength	: �J�E���g���̃o�b�t�@�̒��� (�����P��(sourceStart�̗v�f��)�B�I�[ \0 �͊܂܂Ȃ��悤�ɂ��邱��)
	*/
	static UTFConversionResult GetUTF16CharCount(
		const UTF16*			sourceStart,
		int						sourceLength,
		bool					isStrict,
		int*					count);

	/**
		@brief			UTF8 �����̌㑱�o�C�g�����擾����
		@param[in]		sourceStart			: �`�F�b�N�Ώۂ̐擪�����̃A�h���X
		@param[in]		sourceEnd			: �o�b�t�@�̏I�[�����̃A�h���X (�L���ȗ̈�̎��̃A�h���X���w�� (\0 �I�[������Ȃ� \0 ���w��))
		@param[in]		strict				: true �̏ꍇ�A�s�������̃`�F�b�N���s�� (�s�����������������ꍇ�� UTFConversionResult_SourceIllegal ��Ԃ�)
		@param[out]		outExtraByteCount	: �㑱�o�C�g�� (ASCII �������A�㑱�������ꍇ�� 0 ���i�[�����)
	*/
	static UTFConversionResult CheckUTF8TrailingBytes(const UTF8* sourceStart, const UTF8* sourceEnd, bool strict, int* outExtraByteCount);

	/**
		@brief			UTF16 �̃T���Q�[�g�y�A�ł��邩���m�F����
		@param[in]		sourceStart		: �`�F�b�N�Ώۂ̐擪�����̃A�h���X
		@param[in]		sourceEnd		: �o�b�t�@�̏I�[�����̃A�h���X (�L���ȗ̈�̎��̃A�h���X���w�� (\0 �I�[������Ȃ� \0 ���w��))
		@param[in]		strict			: true �̏ꍇ�A�s�������̃`�F�b�N���s�� (�s�����������������ꍇ�� UTFConversionResult_SourceIllegal ��Ԃ�)
		@param[out]		outSurrogate	: true ���i�[���ꂽ�ꍇ�AsourceStart �� ���̎��̕����̓T���Q�[�g�y�A�ł���
	*/
	static UTFConversionResult CheckUTF16Surrogate(const UTF16* sourceStart, const UTF16* sourceEnd, bool strict, bool* outSurrogate);

	/// ���[�h���AUTF16 �̏�ʃT���Q�[�g�ł��邩���m�F����
	static bool CheckUTF16HighSurrogate(UTF16 ch) { return (SurrogateHighStart <= ch && ch <= SurrogateHighEnd); }

	/// ���[�h���AUTF16 �̉��ʃT���Q�[�g�ł��邩���m�F����
	static bool CheckUTF16LowSurrogate(UTF16 ch) { return (SurrogateLowStart <= ch && ch <= SurrogateLowEnd); }

private:

	/// 1�������̕ϊ� (UTF8 �� UTF32)
	static UTFConversionResult ConvertCharUTF8toUTF32(
		const UTF8**			sourceStart,
		const UTF8*				sourceEnd,
		UTFConversionOptions*	options,
		UTF32*					outChar);

	/// 1�������̕ϊ� (UTF32 �� UTF8)
	static UTFConversionResult ConvertCharUTF32toUTF8(
		UTF32					ch,
		UTF8**					targetStart, 
		UTF8*					targetEnd,
		UTFConversionOptions*	options);

	/// 1�������̕ϊ� (UTF16 �� UTF32)
	static UTFConversionResult ConvertCharUTF16toUTF32(
		const UTF16**			sourceStart,
		const UTF16*			sourceEnd,
		UTFConversionOptions*	options,
		UTF32*					outChar);

	/// 1�������̕ϊ� (UTF32 �� UTF16)
	static UTFConversionResult ConvertCharUTF32toUTF16(
		UTF32					ch,
		UTF16**					targetStart, 
		UTF16*					targetEnd, 
		UTFConversionOptions*	options);

	/// ���K�� UTF8 �o�C�g�񂩂��`�F�b�N����
	static bool IsLegalUTF8(const UTF8 *source, int length);

	/// �s�����������e���邩�H
	static bool IsStrictConversion(const UTFConversionOptions* options) { return (options->ReplacementChar == 0); }

};

} // namespace Text
} // namespace Lumino
