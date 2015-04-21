
#include "../Internal.h"
#include "../../include/Lumino/IO/FileUtils.h"
#include "../../include/Lumino/IO/FileStream.h"

namespace Lumino
{

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FileStream::FileStream()
	: m_stream(NULL)
	, m_openModeFlags(0)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FileStream::FileStream(const TCHAR* filePath, uint32_t openMode)
	: m_stream(NULL)
	, m_openModeFlags(0)
{
	Open(filePath, openMode);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FileStream::~FileStream()
{
	Close();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileStream::Open(const TCHAR* filePath, uint32_t openMode)
{
	LN_ASSERT( filePath );
	Close();

	const TCHAR* mode = NULL;
	if ((openMode & FileOpenMode_ReadWrite) == FileOpenMode_ReadWrite)
	{
		if (openMode & FileOpenMode_Append) {
			mode = _T("a+");		// 読み取りと書き込み (末尾に追加する)
		}
		else if (openMode & FileOpenMode_Truncate) {
			mode = _T("w+");		// 読み取りと書き込み (ファイルを空にする)
		}
		else {
			mode = _T("r+");		// 読み取りと書き込み (ファイルが存在しない場合はエラー)
		}
	}
	else if (openMode & FileOpenMode_Write)
	{
		if (openMode & FileOpenMode_Append) {
			mode = _T("a");			// 書き込み (末尾に追加する。ファイルが無ければ新規作成)
		}
		else if (openMode & FileOpenMode_Truncate) {
			mode = _T("w");			// 書き込み (ファイルを空にする)
		}
		else {
			mode = _T("w");			// 書き込み (モード省略。Truncate)
		}
	}
	else if (openMode & FileOpenMode_Read)
	{
		if (openMode & FileOpenMode_Append) {
			mode = NULL;			// 読み込みなのに末尾追加はできない
		}
		else if (openMode & FileOpenMode_Truncate) {
			mode = NULL;			// 読み込みなのにファイルを空にはできない
		}
		else {
			mode = _T("r");			// 書き込み (モード省略。Truncate)
		}
	}
	LN_THROW(mode, ArgumentException);

	errno_t err = _tfopen_s(&m_stream, filePath, mode);
	LN_THROW(err == 0, FileNotFoundException);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileStream::Close()
{
	if (m_stream != NULL) {
		fclose(m_stream);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool FileStream::CanRead() const
{
	return ((m_openModeFlags & FileOpenMode_Read) != 0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool FileStream::CanWrite() const
{
	return ((m_openModeFlags & FileOpenMode_Write) != 0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int64_t FileStream::GetLength() const
{
	return (size_t)FileUtils::GetFileSize( m_stream );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int64_t FileStream::GetPosition() const
{
	// TODO: 64bit 確認 → ftello?
	return ftell(m_stream);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
size_t FileStream::Read(void* buffer, size_t readCount)
{
	LN_THROW(m_stream, InvalidOperationException);
	return fread(buffer, 1, readCount, m_stream);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileStream::Write( const void* data, size_t byteCount )
{
	LN_THROW(m_stream, InvalidOperationException);

	size_t nWriteSize = fwrite( data, 1, byteCount, m_stream );
	LN_THROW(nWriteSize == byteCount, NotSupportedException);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileStream::Seek(int64_t offset, SeekOrigin origin)
{
	LN_THROW(m_stream, InvalidOperationException);

#ifdef LN_WIN32
	_fseeki64(m_stream, offset, origin);
#else
	// TODO:
	// http://stackoverflow.com/questions/1035657/seeking-and-reading-large-files-in-a-linux-c-application
	fseek(m_stream, offset, origin);
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileStream::Flush()
{
	LN_THROW(m_stream, InvalidOperationException);
	fflush(m_stream);
}

} // namespace Lumino
