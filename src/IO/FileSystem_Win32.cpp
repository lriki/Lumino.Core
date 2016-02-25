
#include <sys/types.h>
#include <sys/stat.h>
#include "../Internal.h"
#include "../../include/Lumino/Text/Encoding.h"
#include "../../include/Lumino/IO/FileStream.h"
#include "../../include/Lumino/IO/FileSystem.h"
#include "../../include/Lumino/IO/PathTraits.h"

LN_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
static void Win32IOErrorToExceptionThrow(DWORD errorCode, const TChar* message)
{
	switch (errorCode) {
	case ERROR_FILE_NOT_FOUND:
		LN_THROW(0, FileNotFoundException, message);

	case ERROR_PATH_NOT_FOUND:
		LN_THROW(0, DirectoryNotFoundException, message);

	case ERROR_ACCESS_DENIED:
		LN_THROW(0, IOException, LN_T(TChar, "Unauthorized access : %s"), message);

	case ERROR_ALREADY_EXISTS:
		LN_THROW(0, IOException, LN_T(TChar, "Already exists : %s"), message);

	case ERROR_FILENAME_EXCED_RANGE:
		LN_THROW(0, IOException, LN_T(TChar, "Path too long : %s"), message);

	case ERROR_INVALID_DRIVE:
		LN_THROW(0, IOException, LN_T(TChar, "Drive not found : %s"), message);

	case ERROR_FILE_EXISTS:
		LN_THROW(0, IOException, LN_T(TChar, "File exists : %s"), message);

	default:
		LN_THROW(0, Win32Exception, errorCode);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool FileSystem::Exists(const char* filePath)
{
	// ※fopen によるチェックはNG。ファイルが排他ロックで開かれていた時に失敗する。
	DWORD attr = ::GetFileAttributesA(filePath);
	// 他ユーザーフォルダ内のファイルにアクセスしようとすると attr = -1 になる。
	// このとき GetLastError() は ERROR_ACCESS_DENIED である。
	// .NET の仕様にあわせ、エラーは一律 false で返している。
	return ((attr != -1) &&
			(attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool FileSystem::Exists(const wchar_t* filePath)
{
	DWORD attr = ::GetFileAttributesW(filePath);
	return ((attr != -1) &&
			(attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileSystem::SetAttribute(const char* filePath, uint32_t attr)
{
	DWORD dwAttr = 0;
	if (attr & FileAttribute::Directory) dwAttr |= FILE_ATTRIBUTE_DIRECTORY;
	if (attr & FileAttribute::ReadOnly)  dwAttr |= FILE_ATTRIBUTE_READONLY;
	if (attr & FileAttribute::Hidden)    dwAttr |= FILE_ATTRIBUTE_HIDDEN;
	BOOL r = ::SetFileAttributesA(filePath, dwAttr);
	if (r == FALSE) { Win32IOErrorToExceptionThrow(::GetLastError(), filePath); }
}

void FileSystem::SetAttribute(const wchar_t* filePath, uint32_t attr)
{
	DWORD dwAttr = 0;
	if (attr & FileAttribute::Directory) dwAttr |= FILE_ATTRIBUTE_DIRECTORY;
	if (attr & FileAttribute::ReadOnly)  dwAttr |= FILE_ATTRIBUTE_READONLY;
	if (attr & FileAttribute::Hidden)    dwAttr |= FILE_ATTRIBUTE_HIDDEN;
	BOOL r = ::SetFileAttributesW(filePath, dwAttr);
	if (r == FALSE) { Win32IOErrorToExceptionThrow(::GetLastError(), filePath); }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileSystem::Copy(const char* sourceFileName, const char* destFileName, bool overwrite)
{
	BOOL bRes = ::CopyFileA(sourceFileName, destFileName, (overwrite) ? FALSE : TRUE);
	if (bRes == FALSE) {
		// TODO 引数もう一つ増やさないと、どちらが原因かわかりにくい
		Win32IOErrorToExceptionThrow(::GetLastError(), sourceFileName);
	}
}

void FileSystem::Copy(const wchar_t* sourceFileName, const wchar_t* destFileName, bool overwrite)
{
	BOOL bRes = ::CopyFileW(sourceFileName, destFileName, (overwrite) ? FALSE : TRUE);
	if (bRes == FALSE) {
		// TODO 引数もう一つ増やさないと、どちらが原因かわかりにくい
		Win32IOErrorToExceptionThrow(::GetLastError(), sourceFileName);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileSystem::Delete(const char* filePath)
{
	BOOL r = ::DeleteFileA(filePath);
	if (r == FALSE) {
		Win32IOErrorToExceptionThrow(::GetLastError(), filePath);
	}
}
void FileSystem::Delete(const wchar_t* filePath)
{
	BOOL r = ::DeleteFileW(filePath);
	if (r == FALSE) {
		Win32IOErrorToExceptionThrow(::GetLastError(), filePath);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static BOOL RemoveDirectoryT(LPCSTR lpPathName) { return ::RemoveDirectoryA(lpPathName); }
static BOOL RemoveDirectoryT(LPCWSTR lpPathName) { return ::RemoveDirectoryW(lpPathName); }

template<typename TChar>
void FileSystem::DeleteDirectory(const TChar* path, bool recursive)
{
	if (recursive)
	{
		auto list = GetFileSystemEntries(path, nullptr);
		for (auto path : list)
		{
			if (GetAttribute(path.c_str()) == FileAttribute::Directory) {
				DeleteDirectory(path.c_str(), recursive);
			}
			else {
				Delete(path.c_str());
			}
		}
	}
	BOOL r = RemoveDirectoryT(path);
	if (r == FALSE) {
		Win32IOErrorToExceptionThrow(::GetLastError(), path);
	}
}
template void FileSystem::DeleteDirectory<char>(const char* path, bool recursive);
template void FileSystem::DeleteDirectory<wchar_t>(const wchar_t* path, bool recursive);

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
StringArrayA FileSystem::GetFileSystemEntries(const char* directoryPath, const char* pattern)
{
	LN_THROW(0, NotImplementedException);
	return StringArrayA();
}
StringArrayW FileSystem::GetFileSystemEntries(const wchar_t* directoryPath, const wchar_t* pattern)
{
	StringArrayW fileList;
	PathNameW dirPathKey(directoryPath);
	StringW dirPath(dirPathKey.GetStrEndSeparator());

	// 
	if (pattern) {
		dirPathKey.Append(pattern);
	}
	else {
		dirPathKey.Append(_T("*"));
	}

	// 検索開始
	WIN32_FIND_DATAW fd;
	HANDLE h = ::FindFirstFileW(dirPathKey.c_str(), &fd);
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
		if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) {
			// これらはいらない
		}
		else {
			fileList.Add(dirPath + fd.cFileName);
		}

	} while (::FindNextFileW(h, &fd));

	// 終了
	::FindClose(h);
	return fileList;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t FileSystem::GetFileSize(const TCHAR* filePath)
{
	LN_THROW( filePath != NULL, ArgumentException );

	FILE* fp;
	errno_t r = _tfopen_s( &fp, filePath, _T("r") );
	LN_THROW( r == 0, FileNotFoundException );

	uint64_t size = 0;
	try
	{
		size = GetFileSize(fp);
	}
	catch (...)
	{
		fclose( fp );
		throw;
	}
	fclose( fp );
	return size;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t FileSystem::GetFileSize(FILE* stream)
{
	struct _stat stbuf;
	int handle = _fileno( stream );
	if ( handle == 0 ) {
		return 0;
	}
	if ( _fstat( handle, &stbuf ) == -1 ) {
		return 0;
	}
	return stbuf.st_size;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//bool FileSystem::ExistsDirectory(const char* path)
//{
//	DWORD attr = ::GetFileAttributesA(path);
//	return (attr != -1 && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
//}
//bool FileSystem::ExistsDirectory(const wchar_t* path)
//{
//	DWORD attr = ::GetFileAttributesW(path);
//	return (attr != -1 && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool FileSystem::mkdir(const char* path)
{
	return ::CreateDirectoryA(path, NULL) != FALSE;
}
bool FileSystem::mkdir(const wchar_t* path)
{
	return ::CreateDirectoryW(path, NULL) != FALSE;
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool FileSystem::GetAttributeInternal(const char* path, FileAttribute* outAttr)
{
	DWORD attr = ::GetFileAttributesA(path);
	if (attr == -1) { return false; }

	FileAttribute flags = FileAttribute::Normal;
	if (attr & FILE_ATTRIBUTE_DIRECTORY) flags |= FileAttribute::Directory;
	if (attr & FILE_ATTRIBUTE_READONLY)  flags |= FileAttribute::ReadOnly;
	if (attr & FILE_ATTRIBUTE_HIDDEN)    flags |= FileAttribute::Hidden;
	*outAttr = flags;
	return true;
}
bool FileSystem::GetAttributeInternal(const wchar_t* path, FileAttribute* outAttr)
{
	DWORD attr = ::GetFileAttributesW(path);
	if (attr == -1) { return false; }

	FileAttribute flags = FileAttribute::Normal;
	if (attr & FILE_ATTRIBUTE_DIRECTORY) flags |= FileAttribute::Directory;
	if (attr & FILE_ATTRIBUTE_READONLY)  flags |= FileAttribute::ReadOnly;
	if (attr & FILE_ATTRIBUTE_HIDDEN)    flags |= FileAttribute::Hidden;
	*outAttr = flags;
	return true;
}

LN_NAMESPACE_END
