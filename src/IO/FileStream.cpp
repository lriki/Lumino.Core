
#include "../Internal.h"
#include "../../include/Lumino/IO/FileUtils.h"
#include "../../include/Lumino/IO/FileStream.h"

namespace Lumino
{

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FileStream::FileStream()
	: mStream(NULL)
	, mFileAccess(FileAccess_Max)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FileStream::FileStream(const TCHAR* filePath, FileMode fileMode, FileAccess fileAccess) 
	: mStream(NULL)
	, mFileAccess(FileAccess_Max)
{
	Open(filePath, fileMode, fileAccess);
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
void FileStream::Open( const TCHAR* filePath, FileMode fileMode, FileAccess fileAccess )
{
	LN_ASSERT( filePath );
	Close();

	mFileAccess = fileAccess;

	const TCHAR* modeTable[FileMode_Max][FileAccess_Max] = {
		// FileAccess_Read	FileAccess_ReadWrite	FileAccess_Write
		{  NULL,			_T("w+b"),				_T("wb") },		// FileMode_Create
		{  _T("rb"),		_T("w+b"),				_T("wb") },		// FileMode_Open
		{  _T("ab"),		_T("a+b"),				_T("a+b") },	// FileMode_Append
	};

	const TCHAR* mode = modeTable[fileMode][fileAccess];
	LN_THROW(mode, FileNotFoundException);

	errno_t err = _tfopen_s(&mStream, filePath, mode);
	LN_THROW( err == 0, FileNotFoundException );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileStream::Close()
{
	if (mStream != NULL) {
		fclose(mStream);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
size_t FileStream::GetSize()
{
	return FileUtils::GetFileSize( mStream );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
size_t FileStream::Read(void* buffer, size_t bufferSize, size_t readCount)
{
	LN_THROW(mStream, InvalidOperationException);
	LN_THROW(bufferSize >= readCount, ArgumentException);
	return fread(buffer, 1, readCount, mStream );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileStream::Write( const void* data, size_t byteCount )
{
	LN_THROW(mStream, InvalidOperationException);

	size_t nWriteSize = fwrite( data, 1, byteCount, mStream );
	LN_THROW(nWriteSize == byteCount, NotSupportedException);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileStream::Flush()
{
	LN_THROW(mStream, InvalidOperationException);
	fflush(mStream);
}

} // namespace Lumino
