﻿
#include "../../include/Lumino/Base/String.h"
#include "../../include/Lumino/Text/Encoding.h"

namespace Lumino
{

/**
	@brief		Win32 コードページ エンコーディング
*/
class Win32CodePageEncoding : public Encoding
{
public:
	Win32CodePageEncoding(UINT codePage);
	virtual ~Win32CodePageEncoding() {};

public:
	// override Encoding
	virtual const TCHAR* GetName() const { return m_name; }
	virtual int GetMinByteCount() const { return 1; }
	virtual int GetMaxByteCount() const { return m_cpInfo.MaxCharSize; }
	virtual Decoder* CreateDecoder() const { return LN_NEW Win32CodePageDecoder(m_cpInfo); }
	virtual Encoder* CreateEncoder() const { return LN_NEW Win32CodePageEncoder(m_cpInfo); }
	virtual byte_t* GetPreamble() const { return NULL; }
	virtual int GetCharacterCount(const byte_t* buffer, size_t bufferSize) const;
	virtual int GetLeadExtraLength(const void* buffer, size_t bufferSize) const;

private:
	CPINFOEX	m_cpInfo;
	String		m_name;

private:
	// Decoder
	class Win32CodePageDecoder : public Decoder
	{
	public:
		Win32CodePageDecoder(const CPINFOEX& cpInfo);
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return m_maxByteCount; }
		virtual bool CanRemain() { return m_canRemain; }
		virtual void ConvertToUTF16(const byte_t* inBuffer, size_t inBufferByteCount, UTF16* outBuffer, size_t outBufferCharCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return m_usedDefaultCharCount; }
		virtual bool Completed() { return m_lastLeadByte == 0; }
		virtual void Reset() { m_usedDefaultCharCount = 0; m_lastLeadByte = 0x0000; }

	private:
		int		m_codePage;			///< コードページ
		int		m_maxByteCount;		///< 1文字の最大バイト数
		byte_t	m_lastLeadByte;		///< 前回の Convert で末尾に見つかった先行バイトを保存する
		int		m_usedDefaultCharCount;
		bool	m_canRemain;
	};

	// Encoder
	class Win32CodePageEncoder : public Encoder
	{
	public:
		Win32CodePageEncoder(const CPINFOEX& cpInfo);
		virtual int GetMinByteCount() { return 1; }
		virtual int GetMaxByteCount() { return m_maxByteCount; }
		virtual bool CanRemain() { return m_canRemain; }
		virtual void ConvertFromUTF16(const UTF16* inBuffer, size_t inBufferCharCount, byte_t* outBuffer, size_t outBufferByteCount, size_t* outBytesUsed, size_t* outCharsUsed);
		virtual int UsedDefaultCharCount() { return m_usedDefaultCharCount; }
		virtual bool Completed() { return true; }
		virtual void Reset() { m_usedDefaultCharCount = 0; }

	private:
		int		m_codePage;			///< コードページ
		int		m_maxByteCount;		///< 1文字の最大バイト数
		int		m_usedDefaultCharCount;
		bool	m_canRemain;
	};
};

} // namespace Lumino
