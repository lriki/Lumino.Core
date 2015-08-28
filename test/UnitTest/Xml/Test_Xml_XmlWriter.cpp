#include <TestConfig.h>
using namespace Text;

class Test_Xml_XmlWriter : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Xml_XmlWriter, Unit)
{
	// <Unit> WriteStartDocument
	{
		StringWriter strWriter;
		XmlWriter xmlWriter(&strWriter);
		xmlWriter.WriteStartDocument();
		xmlWriter.WriteEndDocument();
		ASSERT_STREQ(_T("<?xml version=\"1.0\"?>"), strWriter.ToString());

		xmlWriter.WriteStartDocument();
		ASSERT_THROW(xmlWriter.WriteStartDocument(), InvalidOperationException);
	}

	// <Unit> WriteStartElement
	{
		StringWriter strWriter;
		XmlWriter xmlWriter(&strWriter);
		xmlWriter.WriteStartElement(_T("test"));
		ASSERT_STREQ(_T("<test"), strWriter.ToString());
	}

	// <Unit> WriteEndElement
	{
		StringWriter strWriter;
		XmlWriter xmlWriter(&strWriter);
		xmlWriter.WriteStartElement(_T("test"));
		xmlWriter.WriteEndElement();
		ASSERT_STREQ(_T("<test />"), strWriter.ToString());
	}

	// <Unit> WriteAttribute
	{
		StringWriter strWriter;
		XmlWriter xmlWriter(&strWriter);
		xmlWriter.WriteStartElement(_T("test"));
		xmlWriter.WriteAttribute(_T("a1"), _T("v1"));
		xmlWriter.WriteAttribute(_T("a2"), _T("&\""));
		xmlWriter.WriteAttribute(_T("a3"), _T("v<v"));
		xmlWriter.WriteAttribute(_T("a4"), _T(">v>"));
		xmlWriter.WriteAttribute(_T("a5"), _T(">>v"));
		xmlWriter.WriteAttribute(_T("a6"), _T("v<<"));
		xmlWriter.WriteEndElement();
		ASSERT_STREQ(_T("<test a1=\"v1\" a2=\"&amp;&quot;\" a3=\"v&lt;v\" a4=\"&gt;v&gt;\" a5=\"&gt;&gt;v\" a6=\"v&lt;&lt;\" />"), strWriter.ToString());
	}
}
