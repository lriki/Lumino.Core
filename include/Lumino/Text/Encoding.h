
#pragma once
#include "../Base/NonCopyable.h"
#include "../Base/RefObject.h"

namespace Lumino
{
class RefBuffer;
class Decoder;
class Encoder;

typedef unsigned short UTF16;

enum EncodingType
{
	EncodingType_Unknown = 0,	///< �s���ȕ����R�[�h (���ʎ��s�B�܂��̓o�C�i���t�@�C��)
	EncodingType_UTF8,
	EncodingType_UTF8N,
	EncodingType_UTF16L,
	EncodingType_UTF16B,
	//EncodingType_UTF16LN,
	//EncodingType_UTF16BN,
	EncodingType_UTF32L,
	EncodingType_UTF32B,
	//EncodingType_UTF32LN,
	//EncodingType_UTF32BN,

	EncodingType_SJIS,

	EncodingType_Max,		// terminator
};

/// �����R�[�h�ϊ��̌��ʂ��󂯎�邽�߂̍\����
struct EncodingConversionResult
{
	size_t	BytesUsed;			///< �ϊ���̗L���o�C�g��
	size_t	CharsUsed;			///< �ϊ���̗L��������(�}���`�o�C�g�������l������������)
	bool	UsedDefaultChar;	///< �ϊ��s�������f�t�H���g����('?') �ɕϊ��������ǂ���
};

/**
	@brief		�����G���R�[�f�B���O
*/
class Encoding : public RefObject
{
public:
	Encoding() : mFallbackReplacementChar((uint32_t)'?') {};
	virtual ~Encoding() {};

public:

	/**
		@brief		���ˑ��̃}���`�o�C�g�����R�[�h�̃G���R�[�f�B���O���擾����B
		@details	�Ԃ����|�C���^�̓O���[�o���ȃC���X�^���X�ł��B���̃|�C���^�͉�����Ȃ��ł��������B
	*/
	static Encoding* GetSystemMultiByteEncoding();

	/**
		@brief		���C�h�����̃G���R�[�f�B���O���擾����B
		@details	�Ԃ����|�C���^�̓O���[�o���ȃC���X�^���X�ł��B���̃|�C���^�͉�����Ȃ��ł��������B
	*/
	static Encoding* GetWideCharEncoding();

	/**
		@brief		TCHAR �^�����̃G���R�[�f�B���O���擾����B
		@details	�Ԃ����|�C���^�̓O���[�o���ȃC���X�^���X�ł��B���̃|�C���^�͉�����Ȃ��ł��������B
	*/
	static Encoding* GetTCharEncoding();

	/**
		@brief		UTF-8 �G���R�[�f�B���O���擾����B
		@details	�Ԃ����|�C���^�̓O���[�o���ȃC���X�^���X�ł��B���̃|�C���^�͉�����Ȃ��ł��������B
	*/
	static Encoding* GetUTF8Encoding();

	/**
		@brief		UTF-16 (Little) �G���R�[�f�B���O���擾����B
		@details	�Ԃ����|�C���^�̓O���[�o���ȃC���X�^���X�ł��B���̃|�C���^�͉�����Ȃ��ł��������B
	*/
	static Encoding* GetUTF16Encoding();

	/**
		@brief		�����R�[�h��ϊ����� (�s���V�[�P���X�����������O)
		@param[in]	src				: �ϊ���
		@param[in]	srcByteCount	: �ϊ����̃o�C�g��
		@param[in]	srcEncoding		: �ϊ����̃G���R�[�f�B���O
		@param[in]	targetEncoding	: �ϊ���̃G���R�[�f�B���O
		@param[out]	result			: �ϊ��̌��ʏ��
		@return		\0�I�[�����͕t������܂���BGetSize() �ɂ��g�p�o�C�g�����m�F�ł��܂��B
					�g�p��ARelease() �ŊJ������K�v������܂��B
	*/
	static RefBuffer* Convert(
		const void* src, size_t srcByteCount, const Encoding* srcEncoding,
		const Encoding* targetEncoding,
		EncodingConversionResult* result);

	/**
		@brief		Encoder �� Decoder ���w�肵�ĕ����R�[�h��ϊ����� (�s���V�[�P���X�����������O)
		@param[in]	src				: �ϊ���
		@param[in]	srcByteCount	: �ϊ����̃o�C�g��
		@param[in]	decoder			: src �𒆊ԃt�H�[�}�b�g�ɕϊ�����f�R�[�_
		@param[in]	encoder			: ���ԃt�H�[�}�b�g���^�[�Q�b�g�ɕϊ�����G���R�[�_
		@param[out]	result			: �ϊ��̌��ʏ��
		@details	�w�肳�ꂽ Encoder �� Decoder �́A�ϊ��X�e�[�^�X��ێ��ł���ꍇ�͕ێ����܂��B
		@return		\0�I�[�����͕t������܂���BGetSize() �ɂ��g�p�o�C�g�����m�F�ł��܂��B
					�g�p��ARelease() �ŊJ������K�v������܂��B
	*/
	static RefBuffer* Convert(
		const void* src, size_t srcByteCount, Decoder* decoder,
		Encoder* encoder,
		EncodingConversionResult* result);

public:

