
#include "../Internal.h"
#include "../../include/Lumino/IO/MemoryStream.h"
#include "../../include/Lumino/IO/FileUtils.h"

namespace Lumino
{

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
MemoryStream::MemoryStream()
	: m_seekPos(0)
	, m_fixedBuffer(NULL)
	, m_fixedBufferSize(0)
{
	// 要素が1つ以上無いと [0] にもアクセスできない (assert が発生する)
	m_buffer.resize(1);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
MemoryStream::~MemoryStream()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MemoryStream::Create(size_t uSize)
{
	m_buffer.resize(uSize);
	m_seekPos = 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MemoryStream::Create(void* buffer, size_t size)
{
	m_fixedBuffer = buffer;
	m_fixedBufferSize = size;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void* MemoryStream::GetBuffer()
{
	if (m_fixedBuffer != NULL) {
		return m_fixedBuffer;
	}
	else {
		return &(m_buffer[0]);
	}
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool MemoryStream::CanRead() const
{
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool MemoryStream::CanWrite() const
{
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int64_t MemoryStream::GetLength() const
{
	return m_buffer.size();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int64_t MemoryStream::GetPosition() const
{
	return m_seekPos;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MemoryStream::Write(const void* data, size_t byteCount)
{
	size_t newPos = m_seekPos + byteCount;

	// 固定長のバッファを使用している場合
	if (m_fixedBuffer != NULL)
	{
		if (newPos > m_fixedBufferSize) {
			LN_THROW(0, ArgumentException);
		}
		memcpy_s(m_fixedBuffer, m_fixedBufferSize, data, byteCount);
	}
	// 可変長のバッファを使用している場合
	else
	{
		// 必要があれば拡張
		if (newPos >= m_buffer.size()) {
			m_buffer.resize(newPos + 1);
		}
		void* p = &(m_buffer[m_seekPos]);
		memcpy_s(p, m_buffer.size(), data, byteCount);
	}
	m_seekPos = newPos;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MemoryStream::Seek(int64_t offset, SeekOrigin origin)
{
	m_seekPos = (size_t)FileUtils::CalcSeekPoint(m_seekPos, m_buffer.size(), offset, origin);
}

} // namespace Lumino
