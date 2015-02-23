
#include "../Internal.h"
#include <sstream>
#include "../../include/Lumino/Base/RefBuffer.h"
#include "../../include/Lumino/Text/Encoding.h"
#include "../../include/Lumino/Base/StringUtils.h"
#include "../../include/Lumino/Base/String.h"
#include "../../include/Lumino/Platform/Environment.h"


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
		ConvertFrom(str + begin, len, Text::Encoding::GetSystemMultiByteEncoding());
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
		ConvertFrom(str + begin, len, Text::Encoding::GetWideCharEncoding());
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::ConvertFrom(const void* buffer, int byteCount, const Text::Encoding* encoding, bool* outUsedDefaultChar)
{
	LN_THROW(encoding, ArgumentException);

	Text::Encoding* thisTypeEncoding = GetThisTypeEncoding();

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
		Text::EncodingConversionResult info;
		RefPtr<RefBuffer> tmpBuffer(
			Text::Encoding::Convert(buffer, byteCount, encoding, thisTypeEncoding,
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
RefBuffer* BasicString<TChar>::ConvertTo(const Text::Encoding* encoding, bool* outUsedDefaultChar) const
{
	//size_t bytesUsed;
	//size_t charsUsed;
	//bool usedDefaultChar;
	Text::EncodingConversionResult info;

	RefBuffer* buf = Text::Encoding::Convert(
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
	size_t from_length = StringUtils::StrLen(from);
	size_t to_length = StringUtils::StrLen(to);

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
