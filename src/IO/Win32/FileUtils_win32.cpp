
#include <sys/types.h>
#include <sys/stat.h>
#include "../../Internal.h"
#include "../../../include/Lumino/Text/Encoding.h"
#include "../../../include/Lumino/IO/FileStream.h"
#include "../../../include/Lumino/IO/FileUtils.h"

namespace Lumino
{

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
bool FileUtils::Exists(const char* filePath)
{
	// ※fopen によるチェックはNG。ファイルが排他ロックで開かれていた時に失敗する。
	DWORD attr = ::GetFileAttributesA(filePath);
	// 他ユーザーフォルダ内のファイルにアクセスしようとすると attr = -1 になる。
	// このとき GetLastError() は ERROR_ACCESS_DENIED である。
	// .NET の仕様にあわせ、エラーは一律 false で返している。
	return ((attr != -1) &&
			(attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool FileUtils::Exists(const wchar_t* filePath)
{
	DWORD attr = ::GetFileAttributesW(filePath);
	return ((attr != -1) &&
			(attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint32_t FileUtils::GetAttribute(const char* filePath)
{
	DWORD attr = ::GetFileAttributesA(filePath);
	if (attr == -1) {
		Win32IOErrorToExceptionThrow(::GetLastError(), filePath);
	}

	uint32_t flags = FileAttribute_Normal;
	if (attr & FILE_ATTRIBUTE_DIRECTORY) flags |= FileAttribute_Directory;
	if (attr & FILE_ATTRIBUTE_READONLY)  flags |= FileAttribute_ReadOnly;
	if (attr & FILE_ATTRIBUTE_HIDDEN)    flags |= FileAttribute_Hidden;
	return flags;
}

uint32_t FileUtils::GetAttribute(const wchar_t* filePath)
{
	DWORD attr = ::GetFileAttributesW(filePath);
	if (attr == -1) {
		Win32IOErrorToExceptionThrow(::GetLastError(), filePath);
	}

	uint32_t flags = FileAttribute_Normal;
	if (attr & FILE_ATTRIBUTE_DIRECTORY) flags |= FileAttribute_Directory;
	if (attr & FILE_ATTRIBUTE_READONLY)  flags |= FileAttribute_ReadOnly;
	if (attr & FILE_ATTRIBUTE_HIDDEN)    flags |= FileAttribute_Hidden;
	return flags;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileUtils::SetAttribute(const char* filePath, uint32_t attr)
{
	DWORD dwAttr = 0;
	if (attr & FileAttribute_Directory) dwAttr |= FILE_ATTRIBUTE_DIRECTORY;
	if (attr & FileAttribute_ReadOnly)  dwAttr |= FILE_ATTRIBUTE_READONLY;
	if (attr & FileAttribute_Hidden)    dwAttr |= FILE_ATTRIBUTE_HIDDEN;
	BOOL r = ::SetFileAttributesA(filePath, dwAttr);
	if (r == FALSE) { Win32IOErrorToExceptionThrow(::GetLastError(), filePath); }
}

void FileUtils::SetAttribute(const wchar_t* filePath, uint32_t attr)
{
	DWORD dwAttr = 0;
	if (attr & FileAttribute_Directory) dwAttr |= FILE_ATTRIBUTE_DIRECTORY;
	if (attr & FileAttribute_ReadOnly)  dwAttr |= FILE_ATTRIBUTE_READONLY;
	if (attr & FileAttribute_Hidden)    dwAttr |= FILE_ATTRIBUTE_HIDDEN;
	BOOL r = ::SetFileAttributesW(filePath, dwAttr);
	if (r == FALSE) { Win32IOErrorToExceptionThrow(::GetLastError(), filePath); }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileUtils::Copy(const char* sourceFileName, const char* destFileName, bool overwrite)
{
	BOOL bRes = ::CopyFileA(sourceFileName, destFileName, (overwrite) ? FALSE : TRUE);
	if (bRes == FALSE) {
		// TODO 引数もう一つ増やさないと、どちらが原因かわかりにくい
		Win32IOErrorToExceptionThrow(::GetLastError(), sourceFileName);
	}
}

void FileUtils::Copy(const wchar_t* sourceFileName, const wchar_t* destFileName, bool overwrite)
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
void FileUtils::Delete(const char* filePath)
{
	BOOL r = ::DeleteFileA(filePath);
	if (r == FALSE) {
		Win32IOErrorToExceptionThrow(::GetLastError(), filePath);
	}
}
void FileUtils::Delete(const wchar_t* filePath)
{
	BOOL r = ::DeleteFileW(filePath);
	if (r == FALSE) {
		Win32IOErrorToExceptionThrow(::GetLastError(), filePath);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t FileUtils::GetFileSize(const TCHAR* filePath)
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
uint64_t FileUtils::GetFileSize(FILE* stream)
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

} // namespace Lumino
