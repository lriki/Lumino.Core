
#pragma once

#include <ctype.h>
#include <wctype.h>
#include <string>
//#include "TChar.h"
#include "STLUtils.h"
#include "Array.h"

namespace Lumino
{

enum NumberConversionResult
{
	NumberConversionResult_Success = 0,
	NumberConversionResult_ArgsError,
	NumberConversionResult_FormatError,
	NumberConversionResult_Overflow,
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
		@brief	NULL 終端文字列の長さを求めます。
	*/
	template<typename TChar>
	static size_t StrLen(const TChar* str);

	/**
		@brief	ASCII の小文字を大文字に変換します。
	*/
	template<typename TChar>
	static TChar ToUpper(TChar ch);

	/**
		@brief		文字が空白文字であるかを確認します。
		@details	標準空白類文字 ： 水平タブ（'\t'）、改行（'\n'）、垂直タブ（'\v'）、書式送り（'\f'）、復帰（'\r'）、空白（' '）
					ロケールの影響を受けます。(例：日本語環境であれば、wchar_t の全角スペースもスペースとして扱われます。)
	*/
	template<typename TChar>
	static bool IsSpace(TChar ch);

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


	static void StrNCpy(char* dest, size_t destSize, const char* src, int count) { strncpy_s(dest, destSize, src, count); }
	static void StrNCpy(wchar_t* dest, size_t destElementsSize, const wchar_t* src, int count) { wcsncpy_s(dest, destElementsSize, src, count); }	// 文字単位。バイト単位ではない

	/// strncmp の overload 実装
	static int StrNCmp(const char* str1, const char* str2, size_t count) { return strncmp(str1, str2, count); }
	static int StrNCmp(const wchar_t* str1, const wchar_t* str2, size_t count) { return wcsncmp(str1, str2, count); }
	static int StrNICmp(const char* str1, const char* str2, size_t count);
	static int StrNICmp(const wchar_t* str1, const wchar_t* str2, size_t count);

	/// strtod の overload 実装
	static double StrToD(const char* str, char** endptr) { return strtod(str, endptr); }
	static double StrToD(const wchar_t* str, wchar_t** endptr) { return wcstod(str, endptr); }

	/**
		@brief		文字列を検索し、見つかった最初の文字のインデックスを返します。
		@param[in]	str1		: 検索対象文字列
		@param[in]	str2		: 検索文字列
		@param[in]	startIndex	: 検索を開始するインデックス (省略した場合は先頭から)
		@return		見つかった文字列の開始インデックス。見つからなかった場合は -1。
	*/
	template<typename TChar>
	static int IndexOf(const TChar* str1, const TChar* str2, int startIndex = 0);

	/**
		@brief		文字列を検索し、最後に見つかったインデックスを返します。
		@param[in]	str1		: 検索対象文字列
		@param[in]	str2		: 検索文字列
		@param[in]	startIndex	: 検索を開始するインデックス
		@param[in]	count		: 検索する文字数
		@return		見つかった文字列の開始インデックス。見つからなかった場合は -1。
	*/
	template<typename TChar>
	static int LastIndexOf(const TChar* str1, int str1Len, const TChar* str2, int str2Len, int startIndex, int count, CaseSensitivity cs);

	/**
		@brief		2 つの文字列を比較します。
		@param[in]	str1		: 比較文字列
		@param[in]	str2		: 比較文字列
		@param[in]	count		: 比較する文字数
		@param[in]	cs			: 大文字と小文字の区別設定
		@return		str1 が str2 より小さい → 0 より小さい値
					str1 と str2 が等しい   → 0
					str1 が str2 より大きい → 0 より大きい値
	*/
	template<typename TChar>
	static int Compare(const TChar* str1, const TChar* str2, int count, CaseSensitivity cs = CaseSensitivity_CaseSensitive);

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
	static bool EndsWith(const TChar* str1, int len1, const TChar* str2, int len2, CaseSensitivity cs);

	/**
		@brief		文字列の左側(先頭)から指定した文字数を抽出します。
		@param[in]	str		: 対象の文字列
		@param[in]	count	: 文字数
		@return		抽出された文字列
	*/
	template<typename TChar>
	static BasicString<TChar> Left(const TChar* str, int count);

	/**
		@brief		文字列の右側(末尾)から指定した文字数を抽出します。
		@param[in]	str		: 対象の文字列
		@param[in]	count	: 文字数
		@return		抽出された文字列
	*/
	template<typename TChar>
	static BasicString<TChar> Right(const TChar* str, int count);

	/**
		@brief		文字列の部分文字列を抽出します。
		@param[in]	str		: 対象の文字列
		@param[in]	start	: 開始文字インデックス
		@param[in]	count	: 文字数 (-1 の場合、末尾まで抽出する)
		@return		抽出された文字列
	*/
	template<typename TChar>
	static BasicString<TChar> Mid(const TChar* str, int start, int count = -1);

