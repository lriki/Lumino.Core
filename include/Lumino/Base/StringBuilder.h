
#pragma once
#include "ByteBuffer.h"
#include "String.h"

namespace Lumino
{

/**
	@brief
*/
class StringBuilder
{
public:
	StringBuilder();
	~StringBuilder();

public:
	void Append(const TCHAR* str, int length);
	String ToString() const;

private:
	void WriteInternal(const TCHAR* str, int length);

private:
	ByteBuffer	m_buffer;
	size_t		m_bufferUsed;	///< m_buffer 内の使用中バイト数
};

} // namespace Lumino
