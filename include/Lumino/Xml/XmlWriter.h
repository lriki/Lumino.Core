
#pragma once
#include "../IO/TextWriter.h"
#include "XmlError.h"

namespace Lumino
{

/**
	@brief		SAX スタイルの XML ライターです。
	@details	「Extensible Markup Language (XML) 1.1 (第二版)」をベースに実装されています。
				http://www.eplusx.net/translation/W3C/REC-xml11-20060816/#NT-elementdecl
*/
class XmlWriter
{
public:
	XmlWriter(TextWriter* textWriter);
	~XmlWriter();

public:
	/** XML 宣言を書き込みます。*/
	void WriteStartDocument();

	/** XML 宣言を閉じ、終端要素を書き込みます。*/
	void WriteEndDocument();

private:
	RefPtr<TextWriter>		m_textWriter;
};

} // namespace Lumino
