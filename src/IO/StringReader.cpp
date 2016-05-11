
#include "../Internal.h"
#include <Lumino/IO/StringReader.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// TextReader
//==============================================================================

//------------------------------------------------------------------------------
TextReader::~TextReader()
{
}

//==============================================================================
// StringReader
//==============================================================================

//------------------------------------------------------------------------------
StringReader::StringReader(const String& str)
	: m_src(str)
	, m_range(m_src)
	, m_pos(0)
{
}

//------------------------------------------------------------------------------
StringReader::StringReader(const TCHAR* str)
	: m_src(str)
	, m_range(m_src)
	, m_pos(0)
{
}

//------------------------------------------------------------------------------
StringReader::StringReader(const StringRef& str)
	: m_src()
	, m_range(str)
	, m_pos(0)
{
}

//------------------------------------------------------------------------------
StringReader::~StringReader()
{
}

//------------------------------------------------------------------------------
int StringReader::Peek()
{
	if (m_pos >= m_range.GetLength()) {
		return -1;
	}
	return m_range[m_pos];
}

//------------------------------------------------------------------------------
int StringReader::Read()
{
	if (m_pos >= m_range.GetLength()) {
		return -1;
	}
	return m_range[m_pos++];
}

//------------------------------------------------------------------------------
bool StringReader::ReadLine(String* line)
{
	int i = m_pos;
	while (i < m_range.GetLength())
	{
		TCHAR ch = m_range[i];
		if (ch == '\r' || ch == '\n')
		{
			if (line != NULL) {
				*line = m_range.Mid(m_pos, i - m_pos);
			}
			m_pos = i + 1;
			if (ch == '\r' && m_pos < m_range.GetLength() && m_range[m_pos] == '\n') m_pos++;
			return true;
		}
		i++;
	}

	// i �� EOF �ɂ��ǂ蒅�����B�����āA�ǂݎ�镶��������ΓǂށB
	if (i > m_pos)
	{
		if (line != NULL) {
			*line = m_range.Mid(m_pos, i - m_pos);
		}
		m_pos = i;
		return true;
	}

	// EOF �ɂ��ǂ蒅�������A�ǂݎ�镶���͖��������B
	return false;
}

//------------------------------------------------------------------------------
String StringReader::ReadToEnd()
{
	String s;
	if (m_pos == 0) {
		s = m_range;
	}
	else {
		s = m_range.Mid(m_pos, m_range.GetLength() - m_pos);
	}
	m_pos = m_range.GetLength();
	return s;
}

//------------------------------------------------------------------------------
//int StringReader::GetPosition() const
//{
//	return m_pos;
//}

//------------------------------------------------------------------------------
bool StringReader::IsEOF()
{
	return (m_pos >= m_range.GetLength());
}

LN_NAMESPACE_END
