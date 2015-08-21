
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
StreamWriter::StreamWriter(Stream* stream, Text::Encoding* encoding)
{
	Init(stream, encoding);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
StreamWriter::StreamWriter(const PathName& filePath, Text::Encoding* encoding, FileWriteMode mode)
{
	// ÉÇÅ[ÉhëIë
	FileOpenMode openMode;
	if (mode == FileWriteMode_Truncate) {
		openMode = FileOpenMode::Write | FileOpenMode::Truncate;
	}
	else {
		openMode = FileOpenMode::Write | FileOpenMode::Append;
	}

	RefPtr<FileStream> stream(LN_NEW FileStream(filePath, openMode));
	Init(stream, encoding);
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
void StreamWriter::Init(Stream* stream, Text::Encoding* encoding)
{
	m_stream = stream;
	SetEncoding(encoding);
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