	/**
		@brief		1 �����̍ŏ��o�C�g�����擾����
	*/
	virtual int GetMinByteCount() const = 0;

	/**
		@brief		1 �����̍ő�o�C�g�����擾����
	*/
	virtual int GetMaxByteCount() const = 0;
	
	/**
		@brief		�f�R�[�h�܂��̓G���R�[�h��Ƀ}�b�s���O�ł��Ȃ������������������ɒu�����镶���̐ݒ�
	*/
	virtual void SetFallbackReplacementChar(uint32_t ch) { mFallbackReplacementChar = ch; }

	/**
		@brief		�f�R�[�h�܂��̓G���R�[�h��Ƀ}�b�s���O�ł��Ȃ������������������ɒu�����镶���̎擾
	*/
	virtual uint32_t GetFallbackReplacementChar() { return mFallbackReplacementChar; }

	/**
		@brief		�o�C�g�V�[�P���X�����������֕ϊ�����f�R�[�_���쐬����
	*/
	virtual Decoder* CreateDecoder() const = 0;

	/**
		@brief		������������o�C�g�V�[�P���X�֕ϊ�����G���R�[�_���쐬����
	*/
	virtual Encoder* CreateEncoder() const = 0;

	/**
		@brief		���̃G���R�[�h�����ʂ��邽�߂̑O�������� (BOM) ���擾����
		@return		NULL �ŏI���o�C�g��BBOM �������Ȃ��G���R�[�h�̏ꍇ�� NULL
	*/
	virtual byte_t* GetPreamble() const = 0;

protected:
	uint32_t	mFallbackReplacementChar;
};




/*===============================================================================
	�ȉ��͓����p�N���X�B�V���������R�[�h�ɑΉ�����ꍇ�͊g������K�v�����邪�A
	��{�I�ɂ͈ӎ����Ȃ���OK�B
===============================================================================*/

/**
	@brief		�G���R�[�h�ς݃o�C�g�V�[�P���X������������R�[�h (UTF16) �ւ̕ϊ����s���N���X
	@details	Decoder ��1�̕�����X�g���[���̕ϊ����A�C�ӂ̉� ConvertToUTF16() ���Ăяo�����Ƃ��ł��܂��B
				���̊ԁA�ϊ��X�e�[�^�X��ۑ����܂��B
				�܂�A�o�b�t�@�����OIO���Œ���ȕ��͂𕡐��̃������o�b�t�@�ɕ������ēǂݎ��ꍇ�ɁA
				�o�b�t�@�̋��E�Ń}���`�o�C�g������������Ă��܂��Ă��A�p�����ĕϊ����s�����Ƃ��ł��܂��B

				�������A���̃X�e�[�^�X�ۑ��� CanRemain() �� true ��Ԃ��ꍇ�̂ݎg�p�\�ł��B
				false ��Ԃ��ꍇ�͂��炩���ߑS�Ẵ\�[�X�������1�̘A�������������o�b�t�@�ɓǂݍ��݁A
				ConvertToUTF16() �ɓn���K�v������܂��B

				�Q�l�F.NET Decoder.Convert()
				http://msdn.microsoft.com/ja-jp/library/twte1yfs(v=vs.110).aspx
*/
class Decoder : public RefObject
{
public:
	Decoder() : mFallbackReplacementChar(0) {}
	virtual ~Decoder() {}

public:

	/**
		@brief		1 �����̍ŏ��o�C�g�����擾����
	*/
	virtual int GetMinByteCount() = 0;

	/**
		@brief		1 �����̍ő�o�C�g�����擾����
	*/
	virtual int GetMaxByteCount() = 0;

	/**
		@brief		Decoder ���ϊ���Ԃ�ێ��ł��邩���m�F����
	*/
	virtual bool CanRemain() = 0;

	/**
		@brief		�f�R�[�h��Ƀ}�b�s���O�ł��Ȃ������������������ɒu�����镶���̐ݒ�
	*/
	virtual void SetFallbackReplacementChar(uint32_t ch) { mFallbackReplacementChar = ch; }

	/**
		@brief		�f�R�[�h��Ƀ}�b�s���O�ł��Ȃ������������������ɒu�����镶���̎擾
	*/
	virtual uint32_t GetFallbackReplacementChar() { return mFallbackReplacementChar; }

