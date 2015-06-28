
#include "../Base/ByteBuffer.h"
#include "Encoding.h"

namespace Lumino
{
namespace Text
{

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

private:
	void CheckUpdateEncoderDecoder();

	void ConvertDecoderRemain(
		const void* src_, size_t srcByteCount, Decoder* srcDecoder,
		void* dest_, size_t destByteCount, Encoder* destEncoder,
		EncodingConversionResult* outResult);

private:
	RefPtr<Encoding>	m_dstEncoding;
	RefPtr<Encoding>	m_srcEncoding;
	RefPtr<Encoder>		m_dstEncoder;
	RefPtr<Decoder>		m_srcDecoder;
	ByteBuffer			m_outputBuffer;
	bool				m_encodingModified;
};

} // namespace Text
} // namespace Lumino
