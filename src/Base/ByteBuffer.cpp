
#include "../Internal.h"
#include <Lumino/Base/ByteBuffer.h>

namespace Lumino
{

//=============================================================================
// ByteBuffer
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ByteBuffer::ByteBuffer()
	: m_buffer(NULL)
	, m_capacity(0)
	, m_size(0)
	, m_refMode(false)
{}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ByteBuffer::ByteBuffer(size_t size, bool zeroClear)
	: m_buffer(NULL)
	, m_capacity(0)
	, m_size(0)
	, m_refMode(false)
{
	Resize(size, zeroClear);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ByteBuffer::ByteBuffer(const void* data, size_t size)
	: m_buffer(NULL)
	, m_capacity(0)
	, m_size(0)
	, m_refMode(false)
{
	Alloc(data, size);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ByteBuffer::ByteBuffer(void* data, size_t size, bool refMode)
	: m_buffer(NULL)
	, m_capacity(0)
	, m_size(0)
	, m_refMode(false)
{
	if (refMode) {
		Attach(data, size);
	}
	else {
		Alloc(data, size);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ByteBuffer::ByteBuffer(const char* str)
	: m_buffer(NULL)
	, m_capacity(0)
	, m_size(0)
	, m_refMode(false)
{
	// 終端 NULL はコピーしない (QByteArray と同じ動作)
	Alloc(str, sizeof(char) * strlen(str));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ByteBuffer::ByteBuffer(const wchar_t* str)
	: m_buffer(NULL)
	, m_capacity(0)
	, m_size(0)
	, m_refMode(false)
{
	Alloc(str, sizeof(wchar_t) * wcslen(str));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ByteBuffer::~ByteBuffer()
{
	Dispose();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ByteBuffer::Alloc(size_t size, bool zeroClear)
{
	Dispose();
	m_buffer = LN_NEW byte_t[size];
	m_capacity = size;
	m_size = size;
	m_refMode = false;

	// 必要であれば 0 クリア
	if (zeroClear) {
		memset(m_buffer, 0, m_size);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ByteBuffer::Alloc(const void* data, size_t size)
{
	Alloc(size, false);
	memcpy_s(m_buffer, size, data, size);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ByteBuffer::Resize(size_t size, bool zeroClear)
{
	// 未割り当てなら new するだけ。
	if (m_buffer == NULL)
	{
		Alloc(size, zeroClear);
	}
	else
	{
		// サイズが capacity より大きくなるようであれば再確保
		byte_t* newBuf = m_buffer;
		size_t newSize = size;
		size_t newCapacity = m_capacity;
		if (size > m_capacity) {
			newBuf = LN_NEW byte_t[size];
			newCapacity = size;
			// 必要であれば 0 クリア
			if (zeroClear) {
				memset(newBuf, 0, newCapacity);
			}
		}

		// 元のバッファがあればコピー。その後元のバッファを破棄
		if (newBuf != m_buffer)
		{
			memcpy_s(newBuf, size, m_buffer, std::min(size, m_size));
			Dispose();
		}

		// 新しいバッファに差し替え
		m_buffer = newBuf;
		m_capacity = newCapacity;
		m_size = newSize;
		m_refMode = false;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ByteBuffer::Attach(void* buffer, size_t size)
{
	Dispose();
	m_buffer = (byte_t*)buffer;
	m_size = size;
	m_refMode = true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ByteBuffer::Copy(const void* data, size_t size)
{
	LN_THROW(m_buffer != NULL, InvalidOperationException);
	LN_THROW(size <= m_size, ArgumentException);
	memcpy_s(m_buffer, m_size, data, size);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ByteBuffer::Copy(size_t offset, const void* data, size_t size)
{
	LN_THROW(m_buffer != NULL, InvalidOperationException);
	LN_THROW(offset + size <= m_size, ArgumentException);
	memcpy_s(m_buffer + offset, m_size - offset, data, size);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ByteBuffer::Clear()
{
	if (m_buffer) {
		memset(m_buffer, 0, m_size);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
byte_t& ByteBuffer::operator[] (size_t index)
{
	LN_THROW(index < m_size, ArgumentException);
	return m_buffer[index];
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ByteBuffer::Dispose()
{
	if (!m_refMode) {
		LN_SAFE_DELETE(m_buffer);
		m_capacity = 0;
		m_size = 0;
		m_refMode = false;
	}
}

//-----------------------------------------------------------------------------
//
////-----------------------------------------------------------------------------
//void ByteBuffer::SetInternalSize(size_t size)
//{
//	LN_THROW(size <= m_size, ArgumentException);	// 縮小方向のみとりあえず許可
//	m_size = size;
//}
//
////-----------------------------------------------------------------------------
////
////-----------------------------------------------------------------------------
//void Reserve(size_t size, bool zeroClear)
//{
//	// 最初にメモリ確保。例外した時は何もせずに抜けられる
//	byte_t* newBuf = LN_NEW byte_t[size];
//
//	// 元のバッファがあればコピー。その後元のバッファを破棄
//	if (m_buffer != NULL) {
//		memcpy_s(newBuf, size, m_buffer, std::min(size, m_size));
//	}
//	Dispose();
//
//	// 新しいバッファに差し替え
//	m_buffer = newBuf;
//	m_size = size;
//	m_refMode = false;
//
//	// 必要であれば 0 クリア
//	if (zeroClear) {
//		memset(m_buffer, 0, m_size);
//	}
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void Reserve(const byte_t* data, size_t size)
//{
//	// 元のバッファは完全に破棄するので先に消してしまって良い
//	Dispose();
//	m_buffer = LN_NEW byte_t[size];
//	m_size = size;
//	mAutoDelete = true;
//	memcpy(m_buffer, data, m_size);
//}
//

} // namespace Lumino
