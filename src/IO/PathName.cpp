
#include <time.h>
#include "../Internal.h"
#include "../../include/Lumino/Base/StringUtils.h"
#include "../../include/Lumino/IO/FileUtils.h"
#include "../../include/Lumino/IO/PathName.h"

namespace Lumino
{
	
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicPathName<TChar>::Assign(const char* path)
{
	m_path.AssignCStr(path);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicPathName<TChar>::Assign(const wchar_t* path)
{
	m_path.AssignCStr(path);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicPathName<TChar>::AssignUnderBasePath(const PathNameT& basePath, const char* relativePath)
{
	// フルパスの場合はそのまま割り当てる
	if (PathUtils::IsAbsolutePath(relativePath))
	{
		m_path.AssignCStr(relativePath);
	}
	// フルパスでなければ結合する
	else
	{
		m_path = basePath.m_path;
		if ((*m_path.rbegin()) != Separator) {	// 末尾セパレータ
			m_path += Separator;
		}

		// relativePath 結合
		BasicStringT rel;
		rel.AssignCStr(relativePath);
		m_path += rel;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicPathName<TChar>::AssignUnderBasePath(const PathNameT& basePath, const wchar_t* relativePath)
{
	// フルパスの場合はそのまま割り当てる
	if (PathUtils::IsAbsolutePath(relativePath))
	{
		m_path.AssignCStr(relativePath);
	}
	// フルパスでなければ結合する
	else
	{
		m_path = basePath.m_path;
		if ((*m_path.rbegin()) != Separator) {	// 末尾セパレータ
			m_path += Separator;
		}

		// relativePath 結合
		BasicStringT rel;
		rel.AssignCStr(relativePath);
		m_path += rel;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicPathName<TChar>::Append(const TChar* path)
{
	if (PathUtils::IsAbsolutePath(path)) {
		m_path = path;
	}
	else {
		if (m_path.size() > 0 && (*m_path.rbegin()) != Separator) {	// 末尾セパレータ
			m_path += Separator;
		}
		m_path += path;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
const BasicString<TChar> BasicPathName<TChar>::GetStrEndSeparator() const
{
	BasicStringT newStr = m_path;
	if (!newStr.IsEmpty() && (*newStr.rbegin()) != Separator) {	// 末尾セパレータ
		newStr += Separator;
	}
	return newStr;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool BasicPathName<TChar>::IsAbsolutePath() const
{
	return PathUtils::IsAbsolutePath(m_path.c_str());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool BasicPathName<TChar>::IsRoot() const
{
	return PathUtils::IsRootPath(m_path.c_str());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool BasicPathName<TChar>::CheckExt(const TChar* ext) const
{
	return StringUtils::EndsWith(m_path.c_str(), m_path.size(), ext, -1, StringComparison_IgnoreCase);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicPathName<TChar> BasicPathName<TChar>::GetParent() const
{
	/* 参考：他のライブラリの、空文字やセパレータが無いなどで親ディレクトリが取れない時の動作
		- Qt (QFileInfo)			… "." を返す
		- wxWidgets (wxFileName)	… "" を返す
		- Python (os.path)			… "" を返す
		- Ruby (Pathname)			… ".." を返す
		- Java (os.nio.Paths)		… null を返す
		- C# (Path, Uri)			… "" を返す (入力が "" だった場合は例外)
	*/
	return BasicPathName(PathUtils::GetDirectoryPath(m_path.c_str()).c_str());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicPathName<TChar> BasicPathName<TChar>::CanonicalizePath() const
{
	TChar tmpPath[LN_MAX_PATH + 1];
	memset(tmpPath, 0, sizeof(tmpPath));
	PathUtils::CanonicalizePath(m_path.c_str(), tmpPath);
	return BasicPathName<TChar>(tmpPath);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
std::string BasicPathName<TChar>::ToLocalChar() const
{
	BasicString<char> tmp;
	tmp.AssignCStr(m_path.c_str());
	return std::string(tmp.c_str());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicPathName<TChar> BasicPathName<TChar>::GetUniqueFilePathInDirectory(const PathNameT& directory, const TChar* filePrefix, const TChar* extName)
{
	BasicStringT dirPath = directory.GetStrEndSeparator();
	uint64_t key = static_cast<uint64_t>(::time(NULL));

	// 同番号のファイルがあればインクリメントしつつ空き番号を調べる
	int number = 1;
	BasicStringT filePath;
	BasicStringT work;
	do
	{
		if (filePrefix != NULL && extName != NULL) {
			filePath.Format(LN_T(TChar, "%s%s%llu%d%s"), dirPath.c_str(), filePrefix, key, number, extName);
		}
		else if (filePrefix == NULL && extName != NULL) {
			filePath.Format(LN_T(TChar, "%s%llu%d%s"), dirPath.c_str(), key, number, extName);
		}
		else if (filePrefix != NULL && extName == NULL) {
			filePath.Format(LN_T(TChar, "%s%s%llu%d"), dirPath.c_str(), filePrefix, key, number);
		}
		else {
			filePath.Format(LN_T(TChar, "%s%llu%d"), dirPath.c_str(), key, number);
		}

		number++;
	} while(FileUtils::Exists(filePath.c_str()));

	return PathNameT(filePath.c_str());
}


/// (こちらはファイル名だけを返す)
template<typename TChar>
BasicString<TChar> BasicPathName<TChar>::GetUniqueFileNameInDirectory(const PathNameT& directory, const TChar* filePrefix, const TChar* extName)
{
	return GetUniqueFilePathInDirectory(directory,filePrefix, extName).GetFileName();
}

// テンプレートのインスタンス化
template class BasicPathName<char>;
template class BasicPathName<wchar_t>;

} // namespace Lumino
