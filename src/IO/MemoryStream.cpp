
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
	, m_constfixedBuffer(NULL)
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
void MemoryStream::Create(const void* buffer, size_t size)
{
	m_constfixedBuffer = buffer;
	m_fixedBufferSize = size;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void* MemoryStream::GetBuffer(size_t index)
{
	byte_t* buf = NULL;
	size_t size = 0;
	if (m_fixedBuffer != NULL) {
		buf = (byte_t*)m_fixedBuffer;
		size = m_fixedBufferSize;
	}
	else {
		buf = (byte_t*)&(m_buffer[0]);
		size = m_buffer.size();
	}

	LN_THROW(index < size, ArgumentException);
	return buf + index;
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
	return (m_constfixedBuffer == NULL);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int64_t MemoryStream::GetLength() const
{
	if (m_fixedBuffer != NULL || m_constfixedBuffer != NULL){
		return m_fixedBufferSize;
	}
	else {
		return m_buffer.size();
	}
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
size_t MemoryStream::Read(void* buffer, size_t byteCount)
{
	// const 固定長のバッファを使用している場合
	if (m_constfixedBuffer != NULL)
	{
		size_t readSize = std::min(byteCount, m_fixedBufferSize);
		memcpy_s(buffer, byteCount, &(((byte_t*)m_constfixedBuffer)[m_seekPos]), readSize);
		m_seekPos += readSize;
		return readSize;
	}
	// 固定長のバッファを使用している場合
	else  if (m_fixedBuffer != NULL)
	{
		LN_THROW(0, NotImplementedException);
		return 0;
	}
	// 可変長のバッファを使用している場合
	else
	{
		size_t readSize = std::min(byteCount, m_buffer.size());
		void* p = &(m_buffer[m_seekPos]);
		memcpy_s(buffer, byteCount, p, readSize);
		m_seekPos += readSize;
		return readSize;
	}
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
