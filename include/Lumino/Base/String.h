/**
	@file	String.h
*/

#pragma once

#include <vector>
#include <string>
#include "Common.h"
#include "Array.h"
#include "ByteBuffer.h"

namespace Lumino
{
namespace Text { class Encoding; }

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

/**
	@brief		文字列を表すクラスです。

	@details	このクラスは任意の長さの文字配列を扱うユーティリティクラスです。<br>
				C文字列からの構築、連結と比較、抽出、検索と置換など、文字列に対する一般的な操作を備えています。
				操作中は必要に応じて文字列の長さを拡張します。<br>
				内部では、メモリ使用量を削減し、データの不必要なコピーを避けるためにコピーオンライトの共有を行います。


	@section	文字列型について

	GenericString は型引数として char または wchar_t を受け取ります。
	StringA と StringW はそれぞれ GenericString<char> と GenericString<wchar_t> のインスタンスです。	<br>
	String は、UNICODE または LN_UNICODE シンボルのどちらが定義されている場合は GenericString<wchar_t>、
	それ以外は GenericString<char> としてサポートされます。	

	String を使用する場合、併せて TCHAR 型および _T マクロを使用するべきです。
	これらは Windows プログラミングの国際対応で用いられるもので、本ライブラリでは Windows 以外の環境でも
	UNICODE または LN_UNICODE シンボルの定義に合わせて char または wchar_t 用に定義されます。


	@section	エンコーディング（文字コード）について

	GenericString とそのインスタンスである String、StringA、StringW は、いずれもエンコーディングを規定しません。
	規定されるのは型のみで、エンコーディングは環境に依存します。<br>
	- char 型
		- Windows		… ユーザーロケールに依存するマルチバイトエンコーディング (日本語 OSであれば Shift_JIS)
		- Linux, Mac	… UTF-8
	- wchar_t 型
		- Windows		… UTF-16
		- Linux, Mac	… UTF-32

	例えば VisualStudio (Windows) で LN_UNICODE シンボルを定義している場合、
	String は wchar_t 型であり、エンコーディングは UTF-16 として扱われます。

	現在の環境のエンコーディングは Encoding::GetTCharEncoding() により取得することができます。


	@section	比較について

	このクラスを通じて2つの文字列を比較する場合、NULL と 空文字列は同一視されます。<br>
	また、比較時にロケールは考慮しません。ロケールを考慮した比較を行う場合、Locale クラスの関数を使用してください。<br>

	なお、NULL を代入したり NULL で初期化した場合、インスタンスは空文字列として初期化されます。


*/
template<typename TChar>
class GenericString
{
public:
	static const int MaxFormatLength = 2048;

	typedef typename GenericStringTraits<TChar>::XCHAR XCHAR;
	typedef typename GenericStringTraits<TChar>::YCHAR YCHAR;

	typedef GenericString<TChar>	StringT;
    typedef std::size_t size_type;	// for GCC

public:
	GenericString();
	~GenericString();

	// TChar (同一の文字型は assign だけにし、高速化を図る)
	GenericString(const GenericString& str);
	GenericString(const GenericString& str, int length);
	GenericString(const GenericString& str, int begin, int length);
	GenericString(const TChar* str);
	GenericString(const TChar* str, int length);
	GenericString(const TChar* str, int begin, int length);

	// YCHAR (対の文字型は AssignCStr で文字コード変換を行う)
	GenericString(const GenericString<YCHAR>& str);
	GenericString(const GenericString<YCHAR>& str, int length);
	GenericString(const GenericString<YCHAR>& str, int begin, int length);
	GenericString(const YCHAR* str);
	GenericString(const YCHAR* str, int length);
	GenericString(const YCHAR* str, int begin, int length);

	/// @name Operators
	/// @{
	GenericString& operator=(const GenericString& right);
	GenericString& operator=(const std::basic_string<TChar>& right);
	GenericString& operator=(const TChar* right);
	GenericString& operator=(const GenericString<YCHAR>& right);
	GenericString& operator=(const std::basic_string<YCHAR>& right);
	GenericString& operator=(const YCHAR* right);

	GenericString& operator+=(const GenericString& right);
	GenericString& operator+=(const TChar* ptr);
	GenericString& operator+=(TChar ch);

	bool operator==(const GenericString& right) const	{ return Equals(right); }		///< @see Equals
	bool operator==(const TChar* right) const			{ return Equals(right); }		///< @see Equals
	bool operator!=(const GenericString& right) const	{ return !operator==(right); }	///< @see Equals
	bool operator!=(const TChar* right) const			{ return !operator==(right); }	///< @see Equals

