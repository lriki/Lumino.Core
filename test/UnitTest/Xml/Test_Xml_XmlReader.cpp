﻿#include <TestConfig.h>
#include <Lumino/IO/StreamReader.h>
#include <Lumino/Xml/XmlReader.h>

class Test_Xml_XmlReader : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};
// TODO: ' 文字列に対応して無いかも

//---------------------------------------------------------------------
TEST_F(Test_Xml_XmlReader, User)
{
	StreamReader textReader(LOCALFILE("TestData/ReaderExample1.xml"));
	XmlReader reader(&textReader);	// TODO: XmlFileReader とか。

	String output;
	while (reader.Read())
	{
		if (reader.GetNodeType() == XmlNodeType::Text) {
			output += String::SPrintf(_T("type:%s value:%s\n"), reader.GetNodeType().ToString().c_str(), reader.GetValue().c_str());
		}
		else if (reader.GetNodeType() == XmlNodeType::Whitespace) {
			output += String::SPrintf(_T("type:%s\n"), reader.GetNodeType().ToString().c_str());
		}
		else {
			output += String::SPrintf(_T("type:%s name:%s\n"), reader.GetNodeType().ToString().c_str(), reader.GetName().c_str());
		}

		if (reader.MoveToFirstAttribute())
		{
			do {
				output += String::SPrintf(_T("type:%s name:%s value:%s\n"), reader.GetNodeType().ToString().c_str(), reader.GetName().c_str(), reader.GetValue().c_str());

			} while (reader.MoveToNextAttribute());
		}
	}

	const TCHAR* result =
		_T("type:Element name:ToolSettings\n")
		_T("type:Whitespace\n")
		_T("type:Element name:WorkingDirectory\n")
		_T("type:Text value:C:\\workspace\n")
		_T("type:EndElement name:WorkingDirectory\n")
		_T("type:Whitespace\n")
		_T("type:Element name:ProjectList\n")
		_T("type:Whitespace\n")
		_T("type:Element name:Project\n")
		_T("type:Attribute name:Repository value:server1\n")
		_T("type:Whitespace\n")
		_T("type:Element name:Branch\n")
		_T("type:Attribute name:Name value:master\n")
		_T("type:Attribute name:LastCommit value:d0c5d2fd1e3c7a457776e1ca1290942374b8e0fc\n")
		_T("type:Whitespace\n")
		_T("type:EndElement name:Project\n")
		_T("type:Whitespace\n")
		_T("type:Element name:Project\n")
		_T("type:Attribute name:Repository value:server2\n")
		_T("type:Attribute name:FilterExt value:.c;.cpp;.h\n")
		_T("type:Whitespace\n")
		_T("type:Element name:Branch\n")
		_T("type:Attribute name:Name value:master\n")
		_T("type:Attribute name:LastCommit value:0c9fa5c4509bb5d1043c2d906d88b8507d6b7546\n")
		_T("type:Whitespace\n")
		_T("type:Element name:Branch\n")
		_T("type:Attribute name:Name value:Custom\n")
		_T("type:Attribute name:LastCommit value:662a470c700fc6fa6bc95d0cab8b8ec39bc1cc23\n")
		_T("type:Whitespace\n")
		_T("type:EndElement name:Project\n")
		_T("type:Whitespace\n")
		_T("type:EndElement name:ProjectList\n")
		_T("type:Whitespace\n")
		_T("type:EndElement name:ToolSettings\n");
	ASSERT_EQ(result, output);
}


