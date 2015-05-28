
#include "../Internal.h"
#include <sstream>
#include <Lumino/Base/ByteBuffer.h>
#include <Lumino/Text/Encoding.h>
#include <Lumino/Base/StringUtils.h>
#include <Lumino/Base/String.h>
#include <Lumino/Base/Environment.h>


/*
■[2015/2/21]内部文字コードは固定しない

	QString や NSString は内部文字コードを固定しているが、このライブラリの String は固定しない。
	これは、このライブラリが何かの大規模なフレームワークを作るわけではく、
	ほかのライブラリやフレームワークをサポートする目的もあるため。

	String は BasicString<> の typedef で、Windows では _UNICODE により <char> または <wchar_t> を使用する。
	Unix 系は、今のところ <char> だけを使う。(UTF-8)
	これは、Unix では wchar 系の正常動作が十分に期待できないため。
	例えば、Ubuntu では wsprintf 使うと、日本語が全て消えてしまう。
	さらに、wchar_t をサポートしているシステムAPIは無く、必ず UTF-8 に変換してからシステムAPIに渡す必要がある。
	Unix系はもう OS レベルで wchar_t を見限っていると考えた方がよい。Mac ですら NSString を UTF-16 にしている。

	完全に実装するならば printf も自分で実装する必要がある。
	ゆくゆくはタイプセーフな printf (https://github.com/c42f/tinyformat) を作ろうかと考えてはいるが、
	直近ではその重要性は低い。

	最終的には、どのようなパターンを考えても一長一短があり、内部的な理由からは決定しきれない。
	となるとユーザーが使ったときに、コードを何も変更しなくても別 OS でコンパイルできるようになることが最低条件となる。
	その次に来るのが、ユーザーが使っている他のフレームワークやライブラリと統合し易いかどうか。そしてパフォーマンス。

	FreeType を使うときは UTF-32 文字列を使えた方が良いし、CEGUI を使うときは UTF-8 文字列を使えた方が良い。
	Windows ⇔ Unix 間でファイルをやり取りするときのフォーマットは UTF-16 がベターだろう。
	Ruby と連携するときは UTF-8 だし、Java や C# とは UTF-16。
	MFC(WinAPI)と使うなら char と wchar_t をマクロで切り替えられるように。
	そして、例えば内部文字コードを固定すると IndexOf() に指定したリテラルをイチイチ文字コード変換しなければならず、パフォーマンスに影響がかなり出る。

	内部文字コードを固定しても、外部ライブラリと連携するために変換関数を作ればいいだけの話ではあるのだけど、
	このライブラリのメイン用途 (ゲームエンジン) としてはパフォーマンス解決のための手段は用意しておくべき。

	以下、メモ。

	・UTF-8を内部コードとする場合
		+ 新しい Unix 系であれば OS の内部コードが UTF-8 となっているため、それほど問題はない。
		- Windows では printf や API を呼び出すか時に THREAD_ACP に変換する必要がある。
		- Windows では 文字列リテラルは UTF-8 にコンパイルされないため、日本語文字を含む char* で String::IndexOf できない。
		  日本語をハードコーディングすることは稀だが、それでもする場合は 0 ではない。

	・UTF-16を内部コードとする場合
		+ 日本語を扱うとき、メモリ効率は UTF 系の中で一番良いし、当然 Unicode の文字を全て扱える。
		+ Windows では ≒ wchar_t と考えて wchar 系の標準ライブラリが使える。
		- ただし、Unix 計では ≠ wchar_t なので標準ライブラリに相当する関数をほぼすべて自分で実装する必要がある。

	・wchar_t を内部コードとする場合
		+ UTF-16 と比べて_T() のリテラルで、文字列検索とかし易い。
		- Unix では使い物にならないと考えた方がよい。wsprintf とか日本語が消える。UTF-16と同じく、API呼ぶときは UTF-8への変換必須。

	・ノータッチ(MFC ライク)
		+ 文字列リテラルで検索とか、printf の可変引数に指定できる。
		- wprintf とか使い物にならないのは一緒。

	参考：http://www.nubaria.com/en/blog/?p=289
*/

