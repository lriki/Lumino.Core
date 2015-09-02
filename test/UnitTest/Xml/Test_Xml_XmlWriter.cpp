#include <TestConfig.h>
using namespace Text;

class Test_Xml_XmlWriter : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Xml_XmlWriter, User)
{
	StreamWriter file(TEMPFILE("xml/Test.xml"));
	XmlWriter writer(&file);

	writer.WriteStartElement(_T("ToolSettings"));
	{
		writer.WriteStartElement(_T("WorkingDirectory"));
		writer.WriteString(_T("C:\\workspace"));
		writer.WriteEndElement();

		writer.WriteStartElement(_T("ProjectList"));
		{
			writer.WriteStartElement(_T("Project"));
			writer.WriteAttribute(_T("Repository"), _T("server1"));
			{
				writer.WriteStartElement(_T("Branch"));
				writer.WriteAttribute(_T("Name"), _T("master"));
				writer.WriteAttribute(_T("LastCommit"), _T("d0c5d2fd1e3c7a457776e1ca1290942374b8e0fc"));
				writer.WriteEndElement();
			}
			writer.WriteEndElement();

			writer.WriteStartElement(_T("Project"));
			writer.WriteAttribute(_T("Repository"), _T("server2"));
			writer.WriteAttribute(_T("FilterExt"), _T(".c;.cpp;.h"));
			{
				writer.WriteStartElement(_T("Branch"));
				writer.WriteAttribute(_T("Name"), _T("master"));
				writer.WriteAttribute(_T("LastCommit"), _T("0c9fa5c4509bb5d1043c2d906d88b8507d6b7546"));
				writer.WriteEndElement();

				writer.WriteStartElement(_T("Branch"));
				writer.WriteAttribute(_T("Name"), _T("Custom"));
				writer.WriteAttribute(_T("LastCommit"), _T("662a470c700fc6fa6bc95d0cab8b8ec39bc1cc23"));
				writer.WriteEndElement();
			}
			writer.WriteEndElement();
		}
		writer.WriteEndElement();
	}
	writer.WriteEndElement();

	// TODO: ƒtƒ@ƒCƒ‹”äŠr
}

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
