
#include "../Internal.h"
#include <Lumino/IO/PathName.h>
#include <Lumino/IO/DirectoryUtils.h>
#include <Lumino/IO/PathTraits.h>

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




#if defined(LN_OS_WIN32)
//=============================================================================
// GenericFileFinder
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
static void MakePattern(const GenericStringRef<TChar>& path, TChar* pattern)
{
	int len = path.CopyTo(pattern, LN_MAX_PATH);
	if (!PathTraits::IsSeparatorChar(pattern[len]))
	{
		LN_THROW(len < LN_MAX_PATH - 1, ArgumentException);
		pattern[len] = '/';
		len++;
	}
	LN_THROW(len < LN_MAX_PATH - 1, ArgumentException);
	pattern[len] = '*';
	len++;
	pattern[len] = '\0';
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
GenericFileFinder<char>::GenericFileFinder(const GenericStringRef<char>& dirPath)
	: GenericFileFinderBase(dirPath)
	, m_fh(INVALID_HANDLE_VALUE)
{
	char pattern[LN_MAX_PATH];
	MakePattern(dirPath, pattern);

	m_fh = ::FindFirstFileA(pattern, &m_fd);
	if (m_fh == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = ::GetLastError();
		if (dwError == ERROR_FILE_NOT_FOUND ||
			dwError == ERROR_NO_MORE_FILES)
		{
			SetCurrentFileName((char*)NULL);
		}
		else
		{
			LN_THROW(0, Win32Exception, dwError);
		}
	}
	else
	{
		SetCurrentFileName(m_fd.cFileName);
		if (strcmp(m_fd.cFileName, ".") == 0 || strcmp(m_fd.cFileName, "..") == 0)
		{
			Next();
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
GenericFileFinder<char>::~GenericFileFinder()
{
	if (m_fh != INVALID_HANDLE_VALUE)
	{
		::FindClose(m_fh);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool GenericFileFinder<char>::Next()
{
	do
	{
		if (::FindNextFileA(m_fh, &m_fd) != 0)
		{
			SetCurrentFileName(m_fd.cFileName);
		}
		else
		{
			m_fd.cFileName[0] = '\0';
			SetCurrentFileName((char*)NULL);
		}
	} while (strcmp(m_fd.cFileName, ".") == 0 || strcmp(m_fd.cFileName, "..") == 0);

	return !GetCurrent().IsEmpty();
}


//=============================================================================
// GenericFileFinder
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
GenericFileFinder<wchar_t>::GenericFileFinder(const GenericStringRef<wchar_t>& dirPath)
	: GenericFileFinderBase(dirPath)
	, m_fh(INVALID_HANDLE_VALUE)
{
	wchar_t pattern[LN_MAX_PATH];
	MakePattern(dirPath, pattern);

	m_fh = ::FindFirstFileW(pattern, &m_fd);
	if (m_fh == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = ::GetLastError();
		if (dwError == ERROR_FILE_NOT_FOUND ||
			dwError == ERROR_NO_MORE_FILES)
		{
			SetCurrentFileName((wchar_t*)NULL);
		}
		else
		{
			LN_THROW(0, Win32Exception, dwError);
		}
	}
	else
	{
		SetCurrentFileName(m_fd.cFileName);
		if (wcscmp(m_fd.cFileName, L".") == 0 || wcscmp(m_fd.cFileName, L"..") == 0)
		{
			Next();
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
GenericFileFinder<wchar_t>::~GenericFileFinder()
{
	if (m_fh != INVALID_HANDLE_VALUE)
	{
		::FindClose(m_fh);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool GenericFileFinder<wchar_t>::Next()
{
	do
	{
		if (::FindNextFileW(m_fh, &m_fd) != 0)
		{
			SetCurrentFileName(m_fd.cFileName);
		}
		else
		{
			m_fd.cFileName[0] = '\0';
			SetCurrentFileName((wchar_t*)NULL);
		}
	} while (wcscmp(m_fd.cFileName, L".") == 0 || wcscmp(m_fd.cFileName, L"..") == 0);

	return !GetCurrent().IsEmpty();
}
#elif defined(LN_OS_FAMILY_UNIX)



template<typename TChar>
GenericFileFinder<TChar>::GenericFileFinder(const GenericStringRef<TChar>& dirPath)
	: GenericFileFinderBase<TChar>(dirPath)
{
	StringA t;
	t.AssignCStr(GenericFileFinderBase<TChar>::m_dirPath.c_str());
	Initializ(t.c_str());
}

//template<typename TChar>
//GenericFileFinder<TChar>::GenericFileFinder(const GenericStringRef<char>& dirPath)
//	: GenericFileFinderBase(dirPath)
//{
//	StringA t = dirPath;
//	Initializ(t.c_str());
//}
//
//template<typename TChar>
//GenericFileFinder<TChar>::GenericFileFinder(const GenericStringRef<wchar_t>& dirPath)
//	: GenericFileFinderBase(dirPath)
//{
//	StringA t = StringA::FromNativeWCharString(dirPath.GetBegin(), dirPath.GetLength());
//	Initializ(t.c_str());
//}

template<typename TChar>
GenericFileFinder<TChar>::~GenericFileFinder()
{
	if (m_dir != NULL)
	{
		closedir(m_dir);
	}
}

template<typename TChar>
void GenericFileFinder<TChar>::Initializ(const char* dirPath)
{
	m_dir = opendir(dirPath);
	LN_THROW(m_dir != NULL, IOException, dirPath);

	Next();
}

template<typename TChar>
bool GenericFileFinder<TChar>::Next()
{
	struct dirent* entry;
	do
	{
		entry = readdir(m_dir);
		if (entry)
		{
			GenericFileFinderBase<TChar>::SetCurrentFileName(entry->d_name);
		}
		else
		{
			GenericFileFinderBase<TChar>::SetCurrentFileName((char*)NULL);
		}
	} while (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0);

	return !GenericFileFinderBase<TChar>::GetCurrent().IsEmpty();
}



#endif

// テンプレートのインスタンス化
template class GenericFileFinder<char>;
template class GenericFileFinder<wchar_t>;

LN_NAMESPACE_END
