﻿/**
	@file	String.h
*/

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

template<typename BaseType = char>	// GCC では BasicString 内部に定義できないので外に出している
class BasicStringTraits
{
public:
	typedef char XCHAR;
	typedef wchar_t YCHAR;
};
template<>
class BasicStringTraits<wchar_t>
{
public:
	typedef wchar_t XCHAR;
	typedef char YCHAR;
};

/**
	@brief		文字列のクラス
*/
template<typename TChar>
class BasicString
{
public:

	static const int MaxFormatLength = 2048;

	typedef typename BasicStringTraits<TChar>::XCHAR XCHAR;
	typedef typename BasicStringTraits<TChar>::YCHAR YCHAR;

	typedef BasicString<TChar>			StringT;
    typedef std::size_t size_type;		// need GCC

public:
	BasicString();
	~BasicString();

	// TChar (同一の文字型は assign だけにし、高速化を図る)
	BasicString(const BasicString& str);
	BasicString(const BasicString& str, size_type length);
	BasicString(const BasicString& str, size_type begin, size_type length);
	BasicString(const TChar* str);
	BasicString(const TChar* str, size_type length);
	BasicString(const TChar* str, size_type begin, size_type length);
	BasicString& operator=(const BasicString& right);
	BasicString& operator=(const std::basic_string<TChar>& right);
	BasicString& operator=(const TChar* right);

	// YCHAR (対の文字型は AssignCStr で文字コード変換を行う)
	BasicString(const BasicString<YCHAR>& str);
	BasicString(const BasicString<YCHAR>& str, size_type length);
	BasicString(const BasicString<YCHAR>& str, size_type begin, size_type length);
	BasicString(const YCHAR* str);
	BasicString(const YCHAR* str, size_type length);
	BasicString(const YCHAR* str, size_type begin, size_type length);
	BasicString& operator=(const BasicString<YCHAR>& right);
	BasicString& operator=(const std::basic_string<YCHAR>& right);
	BasicString& operator=(const YCHAR* right);

	// override std::basic_string
	//explicit BasicString(const std_basic_string& str) : std_basic_string(str) {}
	//		 BasicString(size_type count, TChar ch)	: std_basic_string(count, ch) {}
	
	// operators
	BasicString& operator+=(const BasicString& right);
	BasicString& operator+=(const std::basic_string<TChar>& right);
	BasicString& operator+=(const TChar* ptr);
	BasicString& operator+=(TChar ch);
	bool operator==(const BasicString& right) const;
	bool operator==(const TChar* right) const;
	bool operator < (const BasicString& right) const;
	bool operator < (const TChar* right) const;
	TChar& operator[](int index);
	const TChar& operator[](int index) const;
	operator const TChar*() const;

public:

	/// C言語形式の文字列ポインタを返す
	const TChar* GetCStr() const;
	
	/// 文字列が空の時にtrueを返す
	bool IsEmpty() const;

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
	void AssignCStr(const char* str, int begin, int length, bool* usedDefaultChar = NULL);
	void AssignCStr(const char* str, int length, bool* usedDefaultChar = NULL);					///< @overload AssignCStr
	void AssignCStr(const char* str, bool* usedDefaultChar = NULL);								///< @overload AssignCStr
	void AssignCStr(const wchar_t* str, int begin, int length, bool* usedDefaultChar = NULL);	///< @overload AssignCStr
	void AssignCStr(const wchar_t* str, int length, bool* usedDefaultChar = NULL);				///< @overload AssignCStr
	void AssignCStr(const wchar_t* str, bool* usedDefaultChar = NULL);							///< @overload AssignCStr

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
	void SetEmpty();

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
	int IndexOf(TChar ch,         int startIndex = 0) const;	///< @overload IndexOf

