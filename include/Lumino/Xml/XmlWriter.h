
#pragma once
#include "../Base/Stack.h"
#include "../IO/TextWriter.h"
#include "XmlError.h"
#include "XmlReader.h"	//TODO

namespace Lumino
{

/**
	@brief		SAX �X�^�C���� XML ���C�^�[�ł��B
	@details	�uExtensible Markup Language (XML) 1.1 (����)�v���x�[�X�Ɏ�������Ă��܂��B
				http://www.eplusx.net/translation/W3C/REC-xml11-20060816/
*/
class XmlWriter
{
public:
	static const TCHAR* DefaultIndentString;

public:
	XmlWriter(TextWriter* textWriter);
	~XmlWriter();

public:
	/** XML �錾���������݂܂��B*/
	void WriteStartDocument();

	/** XML �h�L�������g����܂��B*/
	void WriteEndDocument();

	/** �v�f�̊J�n�^�O���������݂܂��B*/
	void WriteStartElement(const String& name);

	/** �v�f�̏I���^�O���������݂܂��B�������ЂƂ������ꍇ�͋�v�f�^�O ("/>") �ƂȂ�܂��B*/
	void WriteEndElement();

	/** �w�肵�����O�ƕ�����l�̑������������݂܂��B*/
	void WriteAttribute(const String& name, const String& value);

	/** �w�肵���e�L�X�g���������݂܂��B*/
	void WriteString(const String& text);

	/** �R�����g�u���b�N���������݂܂��B*/
	void WriteComment(const String& text);

	/** <![CDATA[...]]> �u���b�N���������݂܂��B*/
	void WriteCData(const String& text);

private:
	void WriteStartAttribute(const String& name);
	void WriteEndAttribute();
	void WriteStringInternal(const TCHAR* str, int len, bool inAttribute);
	void PreWrite(XmlNodeType type);
	void WriteStartTagEnd(bool empty);
	void Indent(bool beforeEndElement);

private:
	enum State
	{
		State_Start = 0,
		State_Prolog,
		State_StartElement,
		State_Attribute,
		State_Text,
	};

	struct ElementInfo
	{
		String	Name;
		bool	IndentSkip;
	};

	RefPtr<TextWriter>		m_textWriter;
	State					m_state;
	Stack<ElementInfo>		m_elementStack;
	String					m_indentString;
	TCHAR					m_quoteChar;
};

} // namespace Lumino