namespace Lumino
{

//template<typename TChar>
//const TChar BasicStringCore<TChar>::EmptyString[1] = { 0x00 };

template<typename TChar>
typename BasicString<TChar>::BasicStringCore/*<TChar>*/ BasicString<TChar>::BasicStringCore/*<TChar>*/::m_sharedEmpty;

//=============================================================================
// BasicString
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar>::BasicString()
	: m_string(NULL)
{
	LN_REFOBJ_SET(m_string, BasicStringCore::GetSharedEmpty());
	m_ref = m_string->c_str();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar>::~BasicString()
{
	LN_SAFE_RELEASE(m_string);
}

//-----------------------------------------------------------------------------
// TChar コンストラクタ系
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar>::BasicString(const BasicString& str)
	: m_ref(NULL)
	, m_string(NULL)
{
	LN_REFOBJ_SET(m_string, str.m_string);
	m_ref = m_string->c_str();
}
template<typename TChar>
BasicString<TChar>::BasicString(const BasicString& str, size_type length)
	: m_ref(NULL)
	, m_string(NULL)
{
	AssignTString(str.m_string->c_str(), length);
}
template<typename TChar>
BasicString<TChar>::BasicString(const BasicString& str, size_type begin, size_type length)
	: m_ref(NULL)
	, m_string(NULL)
{
	AssignTString(str.m_string->c_str() + begin, length);	// str+begin にしないと、暗黙的コンストラクタの呼び出しが発生してしまう
}
template<typename TChar>
BasicString<TChar>::BasicString(const TChar* str)
	: m_ref(NULL)
	, m_string(NULL)
{
	AssignTString(str, -1);
}
template<typename TChar>
BasicString<TChar>::BasicString(const TChar* str, size_type length)
	: m_ref(NULL)
	, m_string(NULL)
{
	AssignTString(str, length);
}
template<typename TChar>
BasicString<TChar>::BasicString(const TChar* str, size_type begin, size_type length)
	: m_ref(NULL)
	, m_string(NULL)
{
	AssignTString(str + begin, length);
}
template<typename TChar>
BasicString<TChar>& BasicString<TChar>::operator=(const BasicString& right)
{
	LN_REFOBJ_SET(m_string, right.m_string);
	m_ref = m_string->c_str();
	return (*this);
}
template<typename TChar>
BasicString<TChar>& BasicString<TChar>::operator=(const std::basic_string<TChar>& right)
{
	AssignTString(right.c_str(), -1);
	return (*this);
}
template<typename TChar>
BasicString<TChar>& BasicString<TChar>::operator=(const TChar* right)
{
	AssignTString(right, -1);
	return (*this);
}

//-----------------------------------------------------------------------------
// YCHAR コンストラクタ系
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar>::BasicString(const BasicString<YCHAR>& str)
	: m_ref(NULL)
	, m_string(NULL)
{
	AssignCStr(str.GetCStr());
}
template<typename TChar>
BasicString<TChar>::BasicString(const BasicString<YCHAR>& str, size_type length)
	: m_ref(NULL)
	, m_string(NULL)
{
	AssignCStr(str.GetCStr(), length);
}
template<typename TChar>
BasicString<TChar>::BasicString(const BasicString<YCHAR>& str, size_type begin, size_type length)
	: m_ref(NULL)
	, m_string(NULL)
{
	AssignCStr(str.GetCStr(), begin, length);
}
template<typename TChar>
BasicString<TChar>::BasicString(const YCHAR* str)
	: m_ref(NULL)
	, m_string(NULL)
{
	AssignCStr(str);
}
template<typename TChar>
BasicString<TChar>::BasicString(const YCHAR* str, size_type length)
	: m_ref(NULL)
	, m_string(NULL)
{
	AssignCStr(str, length);
}
template<typename TChar>
BasicString<TChar>::BasicString(const YCHAR* str, size_type begin, size_type length)
	: m_ref(NULL)
	, m_string(NULL)
{
	AssignCStr(str, begin, length);
}
template<typename TChar>
BasicString<TChar>& BasicString<TChar>::operator=(const BasicString<YCHAR>& right)
{
	AssignCStr(right.GetCStr()); return (*this);
	return (*this);
}
template<typename TChar>
BasicString<TChar>& BasicString<TChar>::operator=(const std::basic_string<YCHAR>& right)
{
	AssignCStr(right.c_str()); return (*this);
	return (*this);
}
template<typename TChar>
BasicString<TChar>& BasicString<TChar>::operator=(const YCHAR* right)
{
	AssignCStr(right); return (*this);
	return (*this);
}

//-----------------------------------------------------------------------------
// operators
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar>& BasicString<TChar>::operator+=(const BasicString<TChar>& right)
{
	Append(right.m_string->c_str(), right.GetLength());
	return (*this);
}
template<typename TChar>
BasicString<TChar>& BasicString<TChar>::operator+=(const std::basic_string<TChar>& right)
{
	Append(right.c_str(), right.size());
	return (*this);
}
template<typename TChar>
BasicString<TChar>& BasicString<TChar>::operator+=(const TChar* ptr)
{
	Append(ptr, -1);
	return (*this);
}
template<typename TChar>
BasicString<TChar>& BasicString<TChar>::operator+=(TChar ch)
{
	Append(&ch, 1);
	return (*this);
}
template<typename TChar>
bool BasicString<TChar>::operator==(const BasicString& right) const
{
	if (GetLength() != right.GetLength()) {
		return false;
	}
	return Compare(right.GetCStr(), right.GetLength(), CaseSensitivity_CaseSensitive) == 0;
}
template<typename TChar>
bool BasicString<TChar>::operator==(const TChar* right) const
{
	return Compare(right, -1, CaseSensitivity_CaseSensitive) == 0;
}
template<typename TChar>
bool BasicString<TChar>::operator < (const BasicString& right) const
{
	return Compare(right.GetCStr(), -1, CaseSensitivity_CaseSensitive) < 0;
}
template<typename TChar>
bool BasicString<TChar>::operator < (const TChar* right) const
{
	return Compare(right, -1, CaseSensitivity_CaseSensitive) < 0;
}
template<typename TChar>
TChar& BasicString<TChar>::operator[](int index)
{
	return InternalGetAt(index);
}
template<typename TChar>
const TChar& BasicString<TChar>::operator[](int index) const
{
	return InternalGetAt(index);
}
template<typename TChar>
BasicString<TChar>::operator const TChar*() const
{
	return GetCStr();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
const TChar* BasicString<TChar>::GetCStr() const
{
	return m_string->c_str();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool BasicString<TChar>::IsEmpty() const
{
	return m_string->empty();
}

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
void BasicString<TChar>::AssignCStr(const char* str, int begin, int length, bool* outUsedDefaultChar)
{
	if (str == NULL) {
		SetEmpty();
		return;
	}
	//LN_THROW(str != NULL, ArgumentException);	// std::string の assing は NULL が渡されたときの動作は未定義。VS2013 では制御が返ってこなくなった

	if (length <= -1) {
		length = INT_MAX;
	}
	int len = std::min((int)strlen(str), length);

	// サイズ 0 なら空文字列にするだけ
	if (len == 0) {
		SetEmpty();
	}
	else {
		ConvertFrom(str + begin, len, Text::Encoding::GetSystemMultiByteEncoding());
	}
}
template<typename TChar>
void BasicString<TChar>::AssignCStr(const char* str, int length, bool* usedDefaultChar)
{
	AssignCStr(str, 0, length, usedDefaultChar);
}
template<typename TChar>
void BasicString<TChar>::AssignCStr(const char* str, bool* usedDefaultChar)
{
	AssignCStr(str, 0, -1, usedDefaultChar);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::AssignCStr(const wchar_t* str, int begin, int length, bool* outUsedDefaultChar)
{
	if (str == NULL) {
		SetEmpty();
		return;
	}
	//LN_THROW(str != NULL, ArgumentException);	// std::string の assing は NULL が渡されたときの動作は未定義。VS2013 では制御が返ってこなくなった

	if (length <= -1) {
		length = INT_MAX;
	}
	int len = std::min((int)wcslen(str), length) * sizeof(wchar_t);

	// サイズ 0 なら空文字列にするだけ
	if (len == 0) {
		SetEmpty();
	}
	else {
		ConvertFrom(str + begin, len, Text::Encoding::GetWideCharEncoding());
	}
}
template<typename TChar>
void BasicString<TChar>::AssignCStr(const wchar_t* str, int length, bool* usedDefaultChar)
{
	AssignCStr(str, 0, length, usedDefaultChar);
}
template<typename TChar>
void BasicString<TChar>::AssignCStr(const wchar_t* str, bool* usedDefaultChar)
{
	AssignCStr(str, 0, -1, usedDefaultChar);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::ConvertFrom(const void* buffer, size_t byteCount, const Text::Encoding* encoding, bool* outUsedDefaultChar)
{
	LN_THROW(encoding, ArgumentException);

	Text::Encoding* thisTypeEncoding = GetThisTypeEncoding();

	// 全く同じエンコーディングなら変換の必要は無い
	if (thisTypeEncoding == encoding) 
	{
		AssignTString((const TChar*)buffer, byteCount / sizeof(TChar));
	}
	else
	{
		Text::EncodingConversionResult info;
		RefPtr<ByteBuffer> tmpBuffer(
			Text::Encoding::Convert(buffer, byteCount, encoding, thisTypeEncoding,
			&info));
		if (outUsedDefaultChar != NULL) {
			*outUsedDefaultChar = info.UsedDefaultChar;
		}

		AssignTString((const TChar*)tmpBuffer->GetData(), info.BytesUsed / sizeof(TChar));
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
ByteBuffer* BasicString<TChar>::ConvertTo(const Text::Encoding* encoding, bool* outUsedDefaultChar) const
{
	Text::EncodingConversionResult info;

	ByteBuffer* buf = Text::Encoding::Convert(
		GetCStr(), GetByteCount(), GetThisTypeEncoding(),
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
void BasicString<TChar>::SetEmpty()
{
	LN_REFOBJ_SET(m_string, BasicStringCore::GetSharedEmpty());
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
	StringUtils::Trim(GetCStr(), GetLength(), &begin, &length);
	return BasicString<TChar>(begin, length);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
struct CmpCaseSensitive
{
	TChar ch;
	bool operator()(TChar value) { return ch == value; }
};

template<typename TChar>
struct CmpCaseInsensitive
{
	TChar ch;
	bool operator()(TChar value) { return StringUtils::ToUpper(ch) == StringUtils::ToUpper(value); }
};

template<typename TChar>
BasicString<TChar> BasicString<TChar>::Remove(TChar ch, CaseSensitivity cs) const
{
	BasicString<TChar> newStr(*this);
	if (newStr.IsEmpty()) {
		// 空文字列なら処理を行う必要は無い。というか、SharedEmpty に対して erase とかしてはいけない。
		return newStr;
	}

	// 大文字と小文字を区別する
	BasicStringCore& ss = *newStr.m_string;
	if (cs == CaseSensitivity_CaseSensitive)
	{
		CmpCaseSensitive<TChar> cmp;
		cmp.ch = ch;
		ss.erase(std::remove_if(ss.begin(), ss.end(), cmp), ss.end());
	}
	// 大文字と小文字を区別しない
	else //if (cs == CaseSensitivity_CaseSensitive)
	{
		CmpCaseInsensitive<TChar> cmp;
		cmp.ch = ch;
		ss.erase(std::remove_if(ss.begin(), ss.end(), cmp), ss.end());
	}

	return newStr;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> BasicString<TChar>::Replace(const TChar* from, const TChar* to) const
{
	BasicString<TChar> newStr(*this);
	if (newStr.IsEmpty()) {
		// 空文字列なら処理を行う必要は無い。というか、SharedEmpty に対して replace とかしてはいけない。
		return newStr;
	}

	size_type pos = 0;
	size_t fromLength = StringUtils::StrLen(from);
	size_t toLength = StringUtils::StrLen(to);

	while (pos = newStr.m_string->find(from, pos), pos != std::basic_string<TChar>::npos)
	{
		newStr.m_string->replace(pos, fromLength, to);
		pos += toLength;
	}

	return newStr;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int BasicString<TChar>::IndexOf(const TChar* str, int startIndex) const
{
	size_type pos = m_string->find(str, startIndex);
	if (pos == std::basic_string<TChar>::npos) {
		return -1;
	}
	return (int)pos;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int BasicString<TChar>::IndexOf(TChar ch, int startIndex) const
{
	size_type pos = m_string->find(ch, startIndex);
	if (pos == std::basic_string<TChar>::npos) {
		return -1;
	}
	return (int)pos;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int BasicString<TChar>::LastIndexOf(const TChar* str, int startIndex, int count, CaseSensitivity cs) const
{
	return StringUtils::LastIndexOf(GetCStr(), GetLength(), str, StringUtils::StrLen(str), startIndex, count, cs);
}
template<typename TChar>
int BasicString<TChar>::LastIndexOf(TChar ch, int startIndex, int count, CaseSensitivity cs) const
{
	return StringUtils::LastIndexOf(GetCStr(), GetLength(), &ch, 1, startIndex, count, cs);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool BasicString<TChar>::EndsWith(const TChar* str, CaseSensitivity cs) const
{
	return StringUtils::EndsWith(GetCStr(), GetLength(), str, StringUtils::StrLen(str), cs);
}
template<typename TChar>
bool BasicString<TChar>::EndsWith(TChar ch, CaseSensitivity cs) const
{
	return StringUtils::EndsWith(GetCStr(), GetLength(), &ch, 1, cs);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int BasicString<TChar>::Compare(const TChar* str, int count, CaseSensitivity cs) const
{
	return StringUtils::Compare(GetCStr(), str, count/*(count < 0) ? GetLength() : count*/, cs);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> BasicString<TChar>::Left(int count) const
{
	return StringUtils::Left(GetCStr(), count);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> BasicString<TChar>::Right(int count) const
{
	return StringUtils::Right(GetCStr(), count);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> BasicString<TChar>::Mid(int start, int count) const
{
	return StringUtils::Mid(GetCStr(), start, count);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
ArrayList< BasicString<TChar> > BasicString<TChar>::Split(const TChar* delim, StringSplitOptions option) const
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
	bool r = StrToInt(Trim().GetCStr(), &tmp);
	LN_THROW(r, InvalidFormatException);
	return tmp;
}

template<typename TChar>
bool BasicString<TChar>::ToInt(int* value) const
{
	return StrToInt(Trim().GetCStr(), value);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int BasicString<TChar>::GetLength() const
{
	return m_string->size();
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
const BasicString<TChar>& BasicString<TChar>::GetEmpty()
{
	static BasicString<TChar> str;
	return str;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::AssignTString(const TChar* str, int len)
{
	LN_SAFE_RELEASE(m_string);
	if (str == NULL || len == 0) {
		// 空の文字列になる場合は共有の空文字列を参照する
		LN_REFOBJ_SET(m_string, BasicStringCore::GetSharedEmpty());
	}
	else {
		m_string = LN_NEW BasicStringCore();	// 参照カウントは 1
		m_string->assign(str, (len < 0) ? StringUtils::StrLen(str) : len);
	}
	m_ref = m_string->c_str();
}

//-----------------------------------------------------------------------------
// ソース文字列の共有参照を切り、新しい BasicStringCore を確保する
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::Realloc()
{
	if (m_string->IsShared()) {
		BasicStringCore* old = m_string;
		m_string = LN_NEW BasicStringCore();	// 参照カウントは 1
		m_string->assign(old->c_str());
		m_ref = m_string->c_str();
		LN_SAFE_RELEASE(old);
	}
	else {
		// 共有参照されていなければそのまま使い続ければよい
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::Append(const TChar* str, int len)
{
	Realloc();	// 共有参照を切る
	m_string->append(str, (len < 0) ? StringUtils::StrLen(str) : len);
	m_ref = m_string->c_str();
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
template<typename TChar>
TChar& BasicString<TChar>::InternalGetAt(int index)
{
	LN_ASSERT(0 <= index && index < GetLength());
	return m_string->at(index);
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
template<typename TChar>
const TChar& BasicString<TChar>::InternalGetAt(int index) const
{
	LN_ASSERT(0 <= index && index < GetLength());
	return m_string->at(index);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
Text::Encoding* BasicString<TChar>::GetThisTypeEncoding() const
{
	if (sizeof(TChar) == sizeof(char))
	{
		// this のエンコーディングはシステム依存である
		return Text::Encoding::GetSystemMultiByteEncoding();
	}
	else if (sizeof(TChar) == sizeof(wchar_t))
	{
		// this のエンコーディングは wchar_t である
		return Text::Encoding::GetWideCharEncoding();
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
