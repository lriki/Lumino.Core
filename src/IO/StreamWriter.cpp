
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
	// モード選択
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
	// encoding 未指定であれば UTF8 とする
	if (encoding == NULL) {
		encoding = Text::Encoding::GetUTF8Encoding();
	}

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
