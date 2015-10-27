﻿
#pragma once
#include "../Base/Stack.h"
#include "../IO/TextWriter.h"
#include "XmlError.h"
#include "XmlReader.h"	//TODO

LN_NAMESPACE_BEGIN
class StringWriter;

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
	virtual ~XmlWriter();

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

	/** 
		@brief		テキスト子要素を持つ要素を書き込みます。
		@details	この関数は以下の操作と同じ結果になるユーティリティです。
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
	@brief		指定したファイルへ書き込みを行う XML ライターです。
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
		@brief		書き込み先ファイルのパスを指定してインスタンスを初期化します。ファイルが存在しない場合は新しく作成します。
		@param[in]	filePath	: 書き込み先ファイルのパス
		@param[in]	encoding	: 書き込み時のエンコーディング (省略時は UTF-8)
	*/
	XmlFileWriter(const PathName& filePath, Encoding* encoding = NULL);

	virtual ~XmlFileWriter();
};


/**
	@brief		XML 文字列を作成するための XML ライターです。
	@code
				XmlStringWriter writer;
				writer.WriteStartElement(_T("EnvData"));
				writer.WriteTextElement(_T("ToolPath"), _T("dir/app.exe"));
				writer.WriteEndElement();
				String xmlText = writer.ToString();		// String として取り出す
	@endcode
*/
class XmlStringWriter
	: public XmlWriter
{
public:
	XmlStringWriter();
	virtual ~XmlStringWriter();
	
	/**
		@brief		この TextWriter で使用する改行文字列を設定します。
		@details	規定値は String::GetNewLine() で取得できる値です。
	*/
	void SetNewLine(const String& newLine);

	/** XML 文字列を取得します。*/
	String ToString();

private:
	StringWriter* m_stringWriter;
};

LN_NAMESPACE_END
