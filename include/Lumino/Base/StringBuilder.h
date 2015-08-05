
#pragma once
#include "ByteBuffer.h"
#include "String.h"

namespace Lumino
{

/**
	@brief
*/
template<typename TChar>
class GenericStringBuilderCore
{
public:
	GenericStringBuilderCore();
	~GenericStringBuilderCore();

public:
	void Clear();
	void Append(const TChar* str, int length);
	void Append(const byte_t* buffer, int byteCount);
	void Append(const ByteBuffer& buffer);

	const TChar* GetCStr() const { return (const TChar*)m_buffer.GetConstData(); }
	int GetLength() const { return m_bufferUsed / sizeof(TChar); }

private:
	void WriteInternal(const TChar* str, int length);

protected:
	ByteBuffer	m_buffer;
	size_t		m_bufferUsed;	///< m_buffer 内の使用中バイト数
};


template<typename TChar>
class GenericStringBuilder
	: public GenericStringBuilderCore<TChar>
{
public:
	GenericString<TChar> ToString() const
	{
		return GenericString<TChar>(
			(const TChar*)GenericStringBuilderCore<TChar>::m_buffer.GetConstData(),
			GenericStringBuilderCore<TChar>::m_bufferUsed / sizeof(TChar));
	}
};

typedef GenericStringBuilder<TCHAR>	StringBuilder;

} // namespace Lumino
