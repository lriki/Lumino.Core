
#pragma once
#include "../Base/Stack.h"
#include "../IO/TextWriter.h"
#include "XmlError.h"

namespace Lumino
{

/**
	@brief		SAX スタイルの XML ライターです。
	@details	「Extensible Markup Language (XML) 1.1 (第二版)」をベースに実装されています。
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
	/** XML 宣言を書き込みます。*/
	void WriteStartDocument();

	/** XML 宣言を閉じ、終端要素を書き込みます。*/
	void WriteEndDocument();

	/** 要素の開始タグを書き込みます。*/
	void WriteStartElement(const String& name);

	/** 要素の終了タグを書き込みます。属性がひとつも無い場合は空要素タグ ("/>") となります。*/
	void WriteEndElement();

	/** 指定した名前と文字列値の属性を書き込みます。*/
	void WriteAttribute(const String& name, const String& value);

	/** */
	void WriteString(const String& value);

private:
	void WriteStartAttribute(const String& name);
	void WriteEndAttribute();
	void WriteStringInternal(const TCHAR* str, int len, bool inAttribute);
	void Indent();

private:
	enum State
	{
		State_Start = 0,
		State_Prolog,
		State_StartElement,
		State_StartAttribute,
		State_EndAttribute,
	};

	struct ElementInfo
	{
		String	Name;
	};

	RefPtr<TextWriter>		m_textWriter;
	State					m_state;
	Stack<ElementInfo>		m_elementStack;
	String					m_indentString;
	TCHAR					m_quoteChar;
};

} // namespace Lumino
