
#include "../Internal.h"
#include "../../include/Lumino/IO/MemoryStream.h"

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
void MemoryStream::Write(const void* data, size_t byteCount)
{
	size_t newPos = m_seekPos + byteCount;

	// �Œ蒷�̃o�b�t�@���g�p���Ă���ꍇ
	if (m_fixedBuffer != NULL)
	{
		if (newPos > m_fixedBufferSize) {
			LN_THROW(0, ArgumentException);
		}
		memcpy_s(m_fixedBuffer, m_fixedBufferSize, data, byteCount);
	}
	// �ϒ��̃o�b�t�@���g�p���Ă���ꍇ
	else
	{
		// �K�v������Ίg��
		if (newPos > m_buffer.size()) {
			m_buffer.resize(newPos);
		}
		void* p = &(m_buffer[m_seekPos]);
		memcpy_s(p, m_buffer.size(), data, byteCount);
	}
	m_seekPos = newPos;
}

} // namespace Lumino
