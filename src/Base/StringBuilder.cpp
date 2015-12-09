
#include "../Internal.h"
#include <Lumino/Base/StringTraits.h>
#include <Lumino/Base/StringBuilder.h>

LN_NAMESPACE_BEGIN

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
void GenericStringBuilderCore<TChar>::Append(const TChar ch)
{
	Append(&ch, 1);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void GenericStringBuilderCore<TChar>::Append(const TChar ch, int count)
{
	for (int i = 0; i < count; ++i) {
		Append(ch);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void GenericStringBuilderCore<TChar>::Append(const TChar* str, int length)
{
	//if (str == NULL || length <= 0) { return; }	// �R�s�[�̕K�v����
	WriteInternal(str, (length < 0) ? StringTraits::StrLen(str) : length);
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
	int diff = sizeof(TChar) * (strLength - length);

	// �o�b�t�@������Ȃ���Ίg������
	if (diff > 0 && m_bufferUsed + diff > m_buffer.GetSize())
	{
		size_t newSize = m_buffer.GetSize() + std::max(m_buffer.GetSize(), (size_t)diff);
		m_buffer.Resize(newSize, false);
	}

	int diffChars = (strLength - length);
	// before �͒u�����Ȃ�����
	TChar* beforeBegin = (TChar*)m_buffer.GetData();
	TChar* beforeEnd = beforeBegin + start;				// ����1�O�܂ł� before �̕����B�S�Ēu������ꍇ�� beforeBegin �Ɠ����B
	
	// oldAfter �͎c������
	TChar* oldAfterBegin = beforeEnd + length;
	TChar* oldAfterEnd = beforeBegin + GetLength();		// ����1�O�܂ł� after �̕���
	int afterCount = oldAfterEnd - oldAfterBegin;

	// newAfter �� oldAfter �̈ړ��� (�������� oldAfter �Ɠ���)
	TChar* newAfterBegin = oldAfterBegin + diffChars;
	//TChar* newAfterEnd = newAfterBegin + afterCount;	// ����1�O�܂ł� after �̕���

	if (afterCount > 0) {
		memmove(newAfterBegin, oldAfterBegin, sizeof(TChar) * afterCount);
	}
	if (strLength > 0) {
		memcpy(beforeEnd, str, sizeof(TChar) * strLength);
	}

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

LN_NAMESPACE_END
