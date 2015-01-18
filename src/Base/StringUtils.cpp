
#include "../Internal.h"
#include <wctype.h>
#include "../../include/Lumino/Base/RefObject.h"
#include "../../include/Lumino/Base/RefBuffer.h"
#include "../../include/Lumino/Base/String.h"
#include "../../include/Lumino/Base/StringUtils.h"

namespace Lumino
{

// 標準関数をオーバーロードするための実装
static size_t			StrLen(const char* input) { return ::strlen(input); }
static size_t			StrLen(const wchar_t* input) { return ::wcslen(input); }
static const char*		StrStr(const char* s1, const char* s2) { return ::strstr(s1, s2); }
static const wchar_t*	StrStr(const wchar_t* s1, const wchar_t* s2) { return ::wcsstr(s1, s2); }

#ifdef _WIN32
static int				VSPrintf(char* out, int charCount, const char* format, va_list args) { return _vsnprintf_s(out, charCount, _TRUNCATE, format, args); }
static int				VSPrintf(wchar_t* out, int charCount, const wchar_t* format, va_list args) { return vswprintf(out, charCount, format, args); }
#else
static int				VSPrintf(char* out, int charCount, const char* format, va_list args) { return vsnprintf(out, charCount, format, args); }
static int				VSPrintf(wchar_t* out, int charCount, const wchar_t* format, va_list args) { return vswprintf(out, charCount, format, args); }
#endif

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

	int nLen = StrLen(str1);
	if (startIndex > nLen)
		return -1;

	const TChar* pPos = StrStr(str1 + startIndex, str2);
	if (pPos == NULL)
		return -1;

	return pPos - str1;
}
template int StringUtils::IndexOf<char>(const char* str1, const char* str2, int startIndex);
template int StringUtils::IndexOf<wchar_t>(const wchar_t* str1, const wchar_t* str2, int startIndex);

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
	*outLength = end - begin;
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
	out->assign(buf);
}
template void StringUtils::FormatVAList<char>(const char* format, va_list args, BasicString<char>* out);
template void StringUtils::FormatVAList<wchar_t>(const wchar_t* format, va_list args, BasicString<wchar_t>* out);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool StringUtils::EndsWith(const TChar* str1, int len1, const TChar* str2, int len2, StringComparison comparisonType)
{
	// 長さが -1 の場合は \0 までカウント
	len1 = (len1 < 0) ? StrLen(str1) : len1;
	len2 = (len2 < 0) ? StrLen(str2) : len2;

	const TChar* p1 = str1 + len1;
	const TChar* p2 = str2 + len2;
	
	// 大文字小文字を区別する場合
	if (comparisonType == StringComparison_IgnoreCase)
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
	// 大文字小文字を区別しない場合
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
template bool StringUtils::EndsWith<char>(const char* str1, int len1, const char* str2, int len2, StringComparison comparisonType);
template bool StringUtils::EndsWith<wchar_t>(const wchar_t* str1, int len1, const wchar_t* str2, int len2, StringComparison comparisonType);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
Array< BasicString<TChar> > StringUtils::Split(const BasicString<TChar>& str, const TChar* delim, StringSplitOptions option)
{
	Array< BasicString<TChar> > result;

	// 最初の区切り文字を探す
	int tokenStart = 0;
	int delimIndex = str.IndexOf(delim, 0);

	if (delimIndex >= 0) {
		if (option == StringSplitOptions_None || delimIndex > tokenStart) {
			result.push_back(str.SubString(tokenStart, delimIndex - tokenStart));
		}
	}
	else {
		if (option == StringSplitOptions_None || tokenStart != str.size()) {
			result.push_back(str.SubString(tokenStart));	// 残り全て
		}
		return result;
	}
	// 次のトークン開始位置を指す
	tokenStart = delimIndex + 1;

	while (tokenStart <= ((int)str.size()))
	{
		delimIndex = str.IndexOf(delim, tokenStart);
		if (delimIndex >= 0) {
			if (option == StringSplitOptions_None || delimIndex > tokenStart) {
				result.push_back(str.SubString(tokenStart, delimIndex - tokenStart));
			}
		}
		else {
			if (option == StringSplitOptions_None || tokenStart != str.size()) {
				result.push_back(str.SubString(tokenStart));	// 残り全て
			}
			break;
		}
		// 次のトークン開始位置を指す
		tokenStart = delimIndex + 1;
	}

	return result;
}
template Array< BasicString<char> > StringUtils::Split(const BasicString<char>& str, const char* delim, StringSplitOptions option);
template Array< BasicString<wchar_t> > StringUtils::Split(const BasicString<wchar_t>& str, const wchar_t* delim, StringSplitOptions option);

} // namespace Lumino
