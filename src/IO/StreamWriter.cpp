
#include "../Internal.h"
#include <Lumino/IO/FileStream.h>
#include <Lumino/IO/StreamWriter.h>

namespace Lumino
{

//=============================================================================
// StreamWriter
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
StreamWriter::StreamWriter(const PathName& filePath, Text::Encoding* encoding, FileWriteMode mode)
{
	// モード選択
	FileOpenMode openMode;
	if (mode == FileWriteMode_Truncate) {
		openMode = FileOpenMode::Write | FileOpenMode::Truncate;
	}
	else {
		openMode = FileOpenMode::Write | FileOpenMode::Append;
	}
	
	// ファイルを開く
	m_stream.Attach(LN_NEW FileStream(filePath, openMode));

	// エンコーディング指定
	SetEncoding(encoding);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
StreamWriter::~StreamWriter()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void StreamWriter::Flash()
{
	m_stream->Flush();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void StreamWriter::WriteOverride(const void* data, size_t byteCount)
{
	m_stream->Write(data, byteCount);
}

} // namespace Lumino
