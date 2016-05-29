﻿/*
	[2016/4/12] IsNullOrEmpty()
		null か空かは区別できたほうがいい・・・と思う。
		安全管理的には、null は null のままにして値が変なことを知れたほうがよい。
*/
#include "../Internal.h"
#include <Lumino/Base/StringRef.h>
#include <Lumino/IO/PathName.h>
#include "../Text/UTF8Encoding.h"
#ifdef LN_OS_WIN32
#include "../Text/Win32CodePageEncoding.h"
#endif

LN_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
template<>
int GenericStringRef<char>::CopyToLocal8Bit(char* dest, int destLen) const
{
	return CopyTo(dest, destLen);
}
template<>
int GenericStringRef<wchar_t>::CopyToLocal8Bit(char* dest, int destLen) const
{
#ifdef LN_OS_WIN32
	CPINFOEX cpInfo;
	BOOL r = ::GetCPInfoEx(CP_THREAD_ACP, 0, &cpInfo);
	LN_THROW(r, Win32Exception, ::GetLastError());
	Win32CodePageEncoding::Win32CodePageEncoder enc(cpInfo);
	size_t byteUsed = 0;
	size_t charUsed = 0;
	enc.ConvertFromUTF16(m_str + m_pos, m_len, (byte_t*)dest, destLen - 1, &byteUsed, &charUsed);
	dest[byteUsed] = '\0';
	return byteUsed;
#else
	// TODO: メモリ確保伴わないようにはできるけど時間ないので後回し
	EncodingConversionResult result;
	Encoding::Convert(
		m_str + m_pos, m_len * sizeof(wchar_t), Encoding::GetWideCharEncoding(),
		dest, destLen, Encoding::GetUTF8Encoding(),
		&result);
	return result.BytesUsed;

	//UTF8Encoding::UTF8Encoder enc(false);
	//size_t byteUsed = 0;
	//size_t charUsed = 0;
	//enc.ConvertFromUTF16(m_str + m_pos, m_len, (byte_t*)dest, destLen - 1, &byteUsed, &charUsed);
	//dest[byteUsed] = '\0';
	//return byteUsed;
#endif
	//std::mbstate_t state = std::mbstate_t();
	//int len = 1 + std::wcsrtombs(nullptr, &wstr, 0, &state);	// +1 は '\0' の分
	//LN_THROW(destLen > len, OutOfRangeException);
	//if (len < LN_MAX_PATH)
	//{
	//	std::wcsrtombs(dest, &wstr, mbstr.size(), &state);
	//}
}

// テンプレートのインスタンス化
template class GenericStringRef<char>;
template class GenericStringRef<wchar_t>;


LN_NAMESPACE_END
