
#include "../Internal.h"
#include <Lumino/Xml/XmlWriter.h>

namespace Lumino
{


//=============================================================================
// XmlWriter
//=============================================================================
const TCHAR* XmlWriter::DefaultIndentString = _T("  ");

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
XmlWriter::XmlWriter(TextWriter* textWriter)
	: m_textWriter()
	, m_state(State_Start)
	, m_indentString(DefaultIndentString)
	, m_quoteChar(_T('"'))
{
	m_textWriter = textWriter;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
XmlWriter::~XmlWriter()
{

}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void XmlWriter::WriteStartDocument()
{
	LN_CHECK_STATE(m_state == State_Start);

	// TODO: encoding 名
	// http://www.iana.org/assignments/character-sets/character-sets.xhtml
	m_textWriter->Write(_T("<?xml "));
	m_textWriter->Write(_T("version=\"1.0\""));
	m_textWriter->Write(_T("?>"));
	m_state = State_Prolog;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void XmlWriter::WriteEndDocument()
{
	m_state = State_Start;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void XmlWriter::WriteStartElement(const String& name)
{
	Indent();
	m_textWriter->WriteChar(_T('<'));
	m_textWriter->Write(name);

	ElementInfo info;
	info.Name = name;
	m_elementStack.Push(info);
	m_state = State_StartElement;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void XmlWriter::WriteEndElement()
{
	LN_CHECK_STATE(!m_elementStack.IsEmpty());
	LN_CHECK_STATE(m_state == State_StartElement || m_state == State_EndAttribute);

	if (m_state == State_StartElement || m_state == State_EndAttribute) {
		m_textWriter->Write(_T(" />"));
	}
	else
	{
		m_textWriter->Write(_T("</"));
		m_textWriter->Write(m_elementStack.GetTop().Name);
		m_textWriter->WriteChar(_T('>'));
	}

	m_elementStack.Pop();
	m_state = State_Prolog;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void XmlWriter::WriteAttribute(const String& name, const String& value)
{
	WriteStartAttribute(name);
	WriteStringInternal(value.GetCStr(), value.GetLength(), true);
	WriteEndAttribute();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void XmlWriter::WriteStartAttribute(const String& name)
{
	LN_CHECK_STATE(m_state == State_StartElement || m_state == State_EndAttribute);

	m_textWriter->WriteChar(_T(' '));
	m_textWriter->Write(name);
	m_textWriter->Write(_T("=\""));
	m_state = State_StartAttribute;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void XmlWriter::WriteEndAttribute()
{
	m_textWriter->Write(_T("\""));
	m_state = State_EndAttribute;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void XmlWriter::WriteStringInternal(const TCHAR* str, int len, bool inAttribute)
{
	if (str == NULL || len == 0) { return; }

	Text::Encoding* enc = Text::Encoding::GetTCharEncoding();
	const TCHAR* begin = str;
	const TCHAR* end = begin + len;
	const TCHAR* pos = begin;
	while (pos < end)
	{
		int extra = enc->GetLeadExtraLength(pos, end - pos);
		if (extra > 0) 
		{
			// 先行バイトだった。そのまま進める
			pos += extra;
		}
		else
		{
			TCHAR ch = *pos;
			switch (ch)
			{
			case (char)0xA:
			case (char)0xD:
				m_textWriter->Write(begin, pos - begin);	// pos の前までを出力
				begin = pos + 1;

				if (inAttribute) {
					// 3.3.3 属性値正規化
					m_textWriter->WriteChar(_T(' '));
				}
				else {
					m_textWriter->WriteChar(ch);
				}
				break;
			case '<':
				m_textWriter->Write(begin, pos - begin);	// pos の前までを出力
				begin = pos + 1;
				m_textWriter->Write(_T("&lt;"), 4);
				break;
			case '>':
				m_textWriter->Write(begin, pos - begin);	// pos の前までを出力
				begin = pos + 1;
				m_textWriter->Write(_T("&gt;"), 4);
				break;
			case '&':
				m_textWriter->Write(begin, pos - begin);	// pos の前までを出力
				begin = pos + 1;
				m_textWriter->Write(_T("&amp;"), 5);
				break;
			case '\'':
				m_textWriter->Write(begin, pos - begin);	// pos の前までを出力
				begin = pos + 1;
				if (inAttribute && m_quoteChar == ch) {
					m_textWriter->Write(_T("&apos;"), 6);
				}
				else {
					m_textWriter->WriteChar('\'');
				}
				break;
			case '"':
				m_textWriter->Write(begin, pos - begin);	// pos の前までを出力
				begin = pos + 1;
				if (inAttribute && m_quoteChar == ch) {
					m_textWriter->Write(_T("&quot;"), 6);
				}
				else {
					m_textWriter->WriteChar('"');
				}
				break;
			default:

				

				break;
			}

		}

		++pos;
	}

	if (begin < pos)
	{
		m_textWriter->Write(begin, pos - begin);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void XmlWriter::Indent()
{
	if (m_elementStack.IsEmpty()) {
	}
	else if (m_elementStack.GetCount() == 1) {
		m_textWriter->WriteLine();
	}
	else
	{
		m_textWriter->WriteLine();
		for (int i = 0; i < m_elementStack.GetCount(); ++i) {
			m_textWriter->Write(m_indentString);
		}
	}
}

} // namespace Lumino
