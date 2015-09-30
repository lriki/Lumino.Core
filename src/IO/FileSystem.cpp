
#include <sys/types.h>
#include <sys/stat.h>
#include "../Internal.h"
#include "../../include/Lumino/Base/StringTraits.h"
#include "../../include/Lumino/Base/ByteBuffer.h"
#include "../../include/Lumino/Text/Encoding.h"
#include "../../include/Lumino/IO/FileStream.h"
#include "../../include/Lumino/IO/FileSystem.h"

namespace Lumino
{
#if 0


#ifdef _WIN32
#else
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static bool is_stat_writable(struct stat *st, const char *path)
{
	// 制限なしに書き込み可であるか
	if (st->st_mode & S_IWOTH)
		return 1;
	// 現在のユーザーIDに許可されているか
	if ((st->st_uid == geteuid()) && (st->st_mode & S_IWUSR))
		return 1;
	// 現在のグループIDに許可されているか
	if ((st->st_gid == getegid()) && (st->st_mode & S_IWGRP))
		return 1;
	// もうここに来ることはほとんどないはずだが念のため
	return access(path, W_OK) == 0;
}
#endif

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool FileSystem::Exists(const char* filePath)
{
	// ※fopen によるチェックはNG。ファイルが排他ロックで開かれていた時に失敗する。
#ifdef _WIN32
	DWORD attr = ::GetFileAttributesA(filePath);
	return ((attr != -1) &&
			(attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
	// http://www.ie.u-ryukyu.ac.jp/~kono/lecture/1999/os/info1/file-2.html
	//struct stat st;
	//int ret = ::stat(file, &st);
	//st.st_mode
	int ret = access(path, F_OK);
	if (ret == -1) {
		if (errno == ENOENT) {
			return false;
		}
		else {
			// パスが長い、メモリが足りない等理由は様々。
			// http://linuxjm.sourceforge.jp/html/LDP_man-pages/man2/faccessat.2.html
			LN_THROW(0, IOException, strerror(errno));
		}
	}
	return true;
#endif
}
bool FileSystem::Exists(const wchar_t* filePath)
{
#ifdef _WIN32
	DWORD attr = ::GetFileAttributesW(filePath);
	return ((attr != -1) &&
			(attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
	LN_LOCAL_MBCS_FILEPATH(mbcsFilePath, filePath);
	return Exists(mbcsFilePath);
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint32_t FileSystem::GetAttribute(const char* filePath)
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
	// Unix 系の場合、ファイルの先頭が . であれば隠しファイルである。
	// mono-master/mono/io-layer/io.c の、_wapi_stat_to_file_attributes が参考になる。
	struct stat st;
	int ret = ::stat(file, &st);
	if (ret == -1) {
		LN_THROW(0, IOException);
	}

	uint32_t attrs = 0;
	if (S_ISDIR(buf->st_mode))
	{
		attrs |= FileAttribute_Directory;
		if (!is_stat_writable()) {
			attrs |= FileAttribute_ReadOnly;
		}
		if (filename[0] == '.') {
			attrs |= FileAttribute_Hidden;
		}
	}
	else
	{
		if (!is_stat_writable()) {
			attrs |= FileAttribute_ReadOnly;
		}
		if (filename[0] == '.') {
			attrs |= FileAttribute_Hidden;
		}
	}
	return attrs;
#endif
}

uint32_t FileSystem::GetAttribute(const wchar_t* filePath)
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
	LN_LOCAL_MBCS_FILEPATH(mbcsFilePath, filePath);
	return GetAttribute(mbcsFilePath);
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileSystem::Copy(const char* sourceFileName, const char* destFileName, bool overwrite)
{
#ifdef _WIN32
	BOOL bRes = ::CopyFileA(sourceFileName, destFileName, (overwrite) ? FALSE : TRUE);
	LN_THROW(bRes, Win32Exception, ::GetLastError());
#else
	// http://ppp-lab.sakura.ne.jp/ProgrammingPlacePlus/c/044.html
	FILE* fpSrc = fopen(sourceFileName, "rb");
	if (fpSrc == NULL){
		LN_THROW(0, IOException);
	}

	FILE* fpDest = fopen(destFileName, "wb");
	if (fpDest == NULL){
		LN_THROW(0, IOException);
	}

	// バイナリデータとして 1byte ずつコピー
	// (windows ではバッファリングが効くけど、それ以外はわからない。
	//  Linux とかで極端に遅くなるようならここでバッファリングも考える)
	while (1)
	{
		byte_t b;
		fread(&b, 1, 1, fpSrc);
		if (feof(fpSrc)){
			break;
		}
		if (ferror(fpSrc)){
			LN_THROW(0, IOException);
		}

		fwrite(&c, sizeof(c), 1, fpDest);
	}

	fclose(fpDest);
	fclose(fpSrc);
	return 0;
#endif
}

void FileSystem::Copy(const wchar_t* sourceFileName, const wchar_t* destFileName, bool overwrite)
{
#ifdef _WIN32
	BOOL bRes = ::CopyFileW(sourceFileName, destFileName, (overwrite) ? FALSE : TRUE);
	LN_THROW(bRes, Win32Exception, ::GetLastError());
#else
	LN_LOCAL_MBCS_FILEPATH(mbcsSrc, sourceFileName);
	LN_LOCAL_MBCS_FILEPATH(mbcsDest, destFileName);
	Copy(mbcsSrc, mbcsDest, overwrite);
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileSystem::Delete(const char* filePath)
{
#ifdef _WIN32
	BOOL r = ::DeleteFileA(filePath);
	LN_THROW(r != FALSE, Win32Exception, ::GetLastError());
#else
	int ret = remove(path);
	if (ret == -1) LN_THROW(IOException, strerror(errno));
#endif
}
void FileSystem::Delete(const wchar_t* filePath)
{
#ifdef _WIN32
	BOOL r = ::DeleteFileW(filePath);
	LN_THROW(r != FALSE, Win32Exception, ::GetLastError());
#else
	LN_LOCAL_MBCS_FILEPATH(mbcsFilePath, filePath);
	Delete(mbcsFilePath);
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
size_t FileSystem::GetFileSize(const TCHAR* filePath)
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
size_t FileSystem::GetFileSize( FILE* stream )
{
#if defined(LN_OS_WIN32)
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

#endif

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ByteBuffer FileSystem::ReadAllBytes(const char* filePath)
{
	FILE* fp;
	errno_t err = fopen_s(&fp, filePath, "rb");
	LN_THROW(err == 0, FileNotFoundException);
	size_t size = (size_t)GetFileSize(fp);

	ByteBuffer buffer(size);
	fread(buffer.GetData(), 1, size, fp);
	return buffer;
}
ByteBuffer FileSystem::ReadAllBytes(const wchar_t* filePath)
{
	FILE* fp;
	errno_t err = _wfopen_s(&fp, filePath, L"rb");
	LN_THROW(err == 0, FileNotFoundException);
	size_t size = (size_t)GetFileSize(fp);

	ByteBuffer buffer(size);
	fread(buffer.GetData(), 1, size, fp);
	return buffer;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
String FileSystem::ReadAllText(const TCHAR* filePath, const Encoding* encoding)
{
	// TODO: BOM
	const ByteBuffer buffer(FileSystem::ReadAllBytes(filePath));
	String str;
	if (encoding) {
		str.ConvertFrom(buffer.GetData(), buffer.GetSize(), encoding);
	}
	else {
		str.ConvertFrom(buffer.GetData(), buffer.GetSize(), Encoding::GetUTF8Encoding());
	}
	return str;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileSystem::WriteAllBytes(const TCHAR* filePath, const void* buffer, size_t size)
{
	FileStream stream;
	stream.Open(filePath, FileOpenMode::Write | FileOpenMode::Truncate);
	stream.Write(buffer, size);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileSystem::WriteAllText(const TCHAR* filePath, const String& str, const Encoding* encoding)
{
	encoding = (encoding == NULL) ? Encoding::GetUTF8Encoding() : encoding;
	const ByteBuffer buffer(str.ConvertTo(encoding));
	WriteAllBytes(filePath, buffer.GetData(), buffer.GetSize());
}

//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
int64_t FileSystem::CalcSeekPoint(int64_t curPoint, int64_t maxSize, int64_t offset, int origin)
{
	int64_t newPoint = curPoint;
	switch (origin)
	{
	case SEEK_CUR:
		newPoint += offset;
		break;

	case SEEK_END:
		newPoint = maxSize + offset;
		break;

	default:
		newPoint = offset;
		break;
	}

	if (newPoint < 0) {
		newPoint = 0;
	}
	if (newPoint > maxSize) {
		newPoint = maxSize;
	}
	return newPoint;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CaseSensitivity FileSystem::GetFileSystemCaseSensitivity()
{
#ifdef LN_OS_WIN32
	return CaseSensitivity_CaseInsensitive;
#else
	return CaseSensitivity_CaseSensitive;
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void FileSystem::CreateDirectoryInternal(const TChar* path)
{
	Array< GenericString<wchar_t> >	pathList;
	GenericString<wchar_t> dir;

	int i = StringTraits::StrLen(path) - 1;	// 一番後ろの文字の位置
	while (i >= 0)
	{
		dir.AssignCStr(path, 0, i + 1);
		if (FileSystem::ExistsDirectory(dir)) {
			break;
		}
		pathList.Add(dir);

		// セパレータが見つかるまで探す
		while (i > 0 && path[i] != PathTraits::DirectorySeparatorChar && path[i] != PathTraits::AltDirectorySeparatorChar) {
			--i;
		}
		--i;
	}

	if (pathList.IsEmpty()) { return; }	// path が存在している

	for (int i = pathList.GetCount() - 1; i >= 0; --i)
	{
		bool r = FileSystem::mkdir(pathList[i]);
		LN_THROW(r, IOException);
	}
}
void FileSystem::CreateDirectory(const char* path)
{
	CreateDirectoryInternal(path);
}
void FileSystem::CreateDirectory(const wchar_t* path)
{
	CreateDirectoryInternal(path);
}


} // namespace Lumino