	/**
		@brief		���̃G���R�[�f�B���O�ŕ\������镶����o�b�t�@���AUTF16 �ɕϊ�����
		@param[in]	inBuffer			: ���̃G���R�[�f�B���O�ŕ\������镶����o�b�t�@
		@param[in]	inBufferByteCount	: inBuffer �̃o�C�g�� (NULL �����͊܂܂Ȃ�����)
		@param[out]	outBuffer			: �ϊ����ʂ̊i�[��o�b�t�@
		@param[in]	outBufferCharCount	: outBuffer �̃T�C�Y (�������P��)
		@param[out]	outBytesUsed		: �ϊ����ʂ̃o�C�g�����i�[����ϐ�
		@param[out]	outCharsUsed		: �ϊ����ʂ̕��������i�[����ϐ�
	*/
	virtual void ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed) = 0;

	/**
		@brief	��A�� ConvertToUTF16() �̌Ăяo���̒��ŁA�ϊ��ł��Ȃ��������K�蕶���ɕϊ��������������擾����
	*/
	virtual int UsedDefaultCharCount() = 0;

	/**
		@brief	�Ō�� ConvertToUTF16() �ŁA�o�b�t�@�����Ń}���`�o�C�g�������r�؂�Ă��Ȃ���� true ��Ԃ�
	*/
	virtual bool Completed() = 0;

	/**
		@brief	�ϊ��X�e�[�^�X���N���A���A������Ԃɖ߂�
	*/
	virtual void Reset() = 0;

protected:
	uint32_t	mFallbackReplacementChar;
};

/**
	@brief		���������R�[�h (UTF16) ���G���R�[�h����N���X
	@details	Decoder ���l�ACanRemain() �� true �̏ꍇ�͕ϊ��X�e�[�^�X��ۑ����܂��B
*/
class Encoder : public RefObject
{
public:
	Encoder() : mFallbackReplacementChar(0) {}
	virtual ~Encoder() {}

public:

	/**
		@brief		1 �����̍ŏ��o�C�g�����擾����
	*/
	virtual int GetMinByteCount() = 0;

	/**
		@brief		1 �����̍ő�o�C�g�����擾����
	*/
	virtual int GetMaxByteCount() = 0;

	/**
		@brief		�G���R�[�h��Ƀ}�b�s���O�ł��Ȃ������������������ɒu�����镶���̐ݒ�
	*/
	virtual void SetFallbackReplacementChar(uint32_t ch) { mFallbackReplacementChar = ch; }

	/**
		@brief		�G���R�[�h��Ƀ}�b�s���O�ł��Ȃ������������������ɒu�����镶���̎擾
	*/
	virtual uint32_t GetFallbackReplacementChar() { return mFallbackReplacementChar; }

	/**
		@brief	Decoder ���ϊ���Ԃ�ێ��ł��邩���m�F����
	*/
	virtual bool CanRemain() = 0;

	/**
		@brief		UTF16 ��������A���̃G���R�[�f�B���O�ŕ\������镶����ɕϊ�����
		@param[in]	inBuffer			: ���̃G���R�[�f�B���O�ŕ\������镶����o�b�t�@
		@param[in]	inBufferCharCount	: inBuffer �̃T�C�Y (�������P�ʁBNULL �����͊܂܂Ȃ�)
		@param[out]	outBuffer			: �ϊ����ʂ̊i�[��o�b�t�@
		@param[in]	outBufferByteCount	: outBuffer �̃o�C�g��
		@param[out]	outBytesUsed		: �ϊ����ʂ̃o�C�g�����i�[����ϐ�
		@param[out]	outCharsUsed		: �ϊ����ʂ̕��������i�[����ϐ�
	*/
	virtual void ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed) = 0;

	/**
		@brief	��A�� ConvertToUTF16() �̌Ăяo���̒��ŁA�ϊ��ł��Ȃ��������K�蕶���ɕϊ��������������擾����
	*/
	virtual int UsedDefaultCharCount() = 0;

	/**
		@brief	�Ō�� ConvertToUTF16() �ŁA�o�b�t�@�����Ń}���`�o�C�g�������r�؂�Ă��Ȃ���� true ��Ԃ�
	*/
	virtual bool Completed() = 0;

	/**
		@brief	�ϊ��X�e�[�^�X���N���A���A������Ԃɖ߂�
	*/
	virtual void Reset() = 0;

protected:
	uint32_t	mFallbackReplacementChar;
};


