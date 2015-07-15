
#include "../Internal.h"
#include <Lumino/Base/StringBuilder.h>

namespace Lumino
{

//=============================================================================
// StringBuilder
//=============================================================================

const int DefaultCapacity = 512;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
StringBuilder::StringBuilder()
	: m_buffer()
	, m_bufferUsed(0)
{
	m_buffer.Resize(DefaultCapacity, false);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
StringBuilder::~StringBuilder()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void StringBuilder::Append(const TCHAR* str, int length)
{
	if (str == NULL || length <= 0) { return; }	// �R�s�[�̕K�v����
	WriteInternal(str, length);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
String StringBuilder::ToString() const
{
	return String((const TCHAR*)m_buffer.GetConstData(), m_bufferUsed / sizeof(TCHAR));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void StringBuilder::WriteInternal(const TCHAR* str, int length)
{
	LN_ASSERT(str != NULL);

	size_t byteCount = sizeof(TCHAR) * length;

	// �o�b�t�@������Ȃ���Ίg������
	if (m_bufferUsed + byteCount > m_buffer.GetSize())
	{
		size_t newSize = m_buffer.GetSize() + std::max(m_buffer.GetSize(), byteCount);	// �Œ�ł� byteCount �����g������
		m_buffer.Resize(newSize, false);
	}

	// �����ɃR�s�[
	byte_t* ptr = &(m_buffer.GetData()[m_bufferUsed]);
	size_t size = m_buffer.GetSize() - m_bufferUsed;
	memcpy_s(ptr, size, str, byteCount);

	m_bufferUsed += byteCount;
}

} // namespace Lumino
