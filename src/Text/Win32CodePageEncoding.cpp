﻿
#include "../Internal.h"
#include <Lumino/Base/ByteBuffer.h>
#include "../../include/Lumino/Text/UnicodeUtils.h"
#include "Win32CodePageEncoding.h"

LN_NAMESPACE_BEGIN

//=============================================================================
// Win32CodePageEncoding
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32CodePageEncoding::Win32CodePageEncoding(UINT codePage)
{
	BOOL r = ::GetCPInfoEx(codePage, 0, &m_cpInfo);
	LN_THROW(r, Win32Exception, ::GetLastError());

	m_name = String::SPrintf(_T("cp%u"), codePage);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Win32CodePageEncoding::GetCharacterCount(const void* buffer, size_t bufferSize) const
{
	int count = 0;
	const byte_t* pos = (const byte_t*)buffer;
	const byte_t* end = pos + bufferSize;
	while (pos < end)
	{
		if (::IsDBCSLeadByteEx(m_cpInfo.CodePage, *pos)) {
			pos++;
		}
		pos++;
		count++;
	}
	return count;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Win32CodePageEncoding::GetLeadExtraLength(const void* buffer, size_t bufferSize) const
{
	return (::IsDBCSLeadByteEx(m_cpInfo.CodePage, *((const byte_t*)buffer))) ? 1 : 0;
}

//=============================================================================
// Win32CodePageEncoding::Win32CodePageDecoder
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32CodePageEncoding::Win32CodePageDecoder::Win32CodePageDecoder(const CPINFOEX& cpInfo)
	: m_codePage(cpInfo.CodePage)
	, m_maxByteCount(cpInfo.MaxCharSize)
	, m_lastLeadByte(0)
	, m_usedDefaultCharCount(0)
	, m_canRemain(false)
{
	// 2文字マルチバイト文字コードだが、先行バイトの分かるものであれば状態を保持しながら変換ができる。
	if (m_maxByteCount == 2 && cpInfo.LeadByte[0] != '\0') {
		m_canRemain = true;
	}
	Reset();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Win32CodePageEncoding::Win32CodePageDecoder::ConvertToUTF16(const byte_t* input, size_t inputByteSize, UTF16* output, size_t outputElementSize, size_t* outBytesUsed, size_t* outCharsUsed)
{
	if (outputElementSize > 0) {
		output[0] = '\0';
	}
	*outBytesUsed = 0;
	*outCharsUsed = 0;

	// 入力が 0 文字の場合は何もしない
	// (MultiByteToWideChar の戻り値がエラーなのか成功なのかわからなくなる)
	if (inputByteSize == 0) {
		return;
	}

	int convertedWideCount;
	if (m_canRemain)
	{
		// 前回の先行バイトが保存されていれば、入力バッファの先頭に先行バイトを埋め込んだ新しいバッファを作る。
		// されていなければそのまま input をコピーでバッファを作る。
		ByteBuffer tmpInBuffer;
		if (m_lastLeadByte != '\0')
		{
			tmpInBuffer.Resize(1 + inputByteSize);
			tmpInBuffer.Copy(0, &m_lastLeadByte, 1);
			tmpInBuffer.Copy(1, input, inputByteSize);
		}
		else
		{
			tmpInBuffer.Resize(inputByteSize);
			tmpInBuffer.Copy(0, input, inputByteSize);
		}

		// バッファ全体をチェック。末尾が先行バイトであれば defectCount が 1 でループ終了する。
		size_t defectCount = 0;
		for (size_t i = 0; i < tmpInBuffer.GetSize(); i++)
		{
			if (defectCount > 0) {
				// ひとつ前の文字が LeadByte だった。2つで1文字。
				defectCount = 0;
			}
			else
			{
				if (::IsDBCSLeadByteEx(m_codePage, tmpInBuffer[i]) != FALSE) {
					// 先行バイト文字である
					defectCount = 1;
				}
			}
		}

		// バッファ終端が先行バイトだった。先行バイトを保存する
		if (defectCount > 0) {
			m_lastLeadByte = tmpInBuffer[tmpInBuffer.GetSize() - 1];
		}
		else {
			m_lastLeadByte = '\0';
		}

		// バッファ全てが先行バイトだった。何もしない
		if (defectCount == tmpInBuffer.GetSize()) {
			return;
		}

		// 変換
		convertedWideCount = ::MultiByteToWideChar(m_codePage, MB_ERR_INVALID_CHARS, (LPCSTR)tmpInBuffer.GetConstData(), tmpInBuffer.GetSize() - defectCount, (LPWSTR)output, outputElementSize);
		LN_THROW(convertedWideCount > 0, EncodingException);
	}
	else
	{
		convertedWideCount = ::MultiByteToWideChar(m_codePage, MB_ERR_INVALID_CHARS, (LPCSTR)input, inputByteSize, (LPWSTR)output, outputElementSize);
		LN_THROW(convertedWideCount > 0, EncodingException);
	}

	// MultiByteToWideChar じゃ文字数カウントはできないので UnicodeUtils を使う
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)output, convertedWideCount, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingException);

	*outBytesUsed = convertedWideCount * sizeof(wchar_t);
	*outCharsUsed = count;
}

//=============================================================================
// Win32CodePageEncoding::Win32CodePageEncoder
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32CodePageEncoding::Win32CodePageEncoder::Win32CodePageEncoder(const CPINFOEX& cpInfo)
	: m_codePage(cpInfo.CodePage)
	, m_maxByteCount(cpInfo.MaxCharSize)
	, m_usedDefaultCharCount(0)
	, m_canRemain(false)
{
	// TODO: 実装の時間無いので今は flase
	m_canRemain = false;
	Reset();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Win32CodePageEncoding::Win32CodePageEncoder::ConvertFromUTF16(const UTF16* input, size_t inputElementSize, byte_t* output, size_t outputByteSize, size_t* outBytesUsed, size_t* outCharsUsed)
{
	if (outputByteSize > 0) {
		output[0] = '\0';
	}
	*outBytesUsed = 0;
	*outCharsUsed = 0;

	// 入力が 0 文字の場合は何もしない
	// (WideCharToMultiByte の戻り値がエラーなのか成功なのかわからなくなる)
	if (inputElementSize == 0) {
		return;
	}

	// マッピングできない文字をデフォルト文字に変換する設定をいろいろ
	DWORD dwFlags = 0;
	char chDefault = (char)mFallbackReplacementChar;
	BOOL bUsedDefaultChar = FALSE;
	LPCSTR pDefault = NULL;
	if (chDefault != '\0') {
		dwFlags = WC_NO_BEST_FIT_CHARS;
		pDefault = &chDefault;
	}

	int convertedByteCount = 0;
	if (m_canRemain)
	{
		LN_THROW(0, NotImplementedException);
	}
	else
	{
		// 変換
		convertedByteCount = ::WideCharToMultiByte(
			m_codePage,
			dwFlags,
			(const wchar_t*)input,
			inputElementSize,
			(LPSTR)output,
			outputByteSize,
			pDefault,
			&bUsedDefaultChar);

		LN_THROW(convertedByteCount > 0, EncodingException);
	}

	// WideCharToMultiByte じゃ文字数カウントはできないので UnicodeUtils を使う
	int count;
	UTFConversionResult r = UnicodeUtils::GetUTF16CharCount((UnicodeUtils::UTF16*)input, inputElementSize, true, &count);
	LN_THROW(r == UTFConversionResult_Success, EncodingException);

	*outBytesUsed = convertedByteCount;
	*outCharsUsed = count;
}

LN_NAMESPACE_END
