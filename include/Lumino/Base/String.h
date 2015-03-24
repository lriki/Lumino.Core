
#pragma once

#include <vector>
#include <string>
#include "Array.h"
#include "Common.h"

namespace Lumino
{
namespace Text { class Encoding; }

class ByteBuffer;

/// 大文字と小文字の区別指定
enum CaseSensitivity
{
	CaseSensitivity_CaseSensitive = 0,	///< 大文字と小文字を区別する
	CaseSensitivity_CaseInsensitive,	///< 大文字と小文字を区別しない
};

/// String::Split() の出力方法
enum StringSplitOptions
{
	StringSplitOptions_None = 0,			///< 出力は空の文字列を含む
	StringSplitOptions_RemoveEmptyEntries,	///< 出力は空の文字列を含まない
};

template< typename BaseType = char >
class ChTraitsBase
{
public:
	typedef char XCHAR;
	typedef wchar_t YCHAR;
};

template<>
class ChTraitsBase< wchar_t >
{
public:
	typedef wchar_t XCHAR;
	typedef char YCHAR;
};

/**
	@brief		文字列を表すクラス
	@details	std::basic_string をベースに機能拡張を行った文字列クラスです。
	
	@note		Replace 等の文字列を変更する関数は、可読性向上のために基本的に処理結果を戻り値で返している。
				VisualC++ や GCC 等のメジャーなコンパイラでは RVO が適用されるため、殆どの場合コピーコンストラクタは呼ばれない。
				値で返すことのオーバーヘッドは、コンストラクタ呼び出し1回のみ。

*/
template<typename TChar>
class BasicString : public std::basic_string<TChar>
{
public:
	static const int MaxFormatLength = 2048;

	typedef typename ChTraitsBase<TChar>::XCHAR XCHAR;
	typedef typename ChTraitsBase<TChar>::YCHAR YCHAR;

	typedef std::basic_string<TChar>	std_basic_string;
	typedef BasicString<TChar>			StringT;
    typedef std::size_t size_type;		// need GCC

public:
	BasicString() : std_basic_string() {}

	// TChar (同一の文字型は assign だけにし、高速化を図る)
	BasicString(const BasicString& str) : std_basic_string(str) {}
	BasicString(const BasicString& str, size_type length) : std_basic_string(str, length) {}
	BasicString(const BasicString& str, size_type begin, size_type length) : std_basic_string(str, begin, length) {}
	BasicString(const TChar* str) : std_basic_string(str) {}
	BasicString(const TChar* str, size_type length) : std_basic_string(str, length) {}
	BasicString(const TChar* str, size_type begin, size_type length) : std_basic_string(str + begin, length) {}	// str+begin にしないと、暗黙的コンストラクタの呼び出しが発生してしまう
	BasicString& operator=(const BasicString& right)				{ assign(right.GetCStr()); return (*this); }
	BasicString& operator=(const std::basic_string<TChar>& right)	{ assign(right); return (*this); }
	BasicString& operator=(const TChar* right)						{ assign(right); return (*this); }

	// YCHAR (対の文字型は AssignCStr で文字コード変換を行う)
	BasicString(const BasicString<YCHAR>& str) { AssignCStr(str.GetCStr()); }
	BasicString(const BasicString<YCHAR>& str, size_type length) { AssignCStr(str.GetCStr(), length); }
	BasicString(const BasicString<YCHAR>& str, size_type begin, size_type length) { AssignCStr(str.GetCStr(), begin, length); }
	BasicString(const YCHAR* str) { AssignCStr(str); }
	BasicString(const YCHAR* str, size_type length) { AssignCStr(str, length); }
	BasicString(const YCHAR* str, size_type begin, size_type length) { AssignCStr(str, begin, length); }
	BasicString& operator=(const BasicString<YCHAR>& right)			{ AssignCStr(right.GetCStr()); return (*this); }
	BasicString& operator=(const std::basic_string<YCHAR>& right)	{ AssignCStr(right.c_str()); return (*this); }
	BasicString& operator=(const YCHAR* right)						{ AssignCStr(right); return (*this); }

	// override std::basic_string
	explicit BasicString(const std_basic_string& str) : std_basic_string(str) {}
			 BasicString(size_type count, TChar ch)	: std_basic_string(count, ch) {}
	
	// operators
	BasicString& operator+=(const BasicString& right)		{ return static_cast<BasicString&>(std_basic_string::operator+=(right)); }
	BasicString& operator+=(const std_basic_string& right)	{ return static_cast<BasicString&>(std_basic_string::operator+=(right)); }	// += std::string
	BasicString& operator+=(const TChar* ptr)				{ return static_cast<BasicString&>(std_basic_string::operator+=(ptr)); }
	BasicString& operator+=(TChar ch)						{ return static_cast<BasicString&>(std_basic_string::operator+=(ch)); }
	operator const TChar*() const { return std_basic_string::c_str(); }

public:

