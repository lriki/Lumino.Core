
#include <sys/types.h>
#include <sys/stat.h>
#include "../Internal.h"
#include "../../include/Lumino/Base/RefBuffer.h"
#include "../../include/Lumino/Text/Encoding.h"
#include "../../include/Lumino/IO/FileStream.h"
#include "../../include/Lumino/IO/FileUtils.h"

namespace Lumino
{

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
#if 1	// fopen によるチェックはNG。ファイルが排他ロックで開かれていた時に失敗する。

bool FileUtils::Exists(const char* filePath)
{
#ifdef _WIN32
	DWORD attr = ::GetFileAttributesA(filePath);
	return ((attr != -1) &&
			(attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
	// http://www.ie.u-ryukyu.ac.jp/~kono/lecture/1999/os/info1/file-2.html
	struct stat st;
	int ret = ::stat(file, &st);
	st.st_mode
#endif
}
bool FileUtils::Exists(const wchar_t* filePath)
{
#ifdef _WIN32
	DWORD attr = ::GetFileAttributesW(filePath);
	return ((attr != -1) &&
			(attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
	// http://www.ie.u-ryukyu.ac.jp/~kono/lecture/1999/os/info1/file-2.html
	struct stat st;
	int ret = ::stat(file, &st);
	st.st_mode
#endif
}

#else
template<typename TChar>
bool FileUtils::Exists(const TChar* filePath)
{
	if (!filePath) return false; 
	FILE* fp = FileUtils::fopen( filePath, LN_T(TChar, "rb") );
	if (!fp) return false; 
	fclose(fp);
	return true;
}
template bool FileUtils::Exists<char>(const char* filePath);
template bool FileUtils::Exists<wchar_t>(const wchar_t* filePath);
#endif

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint32_t FileUtils::GetAttribute(const char* filePath)
{
#ifdef _WIN32
	DWORD attr = ::GetFileAttributesA(filePath);
	LN_THROW(attr != -1, Win32Exception, ::GetLastError());

	uint32_t flags = FileAttribute_Normal;
	if (attr & FILE_ATTRIBUTE_DIRECTORY) flags |= FileAttribute_Directory;
	if (attr & FILE_ATTRIBUTE_READONLY)  flags |= FileAttribute_ReadOnly;
	if (attr & FILE_ATTRIBUTE_HIDDEN)    flags |= FileAttribute_Hidden;
	return flags;
#else
#error
	// Unix 系の場合、ファイルの先頭が . であれば隠しファイルである。
	// mono-master/mono/io-layer/io.c の、_wapi_stat_to_file_attributes が参考になる。
#endif
}

uint32_t FileUtils::GetAttribute(const wchar_t* filePath)
{
#ifdef _WIN32
	DWORD attr = ::GetFileAttributesW(filePath);
	LN_THROW(attr != -1, Win32Exception, ::GetLastError());

	uint32_t flags = FileAttribute_Normal;
	if (attr & FILE_ATTRIBUTE_DIRECTORY) flags |= FileAttribute_Directory;
	if (attr & FILE_ATTRIBUTE_READONLY)  flags |= FileAttribute_ReadOnly;
	if (attr & FILE_ATTRIBUTE_HIDDEN)    flags |= FileAttribute_Hidden;
	return flags;
#else
#error
	// Unix 系の場合、ファイルの先頭が . であれば隠しファイルである。
	// mono-master/mono/io-layer/io.c の、_wapi_stat_to_file_attributes が参考になる。
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileUtils::Copy(const char* sourceFileName, const char* destFileName, bool overwrite)
{
#ifdef _WIN32
	BOOL bRes = ::CopyFileA(sourceFileName, destFileName, (overwrite) ? FALSE : TRUE);
	LN_THROW(bRes, Win32Exception, ::GetLastError());
#else
	// http://ppp-lab.sakura.ne.jp/ProgrammingPlacePlus/c/044.html
#error
#endif
}

void FileUtils::Copy(const wchar_t* sourceFileName, const wchar_t* destFileName, bool overwrite)
{
#ifdef _WIN32
	BOOL bRes = ::CopyFileW(sourceFileName, destFileName, (overwrite) ? FALSE : TRUE);
	LN_THROW(bRes, Win32Exception, ::GetLastError());
#else
	// http://ppp-lab.sakura.ne.jp/ProgrammingPlacePlus/c/044.html
#error
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileUtils::Delete(const char* filePath)
{
#ifdef _WIN32
	BOOL r = ::DeleteFileA(filePath);
	LN_THROW(r != FALSE, Win32Exception, ::GetLastError());
#else
#error
#endif
}
void FileUtils::Delete(const wchar_t* filePath)
{
#ifdef _WIN32
	BOOL r = ::DeleteFileW(filePath);
	LN_THROW(r != FALSE, Win32Exception, ::GetLastError());
#else
#error
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
size_t FileUtils::GetFileSize(const TCHAR* filePath)
{
	LN_THROW( filePath != NULL, ArgumentException );

	FILE* fp;
	errno_t r = _tfopen_s( &fp, filePath, _T("r") );
	LN_THROW( r == 0, FileNotFoundException );

	size_t size = 0;
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
size_t FileUtils::GetFileSize( FILE* stream )
{
#if defined(LN_WIN32)
	struct _stat stbuf;
	int handle = _fileno( stream );
	if ( handle == 0 ) {
		return 0;
	}
	if ( _fstat( handle, &stbuf ) == -1 ) {
		return 0;
	}
	return stbuf.st_size;
#else
	struct stat stbuf;
	int handle = fileno( stream );
	if ( handle == 0 ) {
		return 0;
	}
	if (fstat(handle, &stbuf) == -1) {
		return 0;
	}
	return stbuf.st_size;
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
RefBuffer* FileUtils::ReadAllBytes(const char* filePath)
{
	FILE* fp;
	errno_t err = fopen_s(&fp, filePath, "rb");
	LN_THROW(err == 0, FileNotFoundException);
	size_t size = GetFileSize(fp);

	
	RefPtr<RefBuffer> buffer(LN_NEW RefBuffer());
	buffer->Reserve( size );
	fread(buffer->GetPointer(), 1, size, fp);

	buffer.SafeAddRef();
	return buffer;
}
RefBuffer* FileUtils::ReadAllBytes(const wchar_t* filePath)
{
	FILE* fp;
	errno_t err = _wfopen_s(&fp, filePath, L"rb");
	LN_THROW(err == 0, FileNotFoundException);

	size_t size = GetFileSize(fp);

	RefPtr<RefBuffer> buffer(LN_NEW RefBuffer());
	buffer->Reserve(size);
	fread(buffer->GetPointer(), 1, size, fp);

	buffer.SafeAddRef();
	return buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
String FileUtils::ReadAllText(const TCHAR* filePath, const Encoding* encoding)
{
	// TODO: BOM
	RefPtr<RefBuffer> buffer(FileUtils::ReadAllBytes(filePath));
	String str;
	if (encoding) {
		str.ConvertFrom(buffer->GetPointer(), buffer->GetSize(), encoding);
	}
	else {
		str.ConvertFrom(buffer->GetPointer(), buffer->GetSize(), Encoding::GetTCharEncoding());
	}
	return str;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileUtils::WriteAllBytes(const TCHAR* filePath, const void* buffer, size_t size)
{
	FileStream stream;
	stream.Open(filePath, FileMode_Create, FileAccess_Write);
	stream.Write(buffer, size);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileUtils::WriteAllText(const TCHAR* filePath, const String& str, const Encoding* encoding)
{
	RefPtr<RefBuffer> buffer(str.ConvertTo(encoding));
	WriteAllBytes(filePath, buffer->GetPointer(), buffer->GetSize());
}

} // namespace Lumino
