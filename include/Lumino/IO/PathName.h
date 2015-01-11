#pragma once

#include "../Base/String.h"
#include "PathUtils.h"

namespace Lumino
{

/**
	@brief		パス文字列を表すクラス
	@details	単純にパスセパレータで区切られた文字列を扱います。
				パスがフォルダを指すかファイルを指すかは区別しません。
				また、ファイルパスとして使用できない文字が含まれているかは確認しません。
				パス文字数の上限も確認しません。<br>

				セパレータは環境依存です。Windows では '\\'、UNIX 系では '/' です。
				※UNIX 系では '\\' をファイル名として使用可能です。

				このクラスに含まれる operator < 等の比較系の機能は、全て大文字小文字を区別しません。
				また、パスを単純化したうえで比較が行われます。
				※大文字小文字区別は Unix系対応の際に変わる可能性がありますが、いずれにしても Windows 環境では区別しません。

	@note		wxWidgets では wxFileName がこのクラスに相当する。
				大文字小文字を区別するかは、ビルド環境による (プリプロ分岐)。比較時にオプション引数を渡すことで区別の有無を変えることは可能。
				ちなみに、内部では単一の文字列ではなく、volume、複数のdir、filenaem、ext に分割された複数の文字列をメンバ変数として持っている。
				http://docs.wxwidgets.org/trunk/classwx_file_name.html#abe8d3ef3a404ed7050ad705d77d47eae

				Qt では QFileInfo / CDir がこのクラスに相当する。
				QFileInfo は「ドライブ名は大文字」等、内部で独自の使用が存在する。
				大文字小文字の区別は環境依存。(QDir::operator== の実装が参考になる)
				http://www.qtcentre.org/threads/38941-QFile-exists%28%29-and-QFileInfo-exists%28%29-and-case-sensitive-file-names
*/
template<typename TChar>
class BasicPathName
{
public:
	static const TChar Separator		= (TChar)PathUtils::DirectorySeparatorChar;
	static const TChar AltSeparator		= (TChar)PathUtils::AltDirectorySeparatorChar;
	static const TChar VolumeSeparator	= (TChar)PathUtils::VolumeSeparatorChar;

public:
	typedef BasicPathName<TChar> PathNameT;
	typedef BasicString<TChar> BasicStringT;

public:
	BasicPathName() {}

	/// @copydoc Assign
	explicit BasicPathName(const char* path) { Assign(path); }
	/// @copydoc Assign
	explicit BasicPathName(const wchar_t* path) { Assign(path); }
	/// @copydoc Assign
	explicit BasicPathName(const BasicStringT& path) { Assign(path); }

	/// @copydoc AssignUnderBasePath
	BasicPathName(const PathNameT& basePath, const char* relativePath) { AssignUnderBasePath(basePath, relativePath); }
	/// @copydoc AssignUnderBasePath
	BasicPathName(const PathNameT& basePath, const wchar_t* relativePath) { AssignUnderBasePath(basePath, relativePath); }
	/// @copydoc AssignUnderBasePath
	BasicPathName(const PathNameT& basePath, const BasicStringT& relativePath) { AssignUnderBasePath(basePath, relativePath); }
	/// @copydoc AssignUnderBasePath
	BasicPathName(const PathNameT& basePath, const PathNameT& relativePath) { AssignUnderBasePath(basePath, relativePath); }

	// operators
	BasicPathName& operator = (const BasicStringT& str)	{ Assign(str.c_str()); return (*this); }
	BasicPathName& operator = (const TChar* str) { Assign(str); return (*this); }
	bool operator < (const BasicPathName& right) const { return PathUtils::Compare(m_path.GetCStr(), right.m_path.GetCStr()) < 0; }
	bool operator < (const TChar* right) const { return PathUtils::Compare(m_path.GetCStr(), right) < 0; }
	operator const TChar*() const { return m_path.c_str(); }


public:

	/**
		@brief		パス文字列を割り当てる
		@param[in]	path		: パス文字列
	*/
	void Assign(const char* path);
	/// @copydoc Assign
	void Assign(const wchar_t* path);
	/// @copydoc Assign
	void Assign(const BasicStringT& path) { Assign(path.c_str()); }

	/**
		@brief		ベースパスと相対パスを連結して、パスを作成する
		@param[in]	basePath		: パス文字列
		@param[in]	relativePath	: パス文字列
		@details	relativePath がフルパスの場合は basePath を無視します。
	*/
	void AssignUnderBasePath(const PathNameT& basePath, const char* relativePath);
	/// @copydoc AssignUnderBasePath
	void AssignUnderBasePath(const PathNameT& basePath, const wchar_t* relativePath);
	/// @copydoc AssignUnderBasePath
	void AssignUnderBasePath(const PathNameT& basePath, const BasicStringT& relativePath) { AssignUnderBasePath(basePath, relativePath.c_str()); }
	/// @copydoc AssignUnderBasePath
	void AssignUnderBasePath(const PathNameT& basePath, const PathNameT& relativePath) { AssignUnderBasePath(basePath, relativePath.GetCStr()); }