/**
	@brief		�v���O�����̃��P�[���ɍ��킹���}���`�o�C�g�R�[�h�G���R�[�f�B���O
	@details	setlocale() �ɂĎw�肳�ꂽ���P�[���̋K�蕶���R�[�h��\���܂��B
				OS �K��̃��P�[�����g�p����ɂ́A���炩���� setlocale( LC_ALL, "" ); �̂悤�ɂ���ɂ���K�v������܂��B
				�Ⴆ�΁A���{�� Windws �ł���� ShiftJIS�ALinux �ł���� UTF-8 �ƂȂ�܂��B
	@attention	��L����̒ʂ�A����̕����R�[�h�͊��ˑ��ł��B
				��{�I�� ASCII �����݂̂ō\�������G���R�[�f�B���O�ƍl����̂�����ł��B
*/
class SystemMultiByteEncoding : public Encoding
{
public:
	SystemMultiByteEncoding() {};
	virtual ~SystemMultiByteEncoding() {};

public:
	// override Encoding
	virtual int GetMinByteCount() const;
	virtual int GetMaxByteCount() const;
	virtual Decoder* CreateDecoder() const { return LN_NEW SystemMultiByteDecoder(); }
	virtual Encoder* CreateEncoder() const { return LN_NEW SystemMultiByteEncoder(); }
	virtual byte_t* GetPreamble() const { return 0; }

private:
	// Decoder
	class SystemMultiByteDecoder : public Decoder
	{
	public:
		SystemMultiByteDecoder() { Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 6; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int			mUsedDefaultCharCount;
		bool		mCompleted;
	};

	// Encoder
	class SystemMultiByteEncoder : public Encoder
	{
	public:
		SystemMultiByteEncoder() { Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 6; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int			mUsedDefaultCharCount;
		bool		mCompleted;
	};
};

/**
	@brief		UTF8 �G���R�[�f�B���O
*/
class UTF8Encoding : public Encoding
{
public:
	UTF8Encoding() {};
	virtual ~UTF8Encoding() {};

public:
	// override Encoding
	virtual int GetMinByteCount() const { return 1; }
	virtual int GetMaxByteCount() const { return 6; }
	virtual Decoder* CreateDecoder() const { return LN_NEW UTF8Decoder(); }
	virtual Encoder* CreateEncoder() const { return LN_NEW UTF8Encoder(); }
	virtual byte_t* GetPreamble() const;

private:
	// Decoder
	class UTF8Decoder : public Decoder
	{
	public:
		UTF8Decoder() { Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 6; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int			mUsedDefaultCharCount;	///< ��A�� ConvertToUTF16() �̌Ăяo���̒��ŁA�ϊ��ł��Ȃ��������K�蕶���ɕϊ�����������
		bool		mCompleted;				///< �Ō�� ConvertToUTF16() �ŁA�o�b�t�@�����Ń}���`�o�C�g�������r�؂�Ă��Ȃ���� true
	};

	// Encoder
	class UTF8Encoder : public Encoder
	{
	public:
		UTF8Encoder() { Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 6; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int			mUsedDefaultCharCount;	///< ��A�� ConvertFromUTF16() �̌Ăяo���̒��ŁA�ϊ��ł��Ȃ��������K�蕶���ɕϊ�����������
		bool		mCompleted;				///< �Ō�� ConvertFromUTF16() �ŁA�o�b�t�@�����Ń}���`�o�C�g�������r�؂�Ă��Ȃ���� true
	};
};

/**
	@brief		UTF16 �G���R�[�f�B���O
	@details	�ϊ����̃o�C�g�I�[�_�[�͋K�肵�܂���B(Big/Little �ǂ���ł�OK)
				GetPreamble() ���Ԃ� BOM �� Little �ł��B
*/
class UTF16Encoding : public Encoding
{
public:
	UTF16Encoding() {};
	virtual ~UTF16Encoding() {};

public:
	// override Encoding
	virtual int GetMinByteCount() const { return 2; }
	virtual int GetMaxByteCount() const { return 4; }
	virtual Decoder* CreateDecoder() const { return LN_NEW UTF16Decoder(); }
	virtual Encoder* CreateEncoder() const { return LN_NEW UTF16Encoder(); }
	virtual byte_t* GetPreamble() const;

private:
	// Decoder
	class UTF16Decoder : public Decoder
	{
	public:
		UTF16Decoder() { Reset(); }
		virtual int GetMinByteCount() { return 2; }
		virtual int GetMaxByteCount() { return 4; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int			mUsedDefaultCharCount;
		bool		mCompleted;
	};

	// Encoder
	class UTF16Encoder : public Encoder
	{
	public:
		UTF16Encoder() { Reset(); }
		virtual int GetMinByteCount() { return 2; }
		virtual int GetMaxByteCount() { return 4; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int			mUsedDefaultCharCount;
		bool		mCompleted;
	};
};


} // namespace Lumino
