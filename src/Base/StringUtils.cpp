﻿
#include "../Internal.h"
#include <wctype.h>
#include "../../include/Lumino/Base/RefObject.h"
#include "../../include/Lumino/Base/ByteBuffer.h"
#include "../../include/Lumino/Base/String.h"
#include "../../include/Lumino/Base/StringUtils.h"

namespace Lumino
{


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
size_t StringUtils::StrLen(const TChar* str)
{
	const TChar* s;
	for (s = str; *s; ++s);
	return (s - str);
}
template size_t StringUtils::StrLen<UTF8>(const UTF8* str);
template size_t StringUtils::StrLen<UTF16>(const UTF16* str);
template size_t StringUtils::StrLen<UTF32>(const UTF32* str);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
TChar StringUtils::ToUpper(TChar ch)
{
	return (TChar)toupper((int)ch);
}
template UTF8 StringUtils::ToUpper<UTF8>(UTF8 ch);
template UTF16 StringUtils::ToUpper<UTF16>(UTF16 ch);
template UTF32 StringUtils::ToUpper<UTF32>(UTF32 ch);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool StringUtils::IsSpace(TChar ch)
{
	return isspace((int)ch) != 0;
}
template bool StringUtils::IsSpace(UTF8 ch);
template bool StringUtils::IsSpace(UTF16 ch);
template bool StringUtils::IsSpace(UTF32 ch);





// 標準関数をオーバーロードするための実装
//static size_t			StrLen(const char* input) { return ::strlen(input); }
//static size_t			StrLen(const wchar_t* input) { return ::wcslen(input); }
static const char*		StrStr(const char* s1, const char* s2) { return ::strstr(s1, s2); }
static const wchar_t*	StrStr(const wchar_t* s1, const wchar_t* s2) { return ::wcsstr(s1, s2); }

#ifdef _WIN32
int				StringUtils::VSPrintf(char* out, int charCount, const char* format, va_list args) { return _vsnprintf_s(out, charCount, _TRUNCATE, format, args); }
int				StringUtils::VSPrintf(wchar_t* out, int charCount, const wchar_t* format, va_list args) { return vswprintf(out, charCount, format, args); }
#else
int				StringUtils::VSPrintf(char* out, int charCount, const char* format, va_list args) { return vsnprintf(out, charCount, format, args); }
int				StringUtils::VSPrintf(wchar_t* out, int charCount, const wchar_t* format, va_list args) 
{
	LN_THROW(0, NotImplementedException);	// vswprintf は動作保障無し
	return vswprintf(out, charCount, format, args);
}
#endif


/*
#ifdef _WIN32
int StringUtils::StrNICmp(const char* str1, const char* str2, size_t count) { return _strnicmp(str1, str2, count); }
int StringUtils::StrNICmp(const wchar_t* str1, const wchar_t* str2, size_t count) { return _wcsnicmp(str1, str2, count); }
#else
int StringUtils::StrNICmp(const char* str1, const char* str2, size_t count) { return strnicmp(str1, str2, count); }
int StringUtils::StrNICmp(const wchar_t* str1, const wchar_t* str2, size_t count) { return wcsnicmp(str1, str2, count); }
#endif
*/
int StringUtils::StrNICmp(const char* s1, const char* s2, size_t count)
{
	if (count == 0) {
		return 0;
	}

	//while (*s1 && *s2)
	do
	{
		if (StringUtils::ToUpper(*s1) != StringUtils::ToUpper(*s2))
		{
			return ((StringUtils::ToUpper(*s1) - StringUtils::ToUpper(*s2)));
		}
		if (*s1 == 0) {
			break;
		}
		++s1;
		++s2;
		--count;
	} while (count != 0);
	return 0;//((StringUtils::ToUpper(*s1) - StringUtils::ToUpper(*s2)));
}
int StringUtils::StrNICmp(const wchar_t* s1, const wchar_t* s2, size_t count)
{
	if (count == 0) {
		return 0;
	}

	//while (*s1 && *s2)
	do
	{
		if (StringUtils::ToUpper(*s1) != StringUtils::ToUpper(*s2))
		{
			return ((StringUtils::ToUpper(*s1) - StringUtils::ToUpper(*s2)));
		}
		if (*s1 == 0) {
			break;
		}
		++s1;
		++s2;
		--count;
	} while (count != 0);
	return 0;//((StringUtils::ToUpper(*s1) - StringUtils::ToUpper(*s2)));
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void StringUtils::ConvertMultiToWide(std::wstring* out, const char* input, int inputLength)
{
	StringW strWide;
	strWide.AssignCStr(input, inputLength);
	(*out) = strWide;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int StringUtils::IndexOf(const TChar* str1, const TChar* str2, int startIndex)
{
	LN_THROW(str1 && str2, ArgumentException);

	int nLen = (int)StrLen(str1);
	if (startIndex > nLen)
		return -1;

	const TChar* pPos = StrStr(str1 + startIndex, str2);
	if (pPos == NULL)
		return -1;

	return (int)(pPos - str1);
}
template int StringUtils::IndexOf<char>(const char* str1, const char* str2, int startIndex);
template int StringUtils::IndexOf<wchar_t>(const wchar_t* str1, const wchar_t* str2, int startIndex);


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int StringUtils::LastIndexOf(const TChar* str1, int str1Len, const TChar* str2, int str2Len, int startIndex, int count, CaseSensitivity cs)
{
	LN_THROW(str1 != NULL && str2 != NULL, ArgumentException);	// 各文字列は NULL でないこと。
	LN_THROW(startIndex < str1Len, ArgumentException);			// startIndex は str1 の長さを超えてはいけない。

	if (str1Len == 0 || str2Len == 0) {
		return -1;	// それぞれ長さが 0 であれば見つかるはずがない
	}
	if (startIndex == 0 || count == 0) {
		return -1;	// 検索範囲がつぶれてる
	}

	if (startIndex < 0) {
		startIndex = StrLen(str1) - 1;
	}

	const TChar* pos = str1 + startIndex;							// 検索範囲の末尾の文字を指す
	const TChar* end = (count < 0) ? str1 : pos - (count - 1);		// 検索範囲の先頭の文字を指す
	LN_THROW(end <= pos, ArgumentException);						// 末尾と先頭が逆転してないこと。

	if (pos - end < str2Len) {
		return -1;	// 検索範囲が検索文字数よりも少ない場合は見つかるはずがない
	}

	pos -= (str2Len - 1);

	// 大文字小文字を区別する
	if (cs == CaseSensitivity_CaseSensitive)
	{
		// 後ろから前へ見ていく
		while (pos >= end)
		{
			if (StrNCmp(pos, str2, str2Len) == 0) {
				return pos - str1;
			}
			--pos;
		}
	}
	// 大文字小文字を区別しない
	else
	{
		// 後ろから前へ見ていく
		while (pos >= end)
		{
			if (StrNICmp(pos, str2, str2Len) == 0) {
				return pos - str1;
			}
			--pos;
		}
	}
	return -1;
}
template int StringUtils::LastIndexOf<char>(const char* str1, int str1Len, const char* str2, int str2Len, int startIndex, int count, CaseSensitivity cs);
template int StringUtils::LastIndexOf<wchar_t>(const wchar_t* str1, int str1Len, const wchar_t* str2, int str2Len, int startIndex, int count, CaseSensitivity cs);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int StringUtils::Compare(const TChar* str1, const TChar* str2, int count, CaseSensitivity cs)
{
	count = (count < 0) ? INT_MAX : count;
	
	if (cs == CaseSensitivity_CaseSensitive)
	{
		while (count > 0)
		{
			if (*str1 != *str2) {
				return ((unsigned int)(*str1)) - ((unsigned int)(*str2));
			}
			if (*str1 == 0) {
				break;
			}
			++str1;
			++str2;
			--count;
		}
		return 0;
	}
	else
	{
		while (count > 0)
		{
			if (ToUpper(*str1) != ToUpper(*str2)) {
				return ((unsigned int)(ToUpper(*str1))) - ((unsigned int)(ToUpper(*str2)));
			}
			if (*str1 == 0) {
				break;
			}
			++str1;
			++str2;
			--count;
		}
		return 0;
	}
}
template int StringUtils::Compare<char>(const char* str1, const char* str2, int count, CaseSensitivity cs);
template int StringUtils::Compare<wchar_t>(const wchar_t* str1, const wchar_t* str2, int count, CaseSensitivity cs);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void StringUtils::Trim(const TChar* begin, int length, const TChar** outBegin, int* outLength)
{
	LN_THROW(begin && length >= 0 && outBegin && outLength, ArgumentException);

	if (length == 0) {
		*outBegin = begin;
		*outLength = 0;
		return;
	}

	const TChar* end = begin + length;

	// Left 部分
	while (*begin)
	{
		if (!IsSpace(*begin)) {
			break;
		}
		++begin;
	}

	// Right 部分
	while (begin < end)
	{
		if (!IsSpace(*(end - 1))) {
			break;
		}
		--end;
	}

	*outBegin = begin;
	*outLength = (int)(end - begin);
}
template void StringUtils::Trim<char>(const char* begin, int length, const char** outBegin, int* outLength);
template void StringUtils::Trim<wchar_t>(const wchar_t* begin, int length, const wchar_t** outBegin, int* outLength);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> StringUtils::Format(const TChar* format, ...)
{
	BasicString<TChar> str;
	va_list args;
	va_start(args, format);
	try {
		StringUtils::FormatVAList(format, args, &str);
		va_end(args);
	}
	catch (...) {
		va_end(args);
		throw;
	}
	return str;
}
template BasicString<char> StringUtils::Format(const char* format, ...);
template BasicString<wchar_t> StringUtils::Format(const wchar_t* format, ...);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void StringUtils::FormatVAList(const TChar* format, va_list args, BasicString<TChar>* out)
{
	static const int nMaxLength = BasicString<TChar>::MaxFormatLength;

	TChar buf[nMaxLength + 1];
	memset(buf, 0, sizeof(buf));
	int validSize = VSPrintf(buf, nMaxLength + 1, format, args);

	LN_THROW(0 <= validSize && validSize <= nMaxLength, ArgumentException);
	*out = buf;
}
template void StringUtils::FormatVAList<char>(const char* format, va_list args, BasicString<char>* out);
template void StringUtils::FormatVAList<wchar_t>(const wchar_t* format, va_list args, BasicString<wchar_t>* out);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool StringUtils::EndsWith(const TChar* str1, int len1, const TChar* str2, int len2, CaseSensitivity cs)
{
	// 長さが -1 の場合は \0 までカウント
	len1 = (len1 < 0) ? StrLen(str1) : len1;
	len2 = (len2 < 0) ? StrLen(str2) : len2;

	const TChar* p1 = str1 + len1;
	const TChar* p2 = str2 + len2;
	
	// 大文字小文字を区別しない場合
	if (cs == CaseSensitivity_CaseInsensitive)
	{
		while (str2 <= p2)
		{
			if (p1 < str1 ||
				toupper(*p1) != toupper(*p2)) {
				return false;
			}
			p1--;
			p2--;
		}
		return true;
	}
	// 大文字小文字を区別する場合
	else
	{
		while (str2 <= p2)
		{
			if (p1 < str1 ||
				*p1 != *p2) {
				return false;
			}
			p1--;
			p2--;
		}
		return true;
	}
}
template bool StringUtils::EndsWith<char>(const char* str1, int len1, const char* str2, int len2, CaseSensitivity cs);
template bool StringUtils::EndsWith<wchar_t>(const wchar_t* str1, int len1, const wchar_t* str2, int len2, CaseSensitivity cs);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> StringUtils::Left(const TChar* str, int count)
{
	if (count < 0) {
		count = 0;
	}

	int len = StrLen(str);
	if (count >= len) {
		return BasicString<TChar>(str);
	}
	return BasicString<TChar>(str, count);
}
template BasicString<char> StringUtils::Left<char>(const char* str, int count);
template BasicString<wchar_t> StringUtils::Left<wchar_t>(const wchar_t* str, int count);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> StringUtils::Right(const TChar* str, int count)
{
	if (count < 0) {
		count = 0;
	}

	int len = StrLen(str);
	if (count >= len) {
		return BasicString<TChar>(str);
	}
	return BasicString<TChar>(str + len - count, count);
}
template BasicString<char> StringUtils::Right<char>(const char* str, int count);
template BasicString<wchar_t> StringUtils::Right<wchar_t>(const wchar_t* str, int count);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> StringUtils::Mid(const TChar* str, int start, int count)
{
	int len = StrLen(str);

	if (start < 0) {
		start = 0;
	}
	if (count < 0) {
		count = len - start;
	}

	if (start + count > len) {
		count = len - start;
	}
	if (start > len) {
		count = 0;
	}

	if (start == 0 && count == len) {
		return BasicString<TChar>(str);
	}

	return BasicString<TChar>(str + start, count);
}
template BasicString<char> StringUtils::Mid<char>(const char* str, int start, int count);
template BasicString<wchar_t> StringUtils::Mid<wchar_t>(const wchar_t* str, int start, int count);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
ArrayList< BasicString<TChar> > StringUtils::Split(const BasicString<TChar>& str, const TChar* delim, StringSplitOptions option)
{
	ArrayList< BasicString<TChar> > result;

	// 最初の区切り文字を探す
	int tokenStart = 0;
	int delimIndex = str.IndexOf(delim, 0);

	if (delimIndex >= 0) {
		if (option == StringSplitOptions_None || delimIndex > tokenStart) {
			result.Add(str.SubString(tokenStart, delimIndex - tokenStart));
		}
	}
	else {
		if (option == StringSplitOptions_None || tokenStart != str.GetLength()) {
			result.Add(str.SubString(tokenStart));	// 残り全て
		}
		return result;
	}
	// 次のトークン開始位置を指す
	tokenStart = delimIndex + 1;

	while (tokenStart <= ((int)str.GetLength()))
	{
		delimIndex = str.IndexOf(delim, tokenStart);
		if (delimIndex >= 0) {
			if (option == StringSplitOptions_None || delimIndex > tokenStart) {
				result.Add(str.SubString(tokenStart, delimIndex - tokenStart));
			}
		}
		else {
			if (option == StringSplitOptions_None || tokenStart != str.GetLength()) {
				result.Add(str.SubString(tokenStart));	// 残り全て
			}
			break;
		}
		// 次のトークン開始位置を指す
		tokenStart = delimIndex + 1;
	}

	return result;
}
template ArrayList< BasicString<char> > StringUtils::Split(const BasicString<char>& str, const char* delim, StringSplitOptions option);
template ArrayList< BasicString<wchar_t> > StringUtils::Split(const BasicString<wchar_t>& str, const wchar_t* delim, StringSplitOptions option);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename T>
int StringUtils::CheckNewLineSequence(const T* start, const T* end)
{
	if (start < end)
	{
		if (start[0] == '\r') {
			if ((start + 1) < end && start[1] == '\n') {
				return 2;	// CR+LF
			}
			else {
				return 1;	// CR
			}
		}
		else if (start[0] == '\n') {
			return 1;		// LF
		}
	}
	return 0;
}
template int StringUtils::CheckNewLineSequence<byte_t>(const byte_t* start, const byte_t* end);
template int StringUtils::CheckNewLineSequence<char>(const char* start, const char* end);
template int StringUtils::CheckNewLineSequence<wchar_t>(const wchar_t* start, const wchar_t* end);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
static bool StringUtils::Match(const TChar* pattern, const TChar* str)
{
	switch (*pattern)
	{
	case _T('\0'):
		return _T('\0') == *str;
	case _T('*'):
		return Match(pattern + 1, str) || ((_T('\0') != *str) && Match(pattern, str + 1));
	case _T('?'):
		return (_T('\0') != *str) && Match(pattern + 1, str + 1);
	default:
		return (*pattern == *str) && Match(pattern + 1, str + 1);
	}
}
template bool StringUtils::Match<char>(const char* pattern, const char* str);
template bool StringUtils::Match<wchar_t>(const wchar_t* pattern, const wchar_t* str);

} // namespace Lumino
