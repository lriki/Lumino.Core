
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
template<typename TChar>
void GenericStringBuilderCore<TChar>::Replace(int start, int length, const TChar* str, int strLength)
{
	// �u�������牽 byte ������H����H
	size_t diff = sizeof(TChar) * (strLength - length);

	// �o�b�t�@������Ȃ���Ίg������
	if (diff > 0 && m_bufferUsed + diff > m_buffer.GetSize())
	{
		size_t newSize = m_buffer.GetSize() + std::max(m_buffer.GetSize(), diff);
		m_buffer.Resize(newSize, false);
	}

	int diffChars = (strLength - length);
	TChar* beforeBegin = (TChar*)m_buffer.GetData();
	TChar* beforeEnd = beforeBegin + start;				// ����1�O�܂ł� before �̕����B�S�Ēu������ꍇ�� beforeBegin �Ɠ����B
	TChar* oldAfterBegin = beforeBegin + start;
	TChar* oldAfterEnd = beforeBegin + GetLength();		// ����1�O�܂ł� after �̕���
	TChar* newAfterBegin = oldAfterBegin + diffChars;
	TChar* newAfterEnd = oldAfterEnd + diffChars;		// ����1�O�܂ł� after �̕���

	int moveCharCount = oldAfterEnd - oldAfterBegin;
	if (moveCharCount > 0) {
		memmove(newAfterBegin, oldAfterBegin, sizeof(TChar) * moveCharCount);
	}

	memcpy(beforeEnd, str, sizeof(TChar) * strLength);

	m_bufferUsed += diff;
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