	bool operator<(const GenericString& right) const;
	bool operator<(const TChar* right) const;
	bool operator>(const GenericString& right) const;
	bool operator>(const TChar* right) const;
	bool operator<=(const GenericString& right) const	{ return !operator>(right); }
	bool operator<=(const TChar* right) const			{ return !operator>(right); }
	bool operator>=(const GenericString& right) const	{ return !operator<(right); }
	bool operator>=(const TChar* right) const			{ return !operator<(right); }

	TChar& operator[](int index);
	const TChar& operator[](int index) const;

	operator const TChar*() const;
	/// @}

public:

	/// C言語形式の文字列ポインタを返す
	const TChar* GetCStr() const;
	
	/// 文字列が空の時にtrueを返す
	bool IsEmpty() const;

	/**
		@brief		この文字列の末尾に指定された文字列を追加します。
		@param[in]	str		: 追加する文字列
		@param[in]	len		: 追加する文字数 (-1 を指定すると \0 まで)
	*/
	void Append(const GenericString& str, int len = -1);
	void Append(const TChar* str, int len = -1);			///< @overload Append
	void Append(TChar ch);									///< @overload Append

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
	void ConvertFrom(const void* buffer, int byteCount, const Text::Encoding* encoding, bool* usedDefaultChar = NULL);

	/**
		@brief		指定したエンコーディングを使用し、変換した文字列バッファを取得する
		@param[in]	encoding			: 変換先文字エンコーディング
		@param[out]	usedDefaultChar		: 変換不可文字をデフォルト文字 ('?') に変換したかどうか
		@return		バッファ終端には encoding に合わせた \0 終端文字が付加されます。
	*/
	ByteBuffer ConvertTo(const Text::Encoding* encoding, bool* usedDefaultChar = NULL) const;

	/**
		@brief		空文字列を設定する
	*/
	void SetEmpty();

	/**
		@brief		部分文字列を取得する
		@param[in]	startIndex	: 文字列の 0 から始まる開始文字位置
		@param[in]	length		: 部分文字列の文字数 (省略した場合は末尾まで)
	*/
	StringT SubString(int startIndex = 0, int length = -1) const;

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
		@endcode
	*/
	bool EndsWith(const TChar* str, CaseSensitivity cs = CaseSensitivity_CaseSensitive) const;
	bool EndsWith(TChar ch,         CaseSensitivity cs = CaseSensitivity_CaseSensitive) const;	///< @overload EndsWith
	
	/**
		@brief		文字列が同一かを判断します。
		@param[in]	str		: この文字列と比較する文字列
		@details	大文字/小文字を区別し、ロケールに依存しない比較を行います。
					また、str が NULL の場合は空文字とみなして比較を行います。
	*/
	bool Equals(const GenericString& str) const;
	bool Equals(const TChar* str) const;			///< @overload Equals

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
	GenericString<TChar> Left(int count) const;

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
	GenericString<TChar> Right(int count) const;

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
	GenericString<TChar> Mid(int start, int count = -1) const;

	/**
		@brief		文字列をデリミタで分割する
		@param[in]	delim	: デリミタ文字列
		@param[in]	option	: 分割方法
		@return		分割結果の文字列配列
	*/
	Array< GenericString<TChar> > Split(const TChar* delim, StringSplitOptions option = StringSplitOptions_None) const;

	/**
		@brief		文字列を構成するバイト数を取得する
	*/
	int GetByteCount() const { return GetLength() * sizeof(TChar); }

	/**
		@brief		この文字列を整数値に変換します。
		@param[in]	base		: 基数 (0、2、8、10、16 のいずれかであること)
		@return		変換結果の数値
		@details	次の書式に従い、文字列を数値に変換します。<br>
					[whitespace] [{+ | – }] [0 [{ x | X }]] [digits | letters]		<br>
					16 進数値のアルファベットは大文字と小文字を区別しません。		<br><br>
					
					基数に 0 を指定すると、文字列の先頭文字から基数を自動判別します。<br>
					"0x" または "0X" であれば 16 進数、"0" であれば 8 進数、それ以外であれば 10 進数です。
					基数に 8 または 16 が指定されている際、文字列の先頭は "0" または "0x" である必要はありません。

		@exception	InvalidFormatException	指定された基数に対して有効な桁を示す数字以外の文字が含まれていました。または、書式の前後に空白以外の文字が存在しました。
		@exception	OverflowException		数値に変換する際にオーバーフローが発生しました。
	*/
	int8_t		ToInt8(int base = 0) const;
	int16_t		ToInt16(int base = 0) const;	///< @copydoc ToInt8
	int32_t		ToInt32(int base = 0) const;	///< @copydoc ToInt8
	int64_t		ToInt64(int base = 0) const;	///< @copydoc ToInt8
	uint8_t		ToUInt8(int base = 0) const;	///< @copydoc ToInt8
	uint16_t	ToUInt16(int base = 0) const;	///< @copydoc ToInt8
	uint32_t	ToUInt32(int base = 0) const;	///< @copydoc ToInt8
	uint64_t	ToUInt64(int base = 0) const;	///< @copydoc ToInt8