	/**
		@brief		文字列をデリミタで分割する
		@param[in]	str		: 分割対象の文字列
		@param[in]	delim	: デリミタ文字列
		@param[in]	option	: 分割方法
		@return		分割結果の文字列配列
	*/
	template<typename TChar>
	static ArrayList< BasicString<TChar> > Split(const BasicString<TChar>& str, const TChar* delim, StringSplitOptions option = StringSplitOptions_None);

	/**
		@brief		[start] ～ [end - 1] の範囲の先頭が改行かを判別し、一致したら文字数を返す ("\r" か "\n" なら 1、"\r\n" なら 2)
	*/
	template<typename TChar>
	static int CheckNewLineSequence(const TChar* start, const TChar* end);
	
	/**
		@brief		ワイルドカード('*', '?')を考慮して文字列を比較する
		@param[in]	puttern		: ワイルドカードを含むパターン文字列
		@param[in]	str			: 比較対象の文字列
	*/
	template<typename TChar>
	static bool Match(const TChar* pattern, const TChar* str);

	/**
		@brief		文字列を整数値に変換します。
		@param[in]	str			: 変換元の文字列
		@param[in]	len			: str の文字数 (-1 を指定すると \0 まで変換する)
		@param[in]	base		: 基数 (0、2、8、10、16 のいずれかであること)
		@param[in]	outEndPtr	: スキャンの終了位置を格納する変数のアドレス
		@param[in]	outResult	: エラーコードを格納する変数のアドレス
		@return		変換結果の数値
		@details	次の書式に従い、文字列を数値に変換します。<br>
					[whitespace] [{+ | – }] [0 [{ x | X }]] [digits | letters]		<br>
					16 進数値のアルファベットは大文字と小文字を区別しません。		<br><br>
					
					基数に 0 を指定すると、文字列の先頭文字から基数を自動判別します。<br>
					"0x" または "0X" であれば 16 進数、"0" であれば 8 進数、それ以外であれば 10 進数です。
					基数に 8 または 16 が指定されている際、文字列の先頭は "0" または "0x" である必要はありません。<br><br>

					outEndPtr は C言語の strtol() のように使用します。
					この値が str+len と一致しなければ、文字列が全て読み取られなかったことを示します。
	*/
	template<typename TChar>
	static int8_t ToInt8(const TChar* str, int len = -1, int base = 0, const TChar** outEndPtr = NULL, NumberConversionResult* outResult = NULL);
	/// @copydoc ToInt8
	template<typename TChar>
	static uint8_t ToUInt8(const TChar* str, int len = -1, int base = 0, const TChar** outEndPtr = NULL, NumberConversionResult* outResult = NULL);
	/// @copydoc ToInt8
	template<typename TChar>
	static int16_t ToInt16(const TChar* str, int len = -1, int base = 0, const TChar** outEndPtr = NULL, NumberConversionResult* outResult = NULL);
	/// @copydoc ToInt8
	template<typename TChar>
	static uint16_t ToUInt16(const TChar* str, int len = -1, int base = 0, const TChar** outEndPtr = NULL, NumberConversionResult* outResult = NULL);
	/// @copydoc ToInt8
	template<typename TChar>
	static int32_t ToInt32(const TChar* str, int len = -1, int base = 0, const TChar** outEndPtr = NULL, NumberConversionResult* outResult = NULL);
	/// @copydoc ToInt8
	template<typename TChar>
	static uint32_t ToUInt32(const TChar* str, int len = -1, int base = 0, const TChar** outEndPtr = NULL, NumberConversionResult* outResult = NULL);
	/// @copydoc ToInt8
	template<typename TChar>
	static int64_t ToInt64(const TChar* str, int len = -1, int base = 0, const TChar** outEndPtr = NULL, NumberConversionResult* outResult = NULL);
	/// @copydoc ToInt8
	template<typename TChar>
	static uint64_t ToUInt64(const TChar* str, int len = -1, int base = 0, const TChar** outEndPtr = NULL, NumberConversionResult* outResult = NULL);

	/**
		@brief		文字列を実数値に変換します。
		@details	次の書式に従い、文字列を数値に変換します。	<br>
					[whitespace] [sign] [digits] [.digits] [ {e | E }[sign]digits]	<br><br>
					
					この変換処理はロケールに依存せず、'.' を小数点とみなします。
					標準関数の strtod はロケールに依存し、例えばフランス語として設定されている場合、
					',' を小数点として認識してしまいます。('.' はエラーとなります)
					JSON 等の '.' が小数点として定められている文字列を変換する場合、この関数を使用します。
	*/
	template<typename TChar>
	static double ToDouble(const TChar* str, int len = -1, const TChar** outEndPtr = NULL, NumberConversionResult* outResult = NULL);

};

} // namespace Lumino
