
#pragma once

#include "../Internal.h"
#include "../../include/Lumino/IO/File.h"
#include "../../include/Lumino/IO/FileUtils.h"

namespace Lumino
{

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
File::File(const String& filePath)
	: m_filePath(filePath)
	, m_fileAccess(FileAccess_Read)
	, m_stream(NULL)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
File::File(const PathName& filePath)
	: m_filePath(filePath)
	, m_fileAccess(FileAccess_Read)
	, m_stream(NULL)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
File::~File()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void File::Open(FileMode mode, FileAccess access)
{
	LN_THROW(m_stream, InvalidOperationException);	// すでにファイルが開かれている

	m_fileAccess = access;

	const TCHAR* modeTable[FileMode_Max][FileAccess_Max] = {
		// FileAccess_Read	FileAccess_ReadWrite	FileAccess_Write
		{ NULL,				_T("w+b"),				_T("wb") },		// FileMode_Create
		{ _T("rb"),			_T("w+b"),				_T("wb") },		// FileMode_Open
		{ _T("ab"),			_T("a+b"),				_T("a+b") },	// FileMode_Append
	};

	const TCHAR* modeStr = modeTable[mode][access];
	LN_THROW(modeStr, FileNotFoundException);

	errno_t err = _tfopen_s(&m_stream, m_filePath.GetCStr(), modeStr);
	LN_THROW(err == 0, FileNotFoundException);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void File::Close()
{
	if (m_stream != NULL) {
		fclose(m_stream);
		m_stream = NULL;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PathName File::GetFilePath() const
{
	return m_filePath.CanonicalizePath();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
String File::GetFileName() const
{
	return m_filePath.GetFileName();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t File::GetLength() const
{
	return FileUtils::GetFileSize(m_filePath.GetCStr());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool File::CanRead()
{
	return (m_fileAccess == FileAccess_Read) || (m_fileAccess == FileAccess_ReadWrite);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool File::CanWrite()
{
	return (m_fileAccess == FileAccess_Write) || (m_fileAccess == FileAccess_ReadWrite);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
size_t File::GetSize()
{
	return (size_t)GetLength();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
size_t File::Read(void* buffer, size_t readCount)
{
	LN_THROW(m_stream, InvalidOperationException);
	return fread(buffer, 1, readCount, m_stream);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void File::Write(const void* data, size_t byteCount)
{
	LN_THROW(m_stream, InvalidOperationException);
	size_t writeSize = fwrite(data, 1, byteCount, m_stream);
	LN_THROW(writeSize == byteCount, NotSupportedException);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void File::Flush()
{
	LN_THROW(m_stream, InvalidOperationException);
	fflush(m_stream);
}

} // namespace Lumino