	/**
		@brief		この文字列を整数値に変換し、成否を返します。
		@param[in]	outValue	: 結果を格納する変数のポインタ (NULL を指定すると成否のみを返す)
		@param[in]	base		: 基数 (0、2、8、10、16 のいずれかであること)
		@return		正常に変換された場合は true。それ以外の場合は false。
		@details	例外が発生しない点を除けば ToInt8 等と同様です。
					大量のループの内部等、例外によるパフォーマンスへの影響が懸念される場合に使用してください。
		@see		ToInt8
	*/
	bool		TryToInt8(int8_t* outValue, int base = 0) const;
	bool		TryToInt16(int16_t* outValue, int base = 0) const;		///< @copydoc TryToInt8
	bool		TryToInt32(int32_t* outValue, int base = 0) const;		///< @copydoc TryToInt8
	bool		TryToInt64(int64_t* outValue, int base = 0) const;		///< @copydoc TryToInt8
	bool		TryToUInt8(uint8_t* outValue, int base = 0) const;		///< @copydoc TryToInt8
	bool		TryToUInt16(uint16_t* outValue, int base = 0) const;	///< @copydoc TryToInt8
	bool		TryToUInt32(uint32_t* outValue, int base = 0) const;	///< @copydoc TryToInt8
	bool		TryToUInt64(uint64_t* outValue, int base = 0) const;	///< @copydoc TryToInt8

	/// 終端 \0 までの文字数を返す (マルチバイト文字は考慮しない。CString::GetLength と同様の関数です)
	int GetLength() const;



public:
	/// 現在の環境で定義されている改行文字列を取得する
	static const StringT& GetNewLine();

	/// 空文字列を取得する
	static const StringT& GetEmpty();

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
	static GenericString Format(const GenericString& format, ...);
	static GenericString Format(const TChar* format, ...);				///< @overload Format

private:
	void AssignTString(const TChar* str, int len);
	void Realloc();
	TChar& InternalGetAt(int index);
	const TChar& InternalGetAt(int index) const;

	Text::Encoding* GetThisTypeEncoding() const;

private:
	class GenericStringCore
		: public std::basic_string<TChar, std::char_traits<TChar>, STLAllocator<TChar> >
	{
	public:
		GenericStringCore() : m_refCount(1) {}
		~GenericStringCore() {}

		inline bool IsShared() const { return (m_refCount > 1); }
		inline void AddRef() { ++m_refCount; }
		inline void Release()
		{
			--m_refCount;
			if (m_refCount <= 0)
			{
				if (this != GetSharedEmpty()) {		// グローバル変数として定義された String からの解放済み delete 対策
					delete this;
				}
			}
		}

		static GenericStringCore* GetSharedEmpty() { return &m_sharedEmpty; }

	public:
		int		m_refCount;

		static GenericStringCore	m_sharedEmpty;
	};

	const TChar* m_ref;		///< 可変長の実引数にされることに備え、クラス先頭のメンバは m_string->c_str() を指しておく
	GenericStringCore*	m_string;

#ifdef LN_GenericString_Extensions
	LN_GenericString_Extensions;
#endif
};

template<typename TChar>
inline GenericString<TChar> operator+(const GenericString<TChar>& left, const GenericString<TChar>& right)
{
	GenericString<TChar> str;
	str.reserve(left.size() + right.size());
	str += left;
	str += right;
	return str;
}
template<typename TChar>
inline GenericString<TChar> operator+(const GenericString<TChar>& left, const TChar* right)
{
	GenericString<TChar> str;
	str += left;
	str += right;
	return str;
}

#ifdef LN_DOXYGEN
/// @see GenericString
class String {};
/// @see GenericString
class StringA {};	
/// @see GenericString
class StringW {};		
#else
typedef GenericString<TCHAR>	String;
typedef GenericString<char>		StringA;
typedef GenericString<wchar_t>	StringW;
#endif

} // namespace Lumino
