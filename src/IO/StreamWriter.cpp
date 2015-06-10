
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
	uint32_t openMode;
	if (mode == FileWriteMode_Truncate) {
		openMode = FileOpenMode_Write | FileOpenMode_Truncate;
	}
	else {
		openMode = FileOpenMode_Write | FileOpenMode_Append;
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
