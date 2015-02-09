
#pragma once

#include <string>
//#include "TChar.h"
#include "STLUtils.h"
#include "Array.h"

namespace Lumino
{

enum StringComparison
{
	StringComparison_Normal = 0,		///< 大文字小文字を区別して比較を行う
	StringComparison_IgnoreCase,		///< 大文字小文字を区別しないで比較を行う
};

/**
	@brief		文字列ユーティリティ
	@details	char および wchar_t 型文字列に対する各操作を行うユーティリティ関数郡です。
				処理の内容は型が違うだけで同一なので template 関数を使用しています。
				ただし、実装を cpp に隠蔽するため明示的インスタンス化を行っています。
				このため char まはた wchar_t 型のみにしか使えません。(ほとんど問題ないと思いますが)
*/
class StringUtils
{
public:

	/**
		@brief		NULL 終端文字列の長さを求める
	*/
	static size_t GetLength(const char* str) { return ::strlen(str); }

	/** @copydoc	GetLength */
	static size_t GetLength(const wchar_t* str) { return ::wcslen(str); }

	/**
		@brief		文字を大文字化する
	*/
	static char ToUpper(char ch) { return (char)::toupper(ch); }

	/** @copydoc	ToUpper */
	static wchar_t ToUpper(wchar_t ch) { return (wchar_t)::towupper(ch); }
	
	/**
		@brief		
		@attention	文字数を計る機能は保障しない。linux では wchar_t 用に文字列を計る関数が無いため。
	*/
	static int VSPrintf(char* out, int charCount, const char* format, va_list args);
	static int VSPrintf(wchar_t* out, int charCount, const wchar_t* format, va_list args);


	/**
		@brief		マルチバイト文字列をワイド文字列に変換する
		@details	input の文字コードは setlocale() に依存します。
					基本的にOSのAPIが返す文字列を変換したい時に使用してください。
	*/
	static void ConvertMultiToWide(std::wstring* out, const char* input, int inputLength);

	/**
		@brief		文字が空白文字であるかを確認する
		@details	標準空白類文字 ： 水平タブ（'\t'）、改行（'\n'）、垂直タブ（'\v'）、書式送り（'\f'）、復帰（'\r'）、空白（' '）
					ロケールの影響を受けます。(例：日本語環境であれば、wchar_t の全角スペースもスペースとして扱われます。)
	*/
	static bool IsSpace(char ch) { return isspace(ch) != 0; }

	/** @copydoc	IsSpace */
	static bool IsSpace(wchar_t ch) { return iswspace(ch) != 0; }

	static void StrNCpy(char* dest, size_t destSize, const char* src, int count) { strncpy_s(dest, destSize, src, count); }
	static void StrNCpy(wchar_t* dest, size_t destElementsSize, const wchar_t* src, int count) { wcsncpy_s(dest, destElementsSize, src, count); }	// 文字単位。バイト単位ではない

	/// strncmp の overload 実装
	static int StrNCmp(const char* str1, const char* str2, size_t count) { return strncmp(str1, str2, count); }
	static int StrNCmp(const wchar_t* str1, const wchar_t* str2, size_t count) { return wcsncmp(str1, str2, count); }

	/**
		@brief		文字列を検索し、見つかった最初の文字のインデックスを返す
		@param[in]	str1		: 検索対象文字列
		@param[in]	str2		: 検索文字列
		@param[in]	startIndex	: 検索を開始するインデックス (省略した場合は先頭から)
		@return		見つからなかった場合は -1
	*/
	template<typename TChar>
	static int IndexOf(const TChar* str1, const TChar* str2, int startIndex = 0);

	/**
		@brief		文字列の前後にある空白を除いた文字列の範囲を調べる
		@param[in]	
	*/
	template<typename TChar>
	static void Trim(const TChar* begin, int length, const TChar** outBegin, int* outLength);

	/**
		@brief		書式文字列と可変長引数リストから文字列を生成する
		@param[in]	format		: 書式文字列
		@param[in]	...			: 引数リスト
		@return		生成された文字列
		@attention	生成される文字数は MaxFormatLength 以内に収まらなければなりません。(あふれた場合、例外をthrowします)
					これは、_vsnwprintf に相当する関数がWindows以外では使用できず、あらかじめ生成後の必要バッファサイズを測ることができないためです。<br>
					Format() は基本的に数値からの変換等、短い文字列にのみ使用し、文字列の連結は += 演算子等を使用してください。
					また、可変長引数リストに文字列クラスのインスタンスを直接指定することはできません。
					GetCStr() 等で取得した文字列ポインタ型を指定してください。
	*/
	template<typename TChar>
	static BasicString<TChar> Format(const TChar* format, ...);

	/**
		@brief		書式文字列と可変長引数リストから文字列を生成する
	*/
	template<typename TChar>
	static void FormatVAList(const TChar* format, va_list args, BasicString<TChar>* out);

	/**
		@brief		文字列の末尾が一致するかを調べる
		@details	str2 が空文字の場合は必ず true が返ります。
		@code
		str = "abc def";
		if (EndsWith(str, -1, "def", -1)) {
			// 一致した
		}
		@endcodes
	*/
	template<typename TChar>
	static bool EndsWith(const TChar* str1, int len1, const TChar* str2, int len2, StringComparison comparisonType);
	
	/**
		@brief		文字列をデリミタで分割する
		@param[in]	str		: 分割対象の文字列
		@param[in]	delim	: デリミタ文字列
		@param[in]	option	: 分割方法
		@return		分割結果の文字列配列
	*/
	template<typename TChar>
	static Array< BasicString<TChar> > Split(const BasicString<TChar>& str, const TChar* delim, StringSplitOptions option = StringSplitOptions_None);

	/**
		@brief		[start] ～ [end - 1] の範囲の先頭が改行かを判別し、一致したら文字数を返す ("\r" か "\n" なら 1、"\r\n" なら 2)
	*/
	template<typename TChar>
	static int CheckNewLineSequence(const TChar* start, const TChar* end);
};

} // namespace Lumino
