
#include "../Base/ByteBuffer.h"
#include "Encoding.h"

namespace Lumino
{
namespace Text
{

/**
	@brief		テキスト間のエンコーディングの変換を行うクラスです。
	@details	同じエンコーディングで何回も変換する際、効率的に変換を行うことができます。
*/
class EncodingConverter : public NonCopyable
{
public:
	EncodingConverter();
	~EncodingConverter();

public:

	/**
		@brief		変換先テキストのエンコーディングを設定します。
	*/
	void SetDestinationEncoding(Encoding* encoding);

	/**
		@brief		変換先テキストのエンコーディングを取得します。
	*/
	Encoding* GetDestinationEncoding() const;

	/**
		@brief		変換元テキストのエンコーディングを設定します。
	*/
	void SetSourceEncoding(Encoding* encoding);

	/**
		@brief		変換元テキストのエンコーディングを取得します。
	*/
	Encoding* GetSourceEncoding() const;

	/**
		@brief		テキストを変換します。
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
