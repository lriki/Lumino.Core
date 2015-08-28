#include <TestConfig.h>
using namespace Text;

class Test_Xml_XmlReader : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

// TODO: ' 文字列に対応して無いかも

//---------------------------------------------------------------------
// Element ノードのテスト
TEST_F(Test_Xml_XmlReader, Element)
{
	// ・空要素
	{
		String xml = _T("<test />");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());			// 空要素である
		ASSERT_FALSE(reader.Read());					// EOF
	}

	// ・End Element
	{
		String xml = _T("<test></test>");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// 空要素ではない

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_EndElement, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// 空要素ではない

		ASSERT_FALSE(reader.Read());					// EOF
	}

	// ・ネスト
	{
		String xml = _T("<n1><n2/></n1>");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("n1"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("n2"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_EndElement, reader.GetNodeType());
		ASSERT_STREQ(_T("n1"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// EndElement は空要素扱いではない

		ASSERT_FALSE(reader.Read());						// EOF
	}

	// ・名前
	{
		String xml = _T("<v:test/><b.n1/>");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("v:test"), reader.GetName());

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("b.n1"), reader.GetName());

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
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// 空要素ではない

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Text, reader.GetNodeType());
		ASSERT_TRUE(reader.GetName().IsEmpty());		// Text ノードの名前は空
		ASSERT_STREQ(_T("Text"), reader.GetValue());	// テキスト値
		ASSERT_FALSE(reader.IsEmptyElement());			// 空要素ではない (というか要素ではない)

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_EndElement, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// 空要素ではない

		ASSERT_FALSE(reader.Read());					// EOF
	}

	// ・Text ノード前後の空白
	{
		String xml = _T("<test> \nText\r\t</test>");
		XmlReader reader(xml);
		reader.Read();

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Text, reader.GetNodeType());
		ASSERT_STREQ(_T(" \nText\r\t"), reader.GetValue());	// テキスト値

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
		ASSERT_EQ(XmlNodeType_Comment, reader.GetNodeType());
		ASSERT_STREQ(_T(" Comment "), reader.GetValue());	// コメントの値

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
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(1, reader.GetAttributeCount());		// 属性1個
		ASSERT_FALSE(reader.MoveToElement());			// 意味無し

		ASSERT_TRUE(reader.MoveToFirstAttribute());
		ASSERT_EQ(XmlNodeType_Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("a"), reader.GetName());
		ASSERT_STREQ(_T("10"), reader.GetValue());

		ASSERT_TRUE(reader.MoveToElement());	// Element に戻らないと、IsEmptyElement が機能しない
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
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
		ASSERT_EQ(XmlNodeType_Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("v.a"), reader.GetName());
		ASSERT_STREQ(_T("10"), reader.GetValue());

		// v:b="ABC"
		ASSERT_TRUE(reader.MoveToNextAttribute());
		ASSERT_EQ(XmlNodeType_Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("v:b"), reader.GetName());
		ASSERT_STREQ(_T("ABC"), reader.GetValue());

		ASSERT_FALSE(reader.MoveToNextAttribute());		// 次は無い

		ASSERT_FALSE(reader.Read());					// EOF
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
		ASSERT_EQ(XmlNodeType_Text, reader.GetNodeType());
		ASSERT_STREQ(_T("<"), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &gt;
		ASSERT_EQ(XmlNodeType_Text, reader.GetNodeType());
		ASSERT_STREQ(_T(">"), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &amp;
		ASSERT_EQ(XmlNodeType_Text, reader.GetNodeType());
		ASSERT_STREQ(_T("&"), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &apos;
		ASSERT_EQ(XmlNodeType_Text, reader.GetNodeType());
		ASSERT_STREQ(_T("'"), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &quot;
		ASSERT_EQ(XmlNodeType_Text, reader.GetNodeType());
		ASSERT_STREQ(_T("\""), reader.GetValue());

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
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(3, reader.GetAttributeCount());		// 属性1個

		ASSERT_TRUE(reader.MoveToFirstAttribute());
		ASSERT_EQ(XmlNodeType_Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("a"), reader.GetName());
		ASSERT_STREQ(_T("<"), reader.GetValue());

		ASSERT_TRUE(reader.MoveToNextAttribute());
		ASSERT_EQ(XmlNodeType_Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("b"), reader.GetName());
		ASSERT_STREQ(_T("&AAAA;"), reader.GetValue());	// 認識できない Entity はエラーにせず、テキストとして返す

		ASSERT_TRUE(reader.MoveToNextAttribute());
		ASSERT_EQ(XmlNodeType_Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("c"), reader.GetName());
		ASSERT_STREQ(_T("A< "), reader.GetValue());

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
		ASSERT_EQ(XmlNodeType_Text, reader.GetNodeType());
		ASSERT_STREQ(_T(" A"), reader.GetValue());

		ASSERT_TRUE(reader.Read());		// "&book;"
		ASSERT_EQ(XmlNodeType_EntityReference, reader.GetNodeType());
		ASSERT_STREQ(_T("book"), reader.GetName());
		ASSERT_TRUE(reader.GetValue().IsEmpty());	// 値は空

		ASSERT_TRUE(reader.Read());		// " "
		ASSERT_EQ(XmlNodeType_Whitespace, reader.GetNodeType());
		ASSERT_STREQ(_T(" "), reader.GetValue());

		ASSERT_TRUE(reader.Read());		// </a>

		ASSERT_FALSE(reader.Read());	// EOF
	}
}
