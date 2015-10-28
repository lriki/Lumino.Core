﻿
#include "../Internal.h"
#include "../../include/Lumino/IO/PathName.h"
#include "../../include/Lumino/IO/DirectoryUtils.h"

LN_NAMESPACE_BEGIN
	
//=============================================================================
// DirectoryUtils
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<>
size_t DirectoryUtils::GetCurrentDirectory(char* outPath)
{
#ifdef LN_OS_WIN32
	return ::GetCurrentDirectoryA(LN_MAX_PATH, outPath);
#else
	return strlen(getcwd(outPath, LN_MAX_PATH));
#endif
}
template<>
size_t DirectoryUtils::GetCurrentDirectory(wchar_t* outPath)
{
#ifdef LN_OS_WIN32
	return ::GetCurrentDirectoryW(LN_MAX_PATH, outPath);
#else
	LN_THROW(0, NotImplementedException);
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#ifdef _WIN32
Array<String> DirectoryUtils::GetFiles(const TCHAR* drPath, const TCHAR* pattern)
{
	Array<String> fileList;
	PathName dirPathKey(drPath);
	String dirPatternPath(dirPathKey.GetStrEndSeparator());
	if (pattern) {
		dirPathKey.Append(pattern);
	}
	else {
		dirPathKey.Append(_T("*"));
	}

    // 検索開始
	WIN32_FIND_DATA fd;
	HANDLE h = ::FindFirstFile(dirPathKey.c_str(), &fd);
	if (h == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = ::GetLastError();
		if (dwError == ERROR_FILE_NOT_FOUND ||
			dwError == ERROR_NO_MORE_FILES){
			// これらは許可。空の配列を返す。
			return fileList;
		}
		else {
			LN_THROW(0, Win32Exception, dwError);
		}
	}

    do
	{
        if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
            // ディレクトリの場合
        }
        else {
            // ファイルの場合
			fileList.Add(dirPatternPath + fd.cFileName);
        }

		// 次のファイルを検索する
	} while (::FindNextFile(h, &fd));

	// 終了
	::FindClose(h);
	return fileList;
}

#else
Array<String> DirectoryUtils::GetFiles(const TCHAR* drPath, const TCHAR* pattern)
{
	LN_THROW(0, NotImplementedException);
	// http://www.syuhitu.org/other/dir.html
	Array<String> fileList;
	return fileList;
}
#endif

LN_NAMESPACE_END
