#pragma once

namespace Lumino
{

/**
	@brief	ネイティブ文字列に対するパス文字列操作をまとめたクラス
*/
class PathUtils
{
public:
#ifdef _WIN32
	static const uint8_t DirectorySeparatorChar		= '\\';
	static const uint8_t AltDirectorySeparatorChar	= '/';
	static const uint8_t VolumeSeparatorChar		= ':';
#else
	static const uint8_t DirectorySeparatorChar		= '/';
	static const uint8_t AltDirectorySeparatorChar	= NULL;
	static const uint8_t VolumeSeparatorChar		= NULL;
#endif

public:

	/// path がルートパスであるかを判定する ("C:/", "C:", "/" 等)
	template<typename TChar>
	static bool IsRootPath(const TChar* path);

	/// path が絶対パスであるかを判定する ("C:/AAA"、"/AAA" 両方判定)
	template<typename TChar>
	static bool IsAbsolutePath(const TChar* path);

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
	static BasicString<TChar> GetDirectoryPath(const TChar* path);

	/**
		@brief		パス文字列の中から拡張子を含むファイル名の部分を返す
		@param[in]	path	: パス文字列
		@details	path が NULL の場合は空文字列を返します。
	*/
	template<typename TChar>
	static BasicString<TChar> GetFileName(const TChar* path);

	/**
		@brief		パスを単純化する
		@param[in]	srcPath	: 元ファイルパス
		@param[in]	outPath	: 単純化したパスの格納先 (LN_MAX_PATH + 1 の領域があること)
	*/
	template<typename TChar>
	static void CanonicalizePath(const TChar* srcPath, TChar* outPath);

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
		@brief		2つのパス文字列が等しいかをチェックする
		@param[in]	path1	: パス文字列
		@param[in]	path2	: パス文字列
		@details	DirectorySeparatorChar と AltDirectorySeparatorChar は等価とみなします。
					また、大文字小文字を区別しません。
	*/
	template<typename TChar>
	static bool Equals(const TChar* path1, const TChar* path2);
};

} // namespace Lumino
