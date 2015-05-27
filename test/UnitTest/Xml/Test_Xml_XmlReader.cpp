#include <TestConfig.h>
using namespace Text;

class Test_Xml_XmlReader : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

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

	//String str = FileUtils::ReadAllText(LOCALFILE("TestData/ReaderTest1.xml"), Text::Encoding::GetSystemMultiByteEncoding());
	//StringReader textReader(str);
	//XmlReader reader(&textReader);
	//reader.Read();
	//XmlNodeType t = reader.GetNodeType();

	//reader.Read();
	//t = reader.GetNodeType();
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



