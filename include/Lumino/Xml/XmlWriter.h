
#pragma once
#include "../Base/Stack.h"
#include "../IO/TextWriter.h"
#include "XmlError.h"
#include "XmlReader.h"	//TODO

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

	/** XML ドキュメントを閉じます。*/
	void WriteEndDocument();

	/** 要素の開始タグを書き込みます。*/
	void WriteStartElement(const String& name);

	/** 要素の終了タグを書き込みます。属性がひとつも無い場合は空要素タグ ("/>") となります。*/
	void WriteEndElement();

	/** 指定した名前と文字列値の属性を書き込みます。*/
	void WriteAttribute(const String& name, const String& value);

	/** 指定したテキストを書き込みます。*/
	void WriteString(const String& text);

	/** コメントブロックを書き込みます。*/
	void WriteComment(const String& text);

	/** <![CDATA[...]]> ブロックを書き込みます。*/
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
