
#include "../Internal.h"
#include "../../include/Lumino/Base/StringUtils.h"
#include "../../include/Lumino/IO/PathUtils.h"

namespace Lumino
{
	
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool PathUtils::IsRootPath(const TChar* path)
{
#ifdef LN_WIN32
	// windows の場合
	size_t len = StringUtils::GetLength(path);
	if (IsAbsolutePath(path) && len >= 2)
	{
		if (path[len - 1] == VolumeSeparatorChar) {
			return true;	// 末尾が : である → "C:" 等
		}
		if (path[len - 2] == VolumeSeparatorChar &&
			(path[len - 1] == DirectorySeparatorChar || path[len - 1] == AltDirectorySeparatorChar))
		{
			return true;	// 末尾が \ か / で、その前が : である → "C:/" 等
		}
	}
	return false;
#else
	// UNIX の場合、/ だけであるか
	size_t len = StringUtils::GetLength(path);
	if (len == 1 && path[0] == '/') {
		return true;
	}
	return false;
#endif
}
template bool PathUtils::IsRootPath<char>(const char* path);
template bool PathUtils::IsRootPath<wchar_t>(const wchar_t* path);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool PathUtils::IsAbsolutePath(const TChar* path)
{
	LN_THROW(path != NULL, ArgumentException);

	// UNIX ルートパス
	if (path[0] == '/') return true;

	// Windows ドライブレター (: の次が / であるか)
	for (; *path; ++path) {
		if (path[0] == ':') {
			return true;
			//if (path[1] == '/' || path[1] == '\\') {
			//	return true;
			//}
		}
	}
	return false;
}
template bool PathUtils::IsAbsolutePath<char>(const char* path);
template bool PathUtils::IsAbsolutePath<wchar_t>(const wchar_t* path);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> PathUtils::GetDirectoryPath(const TChar* path)
{
	LN_THROW(path != NULL, ArgumentException);


	/* 参考：他のライブラリの、空文字やセパレータが無いなどで親ディレクトリが取れない時の動作
									"C:"	"C:/"	"C:/file"
	- Qt (QFileInfo)			…	"C:"	"C:/"	"C:/"
	- wxWidgets (wxFileName)	…
	- Python (os.path)			…
	- Ruby (Pathname)			…	"C:.."	"C:/"	"C:/"
	- Java (os.nio.Paths)		… 
	- C# (Path, Uri)			…	""		"C:/"	"C:/"
	*/

	// 後ろから前に調べて、最初に \\ か / が見つかるところを探す
	int pos = StringUtils::GetLength(path);
	TChar lastSep = 0;
	for ( ; pos >= 0; --pos ) {
		if ( path[pos] == '\\' || path[pos] == '/' ) {
			lastSep = path[pos];
			break;
		}
	}

	BasicString<TChar> str;
	if (pos >= 0) {
		str.assign(path, pos);

		// ルートパスの末尾は必ずセパレータにする
		if (IsRootPath(str.GetCStr())) {
			if ((*str.rbegin() != DirectorySeparatorChar) && (*str.rbegin() != AltDirectorySeparatorChar)) {
				if (lastSep != 0) {
					str += (const char)lastSep;
				}
				else {
					str += (const char)DirectorySeparatorChar;
				}
			}
		}

	}
	else {
		// セパレータが見つからなかった。ただし、ルートパスの場合は空文字にしない。
		if (IsRootPath(path)) {
			str = path;
		}
	}

	

	return str;
}
template BasicString<char> PathUtils::GetDirectoryPath<char>(const char* path);
template BasicString<wchar_t> PathUtils::GetDirectoryPath<wchar_t>(const wchar_t* path);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> PathUtils::GetFileName(const TChar* path)
{
	int len = StringUtils::GetLength(path);
	int pos = len - 1;

	// 後ろから前に調べて、最初にセパレータが見つかるところを探す
	for ( ; pos >= 0; --pos ) {
		if ( path[pos] == DirectorySeparatorChar || path[pos] == AltDirectorySeparatorChar || path[pos] == VolumeSeparatorChar ) {
			return BasicString<TChar>(&path[pos + 1], len - pos - 1);
		}
	}
	return BasicString<TChar>(path);
}
template BasicString<char> PathUtils::GetFileName(const char* path);
template BasicString<wchar_t> PathUtils::GetFileName(const wchar_t* path);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<>
void PathUtils::CanonicalizePath(const char* srcPath, char* outPath)
{
#ifdef _WIN32
	char* canonPath = _fullpath(outPath, srcPath, LN_MAX_PATH);
#else
	char* canonPath = realpath(srcPath, outPath);
#endif
	LN_THROW(canonPath != NULL, ArgumentException);
}
template<>
void PathUtils::CanonicalizePath(const wchar_t* srcPath, wchar_t* outPath)
{
#ifdef _WIN32
	wchar_t* canonPath = _wfullpath(outPath, srcPath, LN_MAX_PATH);
#else
	char mbcsSrc[LN_MAX_PATH];
	if (wcstombs(mbcsSrc, srcPath, LN_MAX_PATH) < 0) {
		LN_THROW(0, IOException);
	}

	char mbcsOut[LN_MAX_PATH];
	char* canonPath = realpath(mbcsSrc, mbcsOut);
	
	//char mbcsSrc[PATH_MAX];
	if (mbstowcs(outPath, mbcsOut, LN_MAX_PATH) < 0) {
		LN_THROW(0, IOException);
	}
#endif
	LN_THROW(canonPath != NULL, ArgumentException);
}
//template void PathUtils::CanonicalizePath<char>(const TChar* srcPath, TChar* outPath);
//template void PathUtils::CanonicalizePath<wchar_t>(const TChar* srcPath, TChar* outPath);

/**
@brief		2つのパス文字列を比較する
@param[in]	path1	: パス文字列
@param[in]	path2	: パス文字列
@return		並べ替え順序を示す整数 (0 で一致)
@details	DirectorySeparatorChar と AltDirectorySeparatorChar は等価とみなします。
また、大文字小文字を区別しません。
*/
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int PathUtils::Compare(const TChar* path1, const TChar* path2)
{
	// まずは正規化。セキュリティ的推奨事項
	// https://www.jpcert.or.jp/java-rules/ids02-j.html
	TChar absPath1[LN_MAX_PATH + 1];
	TChar absPath2[LN_MAX_PATH + 1];
	memset(absPath1, 0, sizeof(absPath1));
	memset(absPath2, 0, sizeof(absPath2));
	CanonicalizePath(path1, absPath1);
	CanonicalizePath(path2, absPath2);

	TChar* s1 = absPath1;
	TChar* s2 = absPath2;

	// 大文字小文字区別せず、文字が等しい間繰り返す
	while (*s1 && *s2)
	{
		if (StringUtils::ToUpper(*s1) != StringUtils::ToUpper(*s2))
		{
			// セパレータの差は区別しない
			if ((*s1 == DirectorySeparatorChar || *s1 == AltDirectorySeparatorChar) &&
				(*s2 == DirectorySeparatorChar || *s2 == AltDirectorySeparatorChar)
				){
				// 継続
			}
			else {
				return ((StringUtils::ToUpper(*s1) - StringUtils::ToUpper(*s2)));
			}
		}
		s1++;
		s2++;
	}

	return ((StringUtils::ToUpper(*s1) - StringUtils::ToUpper(*s2)));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
bool PathUtils::Equals(const TChar* path1, const TChar* path2)
{
	return Compare(path1, path2) == 0;
}
template bool PathUtils::Equals<char>(const char* path1, const char* path2);
template bool PathUtils::Equals<wchar_t>(const wchar_t* path1, const wchar_t* path2);

} // namespace Lumino
