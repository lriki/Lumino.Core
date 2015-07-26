
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
	if (str == NULL || length <= 0) { return; }	// コピーの必要無し
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

	// バッファが足りなければ拡張する
	if (m_bufferUsed + byteCount > m_buffer.GetSize())
	{
		size_t newSize = m_buffer.GetSize() + std::max(m_buffer.GetSize(), byteCount);	// 最低でも byteCount 分を拡張する
		m_buffer.Resize(newSize, false);
	}

	// 末尾にコピー
	byte_t* ptr = &(m_buffer.GetData()[m_bufferUsed]);
	size_t size = m_buffer.GetSize() - m_bufferUsed;
	memcpy_s(ptr, size, str, byteCount);

	m_bufferUsed += byteCount;
}

// テンプレートのインスタンス化
template class GenericStringBuilderCore<char>;
template class GenericStringBuilderCore<wchar_t>;
template class GenericStringBuilderCore<UTF32>;

} // namespace Lumino