	/// C言語形式の文字列ポインタを返す
	const TChar* GetCStr() const { return std_basic_string::c_str(); }
	
	/// 文字列が空の時にtrueを返す
	bool IsEmpty() const { return std_basic_string::empty(); }

	/**
		@brief		書式文字列と可変長引数リストから文字列を生成する
		@param[in]	format		: 書式文字列
		@param[in]	...			: 引数リスト
		@attention	生成される文字数は MaxFormatLength 以内に収まらなければなりません。(あふれた場合、例外をthrowします)
					これは、_vsnwprintf に相当する関数がWindows以外では使用できず、あらかじめ生成後の必要バッファサイズを測ることができないためです。<br>
					Format() は基本的に数値からの変換等、短い文字列にのみ使用し、文字列の連結は += 演算子等を使用してください。
					また、可変長引数リストにこのクラスのインスタンスを直接指定することはできません。
					GetCStr() 等で取得した文字列ポインタ型を指定してください。
	*/
	void Format(const TChar* format, ...);

	/**
		@brief		ネイティブ型文字列を割り当てる
		@param[in]	str				: 設定する文字列
		@param[in]	begin			: コピー範囲の開始インデックス (省略した場合は先頭から)
		@param[in]	length			: コピーする文字列 (省略した場合は終端 \0 まで)
		@param[in]	usedDefaultChar	: 変換不可文字をデフォルト文字 ('?') に変換したかどうか
		@details	この関数は char 用、wchar_t 用それぞれオーバーロードされます。
					型引数 TChar に対して必要であれば、
					それぞれ Encoding::GetSystemMultiByteEncoding()、Encoding::GetWideCharEncoding() で取得できるエンコーディングを使用して
					文字コードをの変換を行います。
					TChar と str の型が同じ場合は文字コードの変換を行いません。
	*/
	void AssignCStr(const char* str, size_type begin, size_type length, bool* usedDefaultChar = NULL);

	/// @copydoc AssignCStr
	void AssignCStr(const char* str, size_type length, bool* usedDefaultChar = NULL) { AssignCStr(str, 0, length, usedDefaultChar); }

	/// @copydoc AssignCStr
	void AssignCStr(const char* str, bool* usedDefaultChar = NULL) { AssignCStr(str, 0, std_basic_string::npos, usedDefaultChar); }

	/// @copydoc AssignCStr
	void AssignCStr(const wchar_t* str, size_type begin, size_type length, bool* usedDefaultChar = NULL);

	/// @copydoc AssignCStr
	void AssignCStr(const wchar_t* str, size_type length, bool* usedDefaultChar = NULL) { AssignCStr(str, 0, length, usedDefaultChar); }

	/// @copydoc AssignCStr
	void AssignCStr(const wchar_t* str, bool* usedDefaultChar = NULL) { AssignCStr(str, 0, std_basic_string::npos, usedDefaultChar); }

	/**
		@brief		指定したエンコーディングを使用し、文字列を変換して設定する
		@param[in]	buffer				: 変換元文字列が格納されているバッファ
		@param[in]	byteCount			: buffer のバイト数 (-1 で \0 まで)
		@param[in]	encoding			: 変換元文字エンコーディング
		@param[out]	usedDefaultChar		: 変換不可文字をデフォルト文字 ('?') に変換したかどうか
	*/
	void ConvertFrom(const void* buffer, size_t byteCount, const Text::Encoding* encoding, bool* usedDefaultChar = NULL);

	/**
		@brief		指定したエンコーディングを使用し、変換した文字列バッファを取得する
		@param[in]	encoding			: 変換先文字エンコーディング
		@param[out]	pUsedDefaultChar	: 変換不可文字をデフォルト文字 ('?') に変換したかどうか
		@return		\0終端文字は付加されません。GetSize() により使用バイト数を確認できます。
					使用後、Release() で開放する必要があります。
	*/
	ByteBuffer* ConvertTo(const Text::Encoding* encoding, bool* usedDefaultChar = NULL) const;

	/**
		@brief		空文字列を設定する
	*/
	void SetEmpty() { this->clear(); }

	/**
		@brief		部分文字列を取得する
		@param[in]	startIndex	: 文字列の 0 から始まる開始文字位置
		@param[in]	length		: 部分文字列の文字数 (省略した場合は末尾まで)
	*/
	StringT SubString(size_type startIndex = 0, size_type length = std::string::npos) const;

	/**
		@brief		文字列の先頭と末尾の空白を全て削除する
		@details	空白文字はロケールに依存します。日本語環境であれば全角スペースも空白扱いです。
	*/
	StringT Trim() const;