	/**
		@brief		現在のパスに別のパス文字列を連結する
		@param[in]	path		: パス文字列
		@details	現在のパスの末尾にセパレータが無い場合は付加し、文字列を連結します。
					path が絶対パスであれば、現在のパスを置き換えます。
	*/
	void Append(const TChar* path);
	/// @copydoc Append
	void Append(const PathNameT& path) { Append(path.m_path.c_str()); }

	/// 空文字列を設定する
	void SetEmpty() { m_path.SetEmpty(); }

	/// パス文字列の中から拡張子を含むファイル名の部分を返す (空パスの場合は空文字列を返す)
	BasicStringT GetFileName() const { return PathUtils::GetFileName(m_path.c_str()); }

	/// パスの TChar 文字列を返す
	const TChar* GetCStr() const { return m_path.c_str(); }
	
	/// パス文字列を返す
	const BasicStringT& GetString() const { return m_path; }

	/// パス文字列を返す (末尾に必ずセパレータをひとつ付加する)
	const BasicString<TChar> GetStrEndSeparator() const;

	/// パスが空であるかを確認する
	bool IsEmpty() const { return m_path.empty(); }

	/// 絶対パスであるかを確認する
	bool IsAbsolutePath() const;

	/// ルートディレクトリであるかを確認する
	bool IsRoot() const;

	/// 指定された拡張子を持っているかを確認する (pExt の . の有無は問わない)
	bool CheckExt(const TChar* ext) const;

	/**
		@brief		親ディレクトリの PathName を返す
		@details	現在のパスがルートパス ('C:\' や '/') の場合、現在のパスを返します。
					それ以外に親ディレクトリを特定できない場合は空文字を返します。
					以下の例を参考にしてください。
		@code
					"C:/dir1/file1.txt"	→ "C:/dir1"
					"C:/dir1"			→ "C:/"
					"C:/"				→ "C:/"
					"C:"				→ "C:"
					"/"					→ "/"		(Unix)
					"file1.txt"			→ ""
					".."				→ ""
					"."					→ ""
					""					→ ""
		@endcode
	*/
	PathNameT GetParent() const;

	/**
		@brief		パスを単純化し、フルパスにしたものを返す
		@details	パスが相対パスの場合はカレントディレクトリを基準に、フルパスを求めます。
					また、Windows 環境では / は \ に置き換えられます。
	*/
	PathNameT CanonicalizePath() const;

	/// ローカルの char 文字列表現として返す
	std::string ToLocalChar() const;

	/**
		@brief		このパスと、別のパス文字列が等しいかをチェックする
		@param[in]	path	: 比較対象のパス文字列
		@details	Separator と AltSeparator は等価とみなします。
					また、大文字小文字を区別しません。
		@note		環境による大文字小文字の区別について…<br>
					区別の有無は、OSではなくファイルシステム依存。
					http://ja.wikipedia.org/wiki/%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB%E3%82%B7%E3%82%B9%E3%83%86%E3%83%A0#.E6.A9.9F.E8.83.BD
					これをプログラムから調べる有効な手段が見つからなかった…。
					ちなみに、Adobe等クロスプラットフォームで動く製品を出してるところも、この辺りは十分に対応しきれていない様子。
					http://helpx.adobe.com/jp/x-productkb/global/cpsid_83180.html
					http://www.clip-studio.com/clip_site/support/faq/detail/svc/52/tid/37429
	*/
	bool Equals(const TChar* path) const { return PathUtils::Equals(m_path.c_str(), path); }
	/// @copydoc Equals
	bool Equals(const PathNameT& path) const { return PathUtils::Equals(m_path.c_str(), path.GetCStr()); }
	/// @copydoc Equals
	bool Equals(const BasicStringT& path) const { return PathUtils::Equals(m_path.c_str(), path.c_str()); }

public:

	/**	
		@brief		あるフォルダ内でユニークなファイルパス(絶対パス)を生成して返す
		@param[in]	directory	: フォルダパス
		@param[in]	filePrefix	: ファイル名の接頭辞 または NULL
		@param[in]	extName		: ファイルの拡張子 (プレフィックス。.を含めること) または NULL
		@details	生成されるファイルパスは "<directory>/<filePrefix><ID><extName>" となります。
					ID は時間情報をキーとして生成、<br>
					filePrefix、extName が NULL の場合は空文字扱いで結合されます。
					例えば両方 NULL にすると、番号だけのファイル名になります。
	*/
	static PathNameT GetUniqueFilePathInDirectory(const PathNameT& directory, const TChar* filePrefix, const TChar* extName);

	/// (こちらはファイル名だけを返す)
	static BasicStringT GetUniqueFileNameInDirectory(const PathNameT& directory, const TChar* filePrefix, const TChar* extName);

private:
	BasicStringT	m_path;

private:
	//void ConvertSeparatorToInternal(BasicStringT* path);
};

typedef BasicPathName<TCHAR>	PathName;
typedef BasicPathName<char>		PathNameA;
typedef BasicPathName<wchar_t>	PathNameW;

} // namespace Lumino
