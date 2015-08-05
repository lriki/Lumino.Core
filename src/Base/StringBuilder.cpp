
#include "../Internal.h"
#include <Lumino/Base/StringBuilder.h>

namespace Lumino
{

//=============================================================================
// GenericStringBuilderCore
//=============================================================================

const int DefaultCapacity = 512;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
GenericStringBuilderCore<TChar>::GenericStringBuilderCore()
	: m_buffer()
	, m_bufferUsed(0)
{
	m_buffer.Resize(DefaultCapacity, false);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
GenericStringBuilderCore<TChar>::~GenericStringBuilderCore()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void GenericStringBuilderCore<TChar>::Clear()
{
	m_bufferUsed = 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void GenericStringBuilderCore<TChar>::Append(const TChar* str, int length)
{
	if (str == NULL || length <= 0) { return; }	// �R�s�[�̕K�v����
	WriteInternal(str, length);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void GenericStringBuilderCore<TChar>::Append(const byte_t* buffer, int byteCount)
{
	Append((const TChar*)buffer, byteCount / sizeof(TChar));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void GenericStringBuilderCore<TChar>::Append(const ByteBuffer& buffer)
{
	Append(buffer.GetData(), buffer.GetSize());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//template<typename TChar>
//GenericString<TChar> GenericStringBuilderCore<TChar>::ToString() const
//{
//	return GenericString<TChar>((const TChar*)m_buffer.GetConstData(), m_bufferUsed / sizeof(TChar));
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void GenericStringBuilderCore<TChar>::WriteInternal(const TChar* str, int length)
{
	LN_ASSERT(str != NULL);

	size_t byteCount = sizeof(TChar) * length;

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

// �e���v���[�g�̃C���X�^���X��
template class GenericStringBuilderCore<char>;
template class GenericStringBuilderCore<wchar_t>;
#ifdef LN_WCHAR_16	// GCC �Ł��� wchar_t �̂Ƒ��d��`�ɂȂ�΍�
template class GenericStringBuilderCore<UTF32>;
#endif

} // namespace Lumino