	/**
		@brief		この文字列から指定した文字をすべて取り除いた新しい文字列を返します。
		@param[in]	ch		: 削除する文字
		@param[in]	cs		: 大文字と小文字の区別設定
	*/
	StringT Remove(TChar ch, CaseSensitivity cs = CaseSensitivity_CaseSensitive) const;

	/**
		@brief		文字列の置換を行う
		@param[in]	from
		@param[in]	to
		@return		置換結果の文字列
		@details	from に一致するすべての文字列を to に置換します。
	*/
	StringT Replace(const TChar* from, const TChar* to) const;

	/**
		@brief		文字列を検索し、見つかった最初の文字のインデックスを返す
		@param[in]	str			: 検索文字列
		@param[in]	startIndex	: 検索を開始するインデックス (省略した場合は先頭から)
		@return		見つからなかった場合は -1
	*/
	int IndexOf(const TChar* str, int startIndex = 0) const;

	/**
		@brief		この文字列と、指定した文字列を比較します。
		@param[in]	str			: 比較文字列
		@param[in]	count		: 比較する文字数 (-1 の場合、GetLength() の値を使用します)
		@param[in]	cs			: 大文字と小文字の区別設定
		@return		str1 が str2 より小さい → 0 より小さい値
					str1 と str2 が等しい   → 0
					str1 が str2 より大きい → 0 より大きい値
	*/
	int Compare(const TChar* str, int count = -1, CaseSensitivity cs = CaseSensitivity_CaseSensitive) const;

	/**
		@brief		文字列の左側(先頭)から指定した文字数を抽出します。
		@param[in]	str		: 対象の文字列
		@param[in]	count	: 文字数
		@return		抽出された文字列
		@code
					String s(_T("abcdef"));
					s.Left(2)		=> _T("ab");
		@endcode
	*/
	BasicString<TChar> Left(int count) const;

	/**
		@brief		文字列の右側(末尾)から指定した文字数を抽出します。
		@param[in]	str		: 対象の文字列
		@param[in]	count	: 文字数
		@return		抽出された文字列
		@code
					String s(_T("abcdef"));
					s.Right(2)		=> _T("ef");
		@endcode
	*/
	BasicString<TChar> Right(int count) const;

	/**
		@brief		文字列の部分文字列を抽出します。
		@param[in]	str		: 対象の文字列
		@param[in]	start	: 開始文字インデックス
		@param[in]	count	: 文字数 (-1 の場合、末尾まで抽出する)
		@return		抽出された文字列
		@code
					String s(_T("abcdef"));
					s.Mid(2, 3)		=> _T("cde");
		@endcode
	*/
	BasicString<TChar> Mid(int start, int count = -1) const;

	/**
		@brief		文字列をデリミタで分割する
		@param[in]	delim	: デリミタ文字列
		@param[in]	option	: 分割方法
		@return		分割結果の文字列配列
	*/
	Array< BasicString<TChar> > Split(const TChar* delim, StringSplitOptions option = StringSplitOptions_None) const;

	/**
		@brief		文字列を構成するバイト数を取得する
	*/
	int GetByteCount() const { return (int)(std_basic_string::size() * sizeof(TChar)); }

	/**
		@brief		文字列を整数値に変換する
		@details	std::istringstream による変換です。
					全ての文字をパース出来ず途中で失敗した場合は例外が throw されます。
	*/
	int ToInt() const;

	/**
		@brief		文字列を整数値に変換する
		@details	パースに失敗した場合は例外を throw するのではなく false を返します。
					大量のループ内部等、例外によるパフォーマンスへの影響が懸念される場合に使用してください。
	*/
	bool ToInt(int* value) const;

	/// 終端 \0 までの文字数を返す (マルチバイト文字は考慮しない。CString::GetLength と同様の関数です)
	int GetLength() const { return (int)(std_basic_string::size()); }

public:
	/// 現在の環境で定義されている改行文字列を取得する
	static const StringT& GetNewLine();

private:
	Text::Encoding* GetThisTypeEncoding() const;
};

template<typename TChar>
inline BasicString<TChar> operator+(const BasicString<TChar>& left, const BasicString<TChar>& right)
{
	BasicString<TChar> str;
	str.reserve(left.size() + right.size());
	str += left;
	str += right;
	return str;
}
template<typename TChar>
inline BasicString<TChar> operator+(const BasicString<TChar>& left, const TChar* right)
{
	BasicString<TChar> str;
	str += left;
	str += right;
	return str;
}

typedef BasicString<TCHAR>		String;
typedef BasicString<char>		StringA;
typedef BasicString<wchar_t>	StringW;

} // namespace Lumino