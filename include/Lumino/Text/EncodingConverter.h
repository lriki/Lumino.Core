
#pragma once
#include "../Base/ByteBuffer.h"
#include "Encoding.h"

LN_NAMESPACE_BEGIN

/**
	@brief		�e�L�X�g�Ԃ̃G���R�[�f�B���O�̕ϊ����s���N���X�ł��B
	@details	�����G���R�[�f�B���O�ŉ�����ϊ�����ہA�����I�ɕϊ����s�����Ƃ��ł��܂��B
*/
class EncodingConverter : public NonCopyable
{
public:
	EncodingConverter();
	~EncodingConverter();

public:

	/**
		@brief		�ϊ���e�L�X�g�̃G���R�[�f�B���O��ݒ肵�܂��B
	*/
	void SetDestinationEncoding(Encoding* encoding);

	/**
		@brief		�ϊ���e�L�X�g�̃G���R�[�f�B���O���擾���܂��B
	*/
	Encoding* GetDestinationEncoding() const;

	/**
		@brief		�ϊ����e�L�X�g�̃G���R�[�f�B���O��ݒ肵�܂��B
	*/
	void SetSourceEncoding(Encoding* encoding);

	/**
		@brief		�ϊ����e�L�X�g�̃G���R�[�f�B���O���擾���܂��B
	*/
	Encoding* GetSourceEncoding() const;

	/**
		@brief		�e�L�X�g��ϊ����܂��B
	*/
	const ByteBuffer& Convert(const void* data, size_t byteCount, EncodingConversionResult* outResult = NULL);

	/**
		@brief		�Ō�ɌĂяo���� Convert() �ŕϊ����ꂽ�o�b�t�@���擾���܂��B����� Convert() �̖߂�l�Ɠ���ł��B
	*/
	const ByteBuffer& GetLastBuffer() const;

	/**
		@brief		�Ō�ɌĂяo���� Convert() �� EncodingConversionResult ���擾���܂��B
	*/
	const EncodingConversionResult& GetLastResult() const;

private:
	void CheckUpdateEncoderDecoder();

	void ConvertDecoderRemain(
		const void* src_, size_t srcByteCount, Decoder* srcDecoder,
		void* dest_, size_t destByteCount, Encoder* destEncoder,
		EncodingConversionResult* outResult);

private:
	RefPtr<Encoding>			m_dstEncoding;
	RefPtr<Encoding>			m_srcEncoding;
	Encoder*					m_dstEncoder;
	Decoder*					m_srcDecoder;
	ByteBuffer					m_outputBuffer;
	EncodingConversionResult	m_lastResult;
	ByteBuffer					m_tmpBuffer;			///< ��Ԃ�ێ��ł��Ȃ��f�R�[�_���g���Ă���ꍇ�Ɏg�p����ꎞ�o�b�t�@
	bool						m_encodingModified;
};

LN_NAMESPACE_END