	/**
		@brief		文字列を検索し、最後に見つかったインデックスを返します。
		@param[in]	str			: 検索文字列
		@param[in]	startIndex	: 検索を開始するインデックス (-1 を指定すると、文字列の末尾から検索を開始する)
		@param[in]	count		: 検索する文字数 (-1 を指定すると、文字列の先頭まで検索する)
		@param[in]	cs			: 大文字と小文字の区別設定
		@return		見つかった文字列の開始インデックス。見つからなかった場合は -1。
		@details	startIndex の位置から文字列の先頭に向かう count 文字分の領域から str を検索します。
		@code
					String str = "abcdef";
					str.LastIndexOf("de");			// => 3
					str.LastIndexOf("bc", 2);		// => 1
					str.LastIndexOf("cd", 2);		// => -1	(検索範囲 "abc" の中に "cd" は存在しない)
					str.LastIndexOf("cd", 4, 3);	// => 2		(検索範囲 "cde" の中に "cd" は存在する)
					str.LastIndexOf("bc", 4, 3);	// => -1	(検索範囲 "cde" の中に "bc" は存在しない)
		@endcode
	*/
	int LastIndexOf(const TChar* str, int startIndex = -1, int count = -1, CaseSensitivity cs = CaseSensitivity_CaseSensitive) const;
	int LastIndexOf(TChar ch,         int startIndex = -1, int count = -1, CaseSensitivity cs = CaseSensitivity_CaseSensitive) const;	///< @overload LastIndexOf

	/**
		@brief		この文字列の末尾が、指定した文字列と一致するかを判断します。
		@param[in]	str			: 検索文字列
		@details	str2 が空文字の場合は必ず true が返ります。
		@code
					str = "file.txt";
					if (str.EndsWith(".txt")) {
						// 一致した
					}
		@endcodes
	*/
	bool EndsWith(const TChar* str, CaseSensitivity cs = CaseSensitivity_CaseSensitive) const;
	bool EndsWith(TChar ch,         CaseSensitivity cs = CaseSensitivity_CaseSensitive) const;	///< @overload EndsWith

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
	ArrayList< BasicString<TChar> > Split(const TChar* delim, StringSplitOptions option = StringSplitOptions_None) const;

	/**
		@brief		文字列を構成するバイト数を取得する
	*/
	int GetByteCount() const { return GetLength() * sizeof(TChar); }

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
	int GetLength() const;

public:
	/// 現在の環境で定義されている改行文字列を取得する
	static const StringT& GetNewLine();

	/// 空文字列を取得する
	static const StringT& GetEmpty();

private:
	void AssignTString(const TChar* str, int len);
	void Realloc();
	void Append(const TChar* str, int len);
	TChar& InternalGetAt(int index);
	const TChar& InternalGetAt(int index) const;

	Text::Encoding* GetThisTypeEncoding() const;

private:

	//template<typename TChar>
	class BasicStringCore
		: public std::basic_string<TChar, std::char_traits<TChar>, STLAllocator<TChar> >
	{
	public:
		//static const TChar EmptyString[1];	// "\0"


	public:
		BasicStringCore() : m_recCount(1) {}
		~BasicStringCore() {}

		inline bool IsShared() const { return (m_recCount > 1); }
		inline void AddRef() { ++m_recCount; }
		inline void Release()
		{
			--m_recCount;
			if (m_recCount <= 0) {
				delete this;
			}
		}

		static BasicStringCore* GetSharedEmpty() { return &m_sharedEmpty; }

	public:
		int		m_recCount;

		static BasicStringCore	m_sharedEmpty;
	};

	const TChar* m_ref;		///< 可変長の実引数にされることに備え、クラス先頭のメンバは m_string->c_str() を指しておく
	BasicStringCore/*<TChar>*/*	m_string;
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

#ifdef LN_DOXYGEN
/// @sa BasicStringCore
class String {};
#else
typedef BasicString<TCHAR>		String;
typedef BasicString<char>		StringA;
typedef BasicString<wchar_t>	StringW;
#endif

} // namespace Lumino