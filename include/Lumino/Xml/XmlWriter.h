
#pragma once
#include "../Base/Stack.h"
#include "../IO/TextWriter.h"
#include "XmlError.h"
#include "XmlReader.h"	//TODO

namespace Lumino
{
class StringWriter;

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
	virtual ~XmlWriter();

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

	/** 
		@brief		�e�L�X�g�q�v�f�����v�f���������݂܂��B
		@details	���̊֐��͈ȉ��̑���Ɠ������ʂɂȂ郆�[�e�B���e�B�ł��B
		@code
					WriteStartElement(elementName);
					WriteString(text);
					WriteEndElement();
		@endcode
	*/
	void WriteElementString(const String& elementName, const String& text);

protected:
	void Initialize(TextWriter* textWriter);

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


/**
	@brief		�w�肵���t�@�C���֏������݂��s�� XML ���C�^�[�ł��B
	@code
				XmlFileWriter writer("test.xml");
				writer.WriteStartElement(_T("EnvData"));
				writer.WriteTextElement(_T("ToolPath"), _T("dir/app.exe"));
				writer.WriteEndElement();
	@endcode
*/
class XmlFileWriter
	: public XmlWriter
{
public:

	/** 
		@brief		�������ݐ�t�@�C���̃p�X���w�肵�ăC���X�^���X�����������܂��B�t�@�C�������݂��Ȃ��ꍇ�͐V�����쐬���܂��B
		@param[in]	filePath	: �������ݐ�t�@�C���̃p�X
		@param[in]	encoding	: �������ݎ��̃G���R�[�f�B���O (�ȗ����� UTF-8)
	*/
	XmlFileWriter(const PathName& filePath, Encoding* encoding = NULL);

	virtual ~XmlFileWriter();
};


/**
	@brief		XML ��������쐬���邽�߂� XML ���C�^�[�ł��B
	@code
				XmlStringWriter writer;
				writer.WriteStartElement(_T("EnvData"));
				writer.WriteTextElement(_T("ToolPath"), _T("dir/app.exe"));
				writer.WriteEndElement();
				String xmlText = writer.ToString();		// String �Ƃ��Ď��o��
	@endcode
*/
class XmlStringWriter
	: public XmlWriter
{
public:
	XmlStringWriter();
	virtual ~XmlStringWriter();
	
	/**
		@brief		���� TextWriter �Ŏg�p������s�������ݒ肵�܂��B
		@details	�K��l�� String::GetNewLine() �Ŏ擾�ł���l�ł��B
	*/
	void SetNewLine(const String& newLine);

	/** XML ��������擾���܂��B*/
	String ToString();

private:
	StringWriter* m_stringWriter;
};

} // namespace Lumino
