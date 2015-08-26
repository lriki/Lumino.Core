
#include "../../include/Lumino/Text/Encoding.h"

namespace Lumino
{
namespace Text
{

/**
	@brief		UTF8 エンコーディング
*/
class UTF8Encoding : public Encoding
{
public:
	UTF8Encoding(bool byteOrderMark);
	virtual ~UTF8Encoding() {};

public:
	// override Encoding
	virtual int GetMinByteCount() const { return 1; }
	virtual int GetMaxByteCount() const { return 6; }
	virtual Decoder* CreateDecoder() const { return LN_NEW UTF8Decoder(m_byteOrderMark); }
	virtual Encoder* CreateEncoder() const { return LN_NEW UTF8Encoder(m_byteOrderMark); }
	virtual byte_t* GetPreamble() const;
	virtual int GetCharacterCount(const byte_t* buffer, size_t bufferSize) const;

private:
	bool	m_byteOrderMark;

private:
	// Decoder
	class UTF8Decoder : public Decoder
	{
	public:
		UTF8Decoder(bool byteOrderMark) { m_byteOrderMark = byteOrderMark; Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 6; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int		mUsedDefaultCharCount;	///< 一連の ConvertToUTF16() の呼び出しの中で、変換できない文字を規定文字に変換した文字数
		bool	mCompleted;				///< 最後の ConvertToUTF16() で、バッファ末尾でマルチバイト文字が途切れていなければ true
		bool	m_byteOrderMark;
	};

	// Encoder
	class UTF8Encoder : public Encoder
	{
	public:
		UTF8Encoder(bool byteOrderMark) { m_byteOrderMark = byteOrderMark; Reset(); }
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return 6; }
		virtual bool CanRemain() { return false; }
		virtual void ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return mUsedDefaultCharCount; }
		virtual bool Completed() { return mCompleted; }
		virtual void Reset() { mUsedDefaultCharCount = 0; mCompleted = false; }

	private:
		int		mUsedDefaultCharCount;	///< 一連の ConvertFromUTF16() の呼び出しの中で、変換できない文字を規定文字に変換した文字数
		bool	mCompleted;				///< 最後の ConvertFromUTF16() で、バッファ末尾でマルチバイト文字が途切れていなければ true
		bool	m_byteOrderMark;
	};
};

} // namespace Text
} // namespace Lumino