//---------------------------------------------------------------------
// Element ノードのテスト
TEST_F(Test_Xml_XmlReader, Element)
{
	// <Test> Read() の前に GetNodeType() すると None が返る。
	{
		String xml = _T("<test />");
		XmlReader reader(xml);
		ASSERT_EQ(XmlNodeType::None, reader.GetNodeType());
	}

	// ・空要素
	{
		String xml = _T("<test />");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_EQ(_T("test"), reader.GetName());
		ASSERT_EQ(true, reader.IsEmptyElement());			// 空要素である
		ASSERT_EQ(false, reader.Read());					// EOF
		ASSERT_EQ(XmlNodeType::None, reader.GetNodeType());	// EOF の後は None
	}

	// ・End Element
	{
		String xml = _T("<test></test>");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_EQ(_T("test"), reader.GetName());
		ASSERT_EQ(false, reader.IsEmptyElement());			// 空要素ではない

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::EndElement, reader.GetNodeType());
		ASSERT_EQ(_T("test"), reader.GetName());
		ASSERT_EQ(false, reader.IsEmptyElement());			// 空要素ではない

		ASSERT_EQ(false, reader.Read());					// EOF
	}

	// ・ネスト
	{
		String xml = _T("<n1><n2/></n1>");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_EQ(_T("n1"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_EQ(_T("n2"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(4, reader.m_textCache.GetCount());
		ASSERT_EQ(XmlNodeType::EndElement, reader.GetNodeType());
		ASSERT_EQ(_T("n1"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// EndElement は空要素扱いではない

		ASSERT_FALSE(reader.Read());						// EOF
	}

	// ・名前
	{
		String xml = _T("<v:test/><b.n1/>");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_EQ(_T("v:test"), reader.GetName());

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_EQ(_T("b.n1"), reader.GetName());

		ASSERT_FALSE(reader.Read());					// EOF
	}
}

//---------------------------------------------------------------------
// Text ノードのテスト
TEST_F(Test_Xml_XmlReader, Text)
{
	// ・Text ノード
	// ・タグ末尾の空白
	{
		String xml = _T("<test >Text</test >");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_EQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// 空要素ではない

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_TRUE(reader.GetName().IsEmpty());		// Text ノードの名前は空
		ASSERT_EQ(_T("Text"), reader.GetValue());	// テキスト値
		ASSERT_FALSE(reader.IsEmptyElement());			// 空要素ではない (というか要素ではない)

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::EndElement, reader.GetNodeType());
		ASSERT_EQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// 空要素ではない

		ASSERT_FALSE(reader.Read());					// EOF
	}

	// ・Text ノード前後の空白
	{
		String xml = _T("<test> \nText\r\t</test>");
		XmlReader reader(xml);
		reader.Read();

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_EQ(_T(" \nText\r\t"), reader.GetValue());	// テキスト値

		reader.Read();
		ASSERT_FALSE(reader.Read());						// EOF
	}
}

//---------------------------------------------------------------------
// Comment ノードのテスト
TEST_F(Test_Xml_XmlReader, Comment)
{
	{
		String xml = _T("<!-- Comment -->");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Comment, reader.GetNodeType());
		ASSERT_EQ(_T(" Comment "), reader.GetValue());	// コメントの値

		ASSERT_FALSE(reader.Read());						// EOF
	}
}

//---------------------------------------------------------------------
// Attribute
TEST_F(Test_Xml_XmlReader, Attribute)
{
	// 単一属性
	{
		String xml = _T("<test a=\"10\" />");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_EQ(_T("test"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(1, reader.GetAttributeCount());		// 属性1個
		ASSERT_FALSE(reader.MoveToElement());			// 意味無し

		ASSERT_TRUE(reader.MoveToFirstAttribute());
		ASSERT_EQ(XmlNodeType::Attribute, reader.GetNodeType());
		ASSERT_EQ(_T("a"), reader.GetName());
		ASSERT_EQ(_T("10"), reader.GetValue());

		ASSERT_TRUE(reader.MoveToElement());	// Element に戻らないと、IsEmptyElement が機能しない
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_EQ(_T("test"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(1, reader.GetAttributeCount());		// 属性1個

		ASSERT_FALSE(reader.Read());					// EOF
	}

	//・複数属性
	//・名前に . と :
	{
		String xml = _T("<test v.a=\"10\" v:b=\"ABC\" />");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(2, reader.GetAttributeCount());		// 属性2個

		// v.a="10"
		ASSERT_TRUE(reader.MoveToFirstAttribute());
		ASSERT_EQ(XmlNodeType::Attribute, reader.GetNodeType());
		ASSERT_EQ(_T("v.a"), reader.GetName());
		ASSERT_EQ(_T("10"), reader.GetValue());

		// v:b="ABC"
		ASSERT_TRUE(reader.MoveToNextAttribute());
		ASSERT_EQ(XmlNodeType::Attribute, reader.GetNodeType());
		ASSERT_EQ(_T("v:b"), reader.GetName());
		ASSERT_EQ(_T("ABC"), reader.GetValue());

		ASSERT_FALSE(reader.MoveToNextAttribute());		// 次は無い

		ASSERT_FALSE(reader.Read());					// EOF
	}

	{
		String xml = _T("<test v=\"a\"></test>");
		XmlReader reader(xml);

		ASSERT_EQ(true, reader.Read());
		ASSERT_EQ(1, reader.GetAttributeCount());

		reader.MoveToElement();
		reader.Read();

		ASSERT_EQ(XmlNodeType::EndElement, reader.GetNodeType());
		ASSERT_EQ(_T("test"), reader.GetName());
	}
}

//---------------------------------------------------------------------
// 予約済み Entity
TEST_F(Test_Xml_XmlReader, ReservedEntity)
{
	//・全ての予約済み Entity の展開
	{
		String xml = _T("<root><ch>&lt;<ch/><ch>&gt;<ch/><ch>&amp;<ch/><ch>&apos;<ch/><ch>&quot;<ch/><root/>");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());	// <root>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &lt;
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_EQ(_T("<"), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &gt;
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_EQ(_T(">"), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &amp;
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_EQ(_T("&"), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &apos;
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_EQ(_T("'"), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &quot;
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_EQ(_T("\""), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <root/>

		ASSERT_FALSE(reader.Read());// EOF
	}

	//・属性
	//・未定義Entity
	//・前後の文字と空白
	{
		String xml = _T("<test a=\"&lt;\" b=\"&AAAA;\"  c=\"A&lt; \" />");
		XmlReader reader(xml);
		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(3, reader.GetAttributeCount());		// 属性1個

		ASSERT_TRUE(reader.MoveToFirstAttribute());
		ASSERT_EQ(XmlNodeType::Attribute, reader.GetNodeType());
		ASSERT_EQ(_T("a"), reader.GetName());
		ASSERT_EQ(_T("<"), reader.GetValue());

		ASSERT_TRUE(reader.MoveToNextAttribute());
		ASSERT_EQ(XmlNodeType::Attribute, reader.GetNodeType());
		ASSERT_EQ(_T("b"), reader.GetName());
		ASSERT_EQ(_T("&AAAA;"), reader.GetValue());	// 認識できない Entity はエラーにせず、テキストとして返す

		ASSERT_TRUE(reader.MoveToNextAttribute());
		ASSERT_EQ(XmlNodeType::Attribute, reader.GetNodeType());
		ASSERT_EQ(_T("c"), reader.GetName());
		ASSERT_EQ(_T("A< "), reader.GetValue());

		ASSERT_FALSE(reader.Read());					// EOF
	}
}

//---------------------------------------------------------------------
// Entity 参照
TEST_F(Test_Xml_XmlReader, EntityReference)
{
	// ・ノード前後のテキストと空白
	{
		String xml = _T("<a> A&book; </a>");
		XmlReader reader(xml);
		ASSERT_TRUE(reader.Read());		// <a>

		ASSERT_TRUE(reader.Read());		// " A"
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_EQ(_T(" A"), reader.GetValue());

		ASSERT_TRUE(reader.Read());		// "&book;"
		ASSERT_EQ(XmlNodeType::EntityReference, reader.GetNodeType());
		ASSERT_EQ(_T("book"), reader.GetName());
		ASSERT_TRUE(reader.GetValue().IsEmpty());	// 値は空

		ASSERT_TRUE(reader.Read());		// " "
		ASSERT_EQ(XmlNodeType::Whitespace, reader.GetNodeType());
		ASSERT_EQ(_T(" "), reader.GetValue());

		ASSERT_TRUE(reader.Read());		// </a>

		ASSERT_FALSE(reader.Read());	// EOF
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Xml_XmlReader, SystemTest)
{
	// <> ReadString
	{
		String xml =
			_T("<root>\n")
			_T("  <test>  AAA  </test>\n")
			_T("</root>");
		XmlReader reader(xml);

		List<XmlNodeType> types;
		while (reader.Read())
		{
			types.Add(reader.GetNodeType());
			if (reader.IsStartElement(_T("test")))
			{
				types.Add(reader.GetNodeType());
				ASSERT_EQ(_T("  AAA  "), reader.ReadString());
				types.Add(reader.GetNodeType());	// EndElement
			}
		}
		ASSERT_EQ(5, types.GetCount());
		ASSERT_EQ(XmlNodeType::Element, types[0]);
		ASSERT_EQ(XmlNodeType::Whitespace, types[1]);
		ASSERT_EQ(XmlNodeType::Element, types[2]);
		ASSERT_EQ(XmlNodeType::EndElement, types[3]);
		ASSERT_EQ(XmlNodeType::Whitespace, types[4]);
		//ASSERT_EQ(XmlNodeType::EndElement, types[5]);	最後のは IsStartElement で消費されるので出てこない
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Xml_XmlReader, Issues)
{
	{
		String xml = _T("<a b=\"c\" />");
		XmlReader reader(xml);

		reader.Read();
		ASSERT_EQ(true, reader.IsEmptyElement());
	}

	XmlFileReader reader(LN_LOCALFILE("TestData/Issue1.xml"));

	//while (reader.Read());

}
