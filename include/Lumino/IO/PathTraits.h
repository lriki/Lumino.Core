﻿#pragma once

LN_NAMESPACE_BEGIN

/**
	@brief	ネイティブ文字列に対するパス文字列操作をまとめたクラス
*/
class PathTraits
{
public:
#ifdef _WIN32
	static const uint8_t DirectorySeparatorChar		= '\\';
	static const uint8_t AltDirectorySeparatorChar	= '/';
	static const uint8_t VolumeSeparatorChar		= ':';
#else
	static const uint8_t DirectorySeparatorChar		= '/';
	static const uint8_t AltDirectorySeparatorChar	= 0x00;
	static const uint8_t VolumeSeparatorChar		= 0x00;
#endif

public:

	/// 文字がディレクトリセパレータ (DirectorySeparatorChar or AltDirectorySeparatorChar) であるかを判定する
	template<typename TChar>
	static bool IsSeparatorChar(TChar ch);

	/// ボリュームセパレータであるかを判定する
	template<typename TChar>
	static bool IsVolumeSeparatorChar(TChar ch);

	/// path がルートパスであるかを判定する ("C:/", "C:", "/" 等)
	template<typename TChar>
	static bool IsRootPath(const TChar* path);

	/// path が絶対パスであるかを判定する ("C:/AAA"、"/AAA" 両方判定)
	template<typename TChar>
	static bool IsAbsolutePath(const TChar* path, int len = -1);

	/** 文字列の末尾がディレクトリセパレータであるかを確認します。*/
	template<typename TChar>
	static bool EndWithSeparator(const TChar* path, int len = -1);

	/**
		@brief		パス文字列からディレクトリ部分を取り出す
		@param[in]	path	: ファイルまたはディレクトリのパス
		@details	出力例
					- "C:\MyDir\MySubDir\myfile.ext" → "C:\MyDir\MySubDir"
					- "C:\MyDir\MySubDir" → "C:\MyDir"
					- "C:\MyDir\" → "C:\MyDir"
					- "C:\MyDir" → "C:\"
					- "C:\" → ""
					- "/MyDir/" → "/MyDir"
					- "/MyDir" → "/"
					- "/" → ""
	*/
	template<typename TChar>
	static GenericString<TChar> GetDirectoryPath(const TChar* path);

	/**
		@brief		パス文字列の中から拡張子を含むファイル名の部分を返す
		@param[in]	path	: パス文字列
		@details	path が NULL の場合は空文字列を返します。
	*/
	template<typename TChar>
	static GenericString<TChar> GetFileName(const TChar* path);

	/**
		@brief		パス文字列の中から拡張子を含むファイル名の部分を返す
	*/
	template<typename TChar>
	static const TChar* GetFileNameSub(const TChar* path);

	/**
		@brief		指定したパス文字列のファイル名を拡張子を付けずに返します。
		@param[in]	path	: パス文字列
		@param[out]	outPath	: ファイル名を格納するバッファ (LN_MAX_PATH 文字の領域があること)
	*/
	template<typename TChar>
	static void GetFileNameWithoutExtension(const TChar* path, TChar* outPath);

	/**
		@brief		パス文字列の中から拡張子を返します。
		@param[in]	path	: パス文字列
		@param[out]	outExt	: '.' を含む拡張子を格納するバッファ (LN_MAX_PATH 文字の領域があること)
		@code
					"file.txt"			=> ".txt"
					"dir.a\file"		=> ""
					"file."				=> ""
					".", "a/", "/", ".."=> ""
		@endcode
	*/
	template<typename TChar>
	static void GetExtension(const TChar* path, TChar* outExt);
	template<typename TChar>
	static Result GetExtension(const TChar* path, bool withDot, GenericStringRef<TChar>* outRef) LN_NOEXCEPT;
	
	/**
		@brief		パス文字列を結合します。
		@param[in]	path1			: 1つ目のパス文字列
		@param[in]	path1Len		: 1つ目のパス文字列の長さ
		@param[in]	path1			: 2つ目のパス文字列
		@param[in]	path1Len		: 2つ目のパス文字列の長さ
		@param[out]	outPath			: 結合されたパスの格納先
		@param[in]	pathCapacity	: outPath のサイズ
	*/
	template<typename TChar>
	static Result Combine(const TChar* path1, int path1Len, const TChar* path2, int path2Len, TChar* outPath, int pathCapacity) LN_NOEXCEPT;

	/**
		@brief		パスを単純化する
		@param[in]	srcPath	: 元ファイルパス
		@param[in]	outPath	: 単純化したパスの格納先 (LN_MAX_PATH + 1 の領域があること)
	*/
	template<typename TChar>
	static void CanonicalizePath(const TChar* srcPath, TChar* outPath);

	/**
		@brief			パスを単純化する
		@param[in,out]	path	: 単純化するファイルパスが格納されている変数
	*/
	template<typename TChar>
	static void CanonicalizePath(GenericString<TChar>* path);

	/**
		@brief		パスを単純化する
		@return		-1 を返した場合は失敗。
		@details	このオーバーロードは現在内部処理用です。
	*/
	template<typename TChar>
	static int CanonicalizePath(const TChar* srcPath, size_t srcLen, TChar* outPath);

	/**
		@brief		パスに含まれるディレクトリセパレータを統一する。
	*/
	template<typename TChar>
	static void NormalizeSeparator(TChar* srcPath);

	/**
		@brief		2つのパス文字列を比較する
		@param[in]	path1	: パス文字列
		@param[in]	path2	: パス文字列
		@return		並べ替え順序を示す整数 (0 で一致)
		@details	DirectorySeparatorChar と AltDirectorySeparatorChar は等価とみなします。
					また、大文字小文字を区別しません。
	*/
	template<typename TChar>
	static int Compare(const TChar* path1, const TChar* path2);


	/**
		@brief		2つのパス文字列が等しいかをチェックします。
		@param[in]	path1	: パス文字列
		@param[in]	path2	: パス文字列
		@details	DirectorySeparatorChar と AltDirectorySeparatorChar は等価とみなします。
					また、大文字小文字を区別しません。
	*/
	template<typename TChar>
	static bool Equals(const TChar* path1, const TChar* path2);



	template<typename TChar>
	static int Compare(TChar ch1, TChar ch2, CaseSensitivity cs);

	template<typename TChar>
	static GenericString<TChar> DiffPath(const TChar* path1, int len1, const TChar* path2, int len2, CaseSensitivity cs);
};

LN_NAMESPACE_END
