
#include <time.h>
#include "../Internal.h"
#include <Lumino/Base/StringTraits.h>
#include <Lumino/IO/FileSystem.h>
#include <Lumino/IO/PathName.h>
#include <Lumino/IO/DirectoryUtils.h>

namespace Lumino
{

//=============================================================================
// GenericPathName
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
GenericPathName<TChar>::GenericPathName(const GenericPathName& obj)
	: m_path(obj.m_path)
{
}
	
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void GenericPathName<TChar>::Assign(const char* path)
{
	m_path.AssignCStr(path);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void GenericPathName<TChar>::Assign(const wchar_t* path)
{
	m_path.AssignCStr(path);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void GenericPathName<TChar>::AssignUnderBasePath(const PathNameT& basePath, const char* relativePath)
{
	// フルパスの場合はそのまま割り当てる
	if (PathTraits::IsAbsolutePath(relativePath))
	{
		m_path.AssignCStr(relativePath);
	}
	// フルパスでなければ結合する
	else
	{
		m_path = basePath.m_path;
		// 末尾にセパレータがなければ付加する
		if (!m_path.EndsWith(Separator)) {
			m_path += Separator;
		}

		// relativePath 結合
		GenericStringT rel;
		rel.AssignCStr(relativePath);
		m_path += rel;
	}

	// 単純化する (.NET の Uri の動作に合わせる)
	PathTraits::CanonicalizePath(&m_path);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void GenericPathName<TChar>::AssignUnderBasePath(const PathNameT& basePath, const wchar_t* relativePath)
{
	// フルパスの場合はそのまま割り当てる
	if (PathTraits::IsAbsolutePath(relativePath))
	{
		m_path.AssignCStr(relativePath);
	}
	// フルパスでなければ結合する
	else
	{
		m_path = basePath.m_path;
		//if ((*m_path.rbegin()) != Separator) {	// 末尾セパレータ
		// 末尾がセパレータでなければ付加する
		if (!m_path.EndsWith(Separator)) {
			m_path += Separator;
		}

		// relativePath 結合
		GenericStringT rel;
		rel.AssignCStr(relativePath);
		m_path += rel;
	}

	// 単純化する (.NET の Uri の動作に合わせる)
	PathTraits::CanonicalizePath(&m_path);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void GenericPathName<TChar>::Append(const TChar* path)
{
	if (PathTraits::IsAbsolutePath(path)) {
		m_path = path;
	}
	else {
		if (m_path.GetLength() > 0 && !m_path.EndsWith(Separator))/*(*m_path.rbegin()) != Separator)*/ {	// 末尾セパレータ
			m_path += Separator;
		}
		m_path += path;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
const GenericString<TChar> GenericPathName<TChar>::GetStrEndSeparator() const
{
	GenericStringT newStr = m_path;
	if (!newStr.IsEmpty() && !newStr.EndsWith(Separator)/*(*newStr.rbegin()) != Separator*/) {	// 末尾セパレータ
		newStr += Separator;
	}
	return newStr;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
GenericPathName<TChar> GenericPathName<TChar>::GetWithoutExtension() const
{
	CaseSensitivity cs = FileSystem::GetFileSystemCaseSensitivity();

	// 最後の . 位置確認
	int dotPos = m_path.LastIndexOf('.', -1, -1, cs);
	if (dotPos == -1) {
		return (*this);		// . が無ければ何もせずそのまま返す
	}

	// 最後のセパレータ位置確認
	int separatorPos = m_path.LastIndexOf(Separator, -1, -1, cs);
	if (AltSeparator != 0) {
		int altPos = m_path.LastIndexOf(AltSeparator, -1, -1, cs);
		separatorPos = std::max(separatorPos, altPos);
	}

	if (dotPos < separatorPos) {
		return (*this);		// "dir.sub/file" のように、. の後ろにセパレータがあった。ファイル名に付く . ではないので、何もせずそのまま返す
	}

	return GenericPathName<TChar>(m_path.Left(dotPos));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool GenericPathName<TChar>::IsAbsolutePath() const
{
	return PathTraits::IsAbsolutePath(m_path.GetCStr());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool GenericPathName<TChar>::IsRoot() const
{
	return PathTraits::IsRootPath(m_path.GetCStr());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool GenericPathName<TChar>::CheckExt(const TChar* ext) const
{
	// TODO: 大文字小文字の区別をする
	return StringTraits::EndsWith(m_path.GetCStr(), m_path.GetLength(), ext, -1, CaseSensitivity_CaseInsensitive);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
GenericPathName<TChar> GenericPathName<TChar>::GetParent() const
{
	/* 参考：他のライブラリの、空文字やセパレータが無いなどで親ディレクトリが取れない時の動作
		- Qt (QFileInfo::dir())		… "." を返す
		- wxWidgets (wxFileName)	… "" を返す
		- Python (os.path)			… "" を返す
		- Ruby (Pathname)			… ".." を返す
		- Java (os.nio.Paths)		… null を返す
		- C# (Path, Uri)			… "" を返す (入力が "" だった場合は例外)
	*/
	return GenericPathName(PathTraits::GetDirectoryPath(m_path.GetCStr()).GetCStr());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
GenericPathName<TChar> GenericPathName<TChar>::CanonicalizePath() const
{
	TChar tmpPath[LN_MAX_PATH + 1];
	memset(tmpPath, 0, sizeof(tmpPath));
	PathTraits::CanonicalizePath(m_path.GetCStr(), tmpPath);
	return GenericPathName<TChar>(tmpPath);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
std::string GenericPathName<TChar>::ToLocalChar() const
{
	GenericString<char> tmp;
	tmp.AssignCStr(m_path.GetCStr());
	return std::string(tmp.GetCStr());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
GenericPathName<TChar> GenericPathName<TChar>::GetCurrentDirectory()
{
	static GenericPathName<TChar> path;

	TChar curDir[LN_MAX_PATH];
	size_t len = DirectoryUtils::GetCurrentDirectory(curDir);

	// メモリ確保を抑える。
	if (path.m_path != curDir) {
		path = curDir;
	}
	return path;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
GenericPathName<TChar> GenericPathName<TChar>::GetUniqueFilePathInDirectory(const PathNameT& directory, const TChar* filePrefix, const TChar* extName)
{
	GenericStringT dirPath = directory.GetStrEndSeparator();
	uint64_t key = static_cast<uint64_t>(::time(NULL));

	// 同番号のファイルがあればインクリメントしつつ空き番号を調べる
	int number = 1;
	GenericStringT filePath;
	GenericStringT work;
	do
	{
		if (filePrefix != NULL && extName != NULL) {
			filePath.Format(LN_T(TChar, "%s%s%llu%d%s"), dirPath.GetCStr(), filePrefix, key, number, extName);
		}
		else if (filePrefix == NULL && extName != NULL) {
			filePath.Format(LN_T(TChar, "%s%llu%d%s"), dirPath.GetCStr(), key, number, extName);
		}
		else if (filePrefix != NULL && extName == NULL) {
			filePath.Format(LN_T(TChar, "%s%s%llu%d"), dirPath.GetCStr(), filePrefix, key, number);
		}
		else {
			filePath.Format(LN_T(TChar, "%s%llu%d"), dirPath.GetCStr(), key, number);
		}

		number++;
	} while (FileSystem::Exists(filePath.GetCStr()));

	return PathNameT(filePath.GetCStr());
}


/// (こちらはファイル名だけを返す)
template<typename TChar>
GenericString<TChar> GenericPathName<TChar>::GetUniqueFileNameInDirectory(const PathNameT& directory, const TChar* filePrefix, const TChar* extName)
{
	return GetUniqueFilePathInDirectory(directory,filePrefix, extName).GetFileName();
}

// テンプレートのインスタンス化
template class GenericPathName<char>;
template class GenericPathName<wchar_t>;

} // namespace Lumino
