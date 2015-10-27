
#pragma once
#include "../Base/StringBuilder.h"
#include "TextWriter.h"

LN_NAMESPACE_BEGIN

/**
	@brief	
*/
class StringWriter
	: public TextWriter
{
public:
	StringWriter();
	virtual ~StringWriter();

public:
	String ToString() const;
	
private:
	virtual void WriteOverride(const void* data, size_t byteCount);

private:
	StringBuilder	m_builder;
};

LN_NAMESPACE_END
