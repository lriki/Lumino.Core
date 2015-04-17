
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
		SetReferenceBuffer(data, size);
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
	// �I�[ NULL �̓R�s�[���Ȃ� (QByteArray �Ɠ�������)
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

	// �K�v�ł���� 0 �N���A
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
	// �����蓖�ĂȂ� new ���邾���B
	if (m_buffer == NULL)
	{
		Alloc(size, zeroClear);
	}
	else
	{
		// �T�C�Y�� capacity ���傫���Ȃ�悤�ł���΍Ċm��
		byte_t* newBuf = m_buffer;
		size_t newSize = size;
		size_t newCapacity = m_capacity;
		if (size > m_capacity) {
			newBuf = LN_NEW byte_t[size];
			newCapacity = size;
			// �K�v�ł���� 0 �N���A
			if (zeroClear) {
				memset(newBuf, 0, newCapacity);
			}
		}

		// ���̃o�b�t�@������΃R�s�[�B���̌㌳�̃o�b�t�@��j��
		if (newBuf != m_buffer)
		{
			memcpy_s(newBuf, size, m_buffer, std::min(size, m_size));
			Dispose();
		}

		// �V�����o�b�t�@�ɍ����ւ�
		m_buffer = newBuf;
		m_capacity = newCapacity;
		m_size = newSize;
		m_refMode = false;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ByteBuffer::SetReferenceBuffer(void* buffer, size_t size)
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
//	LN_THROW(size <= m_size, ArgumentException);	// �k�������݂̂Ƃ肠��������
//	m_size = size;
//}
//
////-----------------------------------------------------------------------------
////
////-----------------------------------------------------------------------------
//void Reserve(size_t size, bool zeroClear)
//{
//	// �ŏ��Ƀ������m�ہB��O�������͉��������ɔ�������
//	byte_t* newBuf = LN_NEW byte_t[size];
//
//	// ���̃o�b�t�@������΃R�s�[�B���̌㌳�̃o�b�t�@��j��
//	if (m_buffer != NULL) {
//		memcpy_s(newBuf, size, m_buffer, std::min(size, m_size));
//	}
//	Dispose();
//
//	// �V�����o�b�t�@�ɍ����ւ�
//	m_buffer = newBuf;
//	m_size = size;
//	m_refMode = false;
//
//	// �K�v�ł���� 0 �N���A
//	if (zeroClear) {
//		memset(m_buffer, 0, m_size);
//	}
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void Reserve(const byte_t* data, size_t size)
//{
//	// ���̃o�b�t�@�͊��S�ɔj������̂Ő�ɏ����Ă��܂��ėǂ�
//	Dispose();
//	m_buffer = LN_NEW byte_t[size];
//	m_size = size;
//	mAutoDelete = true;
//	memcpy(m_buffer, data, m_size);
//}
//

} // namespace Lumino
