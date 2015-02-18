
#include "../../include/Lumino/Text/Encoding.h"

namespace Lumino
{
namespace Text
{

/**
	@brief		ASCII �G���R�[�f�B���O
*/
class ASCIIEncoding : public Encoding
{
public:
	ASCIIEncoding();
	virtual ~ASCIIEncoding() {};

public:
	// override Encoding
	virtual int GetMinByteCount() const { return 1; }
	virtual int GetMaxByteCount() const { return 1; }
	virtual Decoder* CreateDecoder() const { return LN_NEW ASCIIDecoder(); }
	virtual Encoder* CreateEncoder() const { return LN_NEW ASCIIEncoder(); }
	virtual byte_t* GetPreamble() const { return NULL; }

private:
	// Decoder
	class ASCIIDecoder : public Decoder
	{
	public:
		ASCIIDecoder() { Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 1; }
		virtual bool CanRemain() { return true; }
		virtual void ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return true; }
		virtual void Reset() { mUsedDefaultCharCount = 0; }

	private:
		int		mUsedDefaultCharCount;
	};

	// Encoder
	class ASCIIEncoder : public Encoder
	{
	public:
		ASCIIEncoder() { Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 1; }
		virtual bool CanRemain() { return true; }
		virtual void ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return true; }
		virtual void Reset() { mUsedDefaultCharCount = 0; }

	private:
		int			mUsedDefaultCharCount;
	};
};

} // namespace Text
} // namespace Lumino