
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
	// windows �̏ꍇ
	size_t len = StringUtils::GetLength(path);
	if (IsAbsolutePath(path) && len >= 2)
	{
		if (path[len - 1] == VolumeSeparatorChar) {
			return true;	// ������ : �ł��� �� "C:" ��
		}
		if (path[len - 2] == VolumeSeparatorChar &&
			(path[len - 1] == DirectorySeparatorChar || path[len - 1] == AltDirectorySeparatorChar))
		{
			return true;	// ������ \ �� / �ŁA���̑O�� : �ł��� �� "C:/" ��
		}
	}
	return false;
#else
	// UNIX �̏ꍇ�A/ �����ł��邩
	if (len == 1 && path[0] == L"/") {
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

	// UNIX ���[�g�p�X
	if (path[0] == '/') return true;

	// Windows �h���C�u���^�[ (: �̎��� / �ł��邩)
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


	/* �Q�l�F���̃��C�u�����́A�󕶎���Z�p���[�^�������ȂǂŐe�f�B���N�g�������Ȃ����̓���
									"C:"	"C:/"	"C:/file"
	- Qt (QFileInfo)			�c	"C:"	"C:/"	"C:/"
	- wxWidgets (wxFileName)	�c
	- Python (os.path)			�c
	- Ruby (Pathname)			�c	"C:.."	"C:/"	"C:/"
	- Java (os.nio.Paths)		�c 
	- C# (Path, Uri)			�c	""		"C:/"	"C:/"
	*/

	// ��납��O�ɒ��ׂāA�ŏ��� \\ �� / ��������Ƃ����T��
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

		// ���[�g�p�X�̖����͕K���Z�p���[�^�ɂ���
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
		// �Z�p���[�^��������Ȃ������B�������A���[�g�p�X�̏ꍇ�͋󕶎��ɂ��Ȃ��B
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

	// ��납��O�ɒ��ׂāA�ŏ��ɃZ�p���[�^��������Ƃ����T��
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
	char* canonPath = realpath(srcPath, outPath);
#endif
	LN_THROW(canonPath != NULL, ArgumentException);
}
//template void PathUtils::CanonicalizePath<char>(const TChar* srcPath, TChar* outPath);
//template void PathUtils::CanonicalizePath<wchar_t>(const TChar* srcPath, TChar* outPath);

/**
@brief		2�̃p�X��������r����
@param[in]	path1	: �p�X������
@param[in]	path2	: �p�X������
@return		���בւ��������������� (0 �ň�v)
@details	DirectorySeparatorChar �� AltDirectorySeparatorChar �͓����Ƃ݂Ȃ��܂��B
�܂��A�啶������������ʂ��܂���B
*/
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int PathUtils::Compare(const TChar* path1, const TChar* path2)
{
	// �܂��͐��K���B�Z�L�����e�B�I��������
	// https://www.jpcert.or.jp/java-rules/ids02-j.html
	TChar absPath1[LN_MAX_PATH + 1];
	TChar absPath2[LN_MAX_PATH + 1];
	memset(absPath1, 0, sizeof(absPath1));
	memset(absPath2, 0, sizeof(absPath2));
	CanonicalizePath(path1, absPath1);
	CanonicalizePath(path2, absPath2);

	TChar* s1 = absPath1;
	TChar* s2 = absPath2;

	// �啶����������ʂ����A�������������ԌJ��Ԃ�
	while (*s1 && *s2)
	{
		if (StringUtils::ToUpper(*s1) != StringUtils::ToUpper(*s2))
		{
			// �Z�p���[�^�̍��͋�ʂ��Ȃ�
			if ((*s1 == DirectorySeparatorChar || *s1 == AltDirectorySeparatorChar) &&
				(*s2 == DirectorySeparatorChar || *s2 == AltDirectorySeparatorChar)
				){
				// �p��
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
