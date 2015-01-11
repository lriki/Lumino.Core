
#include "../Internal.h"
#include <sstream>
#include <Base/RefBuffer.h>
#include <Base/Encoding.h>
#include <Base/StringUtils.h>
#include <Base/String.h>
#include <Platform/Environment.h>

namespace Lumino
{

//=============================================================================
// BasicString
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::Format(const TChar* format, ...)
{
	va_list args;
	va_start(args, format);
	try {
		StringUtils::FormatVAList(format, args, this);
		va_end(args);
	}
	catch (...) {
		va_end(args);
		throw;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::AssignCStr(const char* str, size_type begin, size_type length, bool* outUsedDefaultChar)
{
	LN_THROW(str != NULL, ArgumentException);	// std::string の assing は NULL が渡されたときの動作は未定義。VS2013 では制御が返ってこなくなった


	if (length == std_basic_string::npos) {
		length = 0xFFFFFFFF;
	}
	size_type len = std::min(strlen(str), length);

	// サイズ 0 なら空文字列にするだけ
	if (len == 0) {
		SetEmpty();
	}
	else {
		ConvertFrom(str + begin, len, Encoding::GetSystemMultiByteEncoding());
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::AssignCStr(const wchar_t* str, size_type begin, size_type length, bool* outUsedDefaultChar)
{
	LN_THROW(str != NULL, ArgumentException);	// std::string の assing は NULL が渡されたときの動作は未定義。VS2013 では制御が返ってこなくなった


	if (length == std_basic_string::npos) {
		length = 0xFFFFFFFF;
	}
	size_type len = std::min(wcslen(str), length) * sizeof(wchar_t);

	// サイズ 0 なら空文字列にするだけ
	if (len == 0) {
		SetEmpty();
	}
	else {
		ConvertFrom(str + begin, len, Encoding::GetWideCharEncoding());
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::ConvertFrom(const void* buffer, int byteCount, const Encoding* encoding, bool* outUsedDefaultChar)
{
	LN_THROW(encoding, ArgumentException);

	Encoding* thisTypeEncoding = GetThisTypeEncoding();

	// 全く同じエンコーディングなら変換の必要は無い
	if (thisTypeEncoding == encoding) 
	{
		std_basic_string::assign((const TChar*)buffer, byteCount / sizeof(TChar));
	}
	else
	{
		//size_t bytesUsed;
		//size_t charsUsed;
		//bool usedDefaultChar;
		EncodingConversionResult info;
		RefPtr<RefBuffer> tmpBuffer(
			Encoding::Convert(buffer, byteCount, encoding, thisTypeEncoding,
			&info));
		if (outUsedDefaultChar != NULL) {
			*outUsedDefaultChar = info.UsedDefaultChar;
		}

		std_basic_string::assign((const TChar*)tmpBuffer->GetPointer(), info.BytesUsed / sizeof(TChar));
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
RefBuffer* BasicString<TChar>::ConvertTo(const Encoding* encoding, bool* outUsedDefaultChar) const
{
	//size_t bytesUsed;
	//size_t charsUsed;
	//bool usedDefaultChar;
	EncodingConversionResult info;

	RefBuffer* buf = Encoding::Convert(
		std_basic_string::c_str(), GetByteCount(), GetThisTypeEncoding(),
		encoding,
		&info);
	if (outUsedDefaultChar != NULL) {
		*outUsedDefaultChar = info.UsedDefaultChar;
	}

	return buf;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> BasicString<TChar>::SubString(size_type startIndex, size_type length) const
{
	return StringT(*this, startIndex, length);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> BasicString<TChar>::Trim() const
{
	const TChar* begin;
	int length;
	StringUtils::Trim(std_basic_string::c_str(), std_basic_string::size(), &begin, &length);
	return BasicString<TChar>(begin, length);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> BasicString<TChar>::Replace(const TChar* from, const TChar* to) const
{
	size_type pos = 0;
	size_t from_length = StringUtils::GetLength(from);
	size_t to_length = StringUtils::GetLength(to);

	StringT output(*this);

	while (pos = output.find(from, pos), pos != String::npos)
	{
		output.replace(pos, from_length, to);
		pos += to_length;
	}

	return output;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int BasicString<TChar>::IndexOf(const TChar* str, int startIndex) const
{
	size_type pos = this->find(str, startIndex);
	if (pos == std_basic_string::npos) {
		return -1;
	}
	return pos;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
Array< BasicString<TChar> > BasicString<TChar>::Split(const TChar* delim, StringSplitOptions option) const
{
	return StringUtils::Split(*this, delim, option);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static bool StrToInt(const char* str, int* value)
{
	// TODO:str はコピーされ、内部では std::string として保持される。
	//		独自の stream を作ってコピーしないようにすれば高速化の余地がある。
	std::istringstream iss(str);
	iss >> *value;
	return iss.eof();
}
static bool StrToInt(const wchar_t* str, int* value)
{
	std::wistringstream iss(str);
	iss >> *value;
	return iss.eof();
}

template<typename TChar>
int BasicString<TChar>::ToInt() const
{
	int tmp;
	bool r = StrToInt(Trim().c_str(), &tmp);
	LN_THROW(r, InvalidFormatException);
	return tmp;
}

template<typename TChar>
bool BasicString<TChar>::ToInt(int* value) const
{
	return StrToInt(Trim().c_str(), value);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
const BasicString<TChar>& BasicString<TChar>::GetNewLine()
{
	static BasicString<TChar> nl(Environment::GetNewLine<TChar>());
	return nl;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
Encoding* BasicString<TChar>::GetThisTypeEncoding() const
{
	if (sizeof(TChar) == sizeof(char))
	{
		// this のエンコーディングはシステム依存である
		return Encoding::GetSystemMultiByteEncoding();
	}
	else if (sizeof(TChar) == sizeof(wchar_t))
	{
		// this のエンコーディングは wchar_t である
		return Encoding::GetWideCharEncoding();
	}
	else
	{
		LN_THROW(0, NotImplementedException);
	}
}

// テンプレートのインスタンス化
template class BasicString<char>;
template class BasicString<wchar_t>;

} // namespace Lumino
