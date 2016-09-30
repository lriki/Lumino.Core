#include <TestConfig.h>
#include <Lumino/IO/StreamWriter.h>
#include <Lumino/IO/StringWriter.h>
#include <Lumino/Xml/XmlWriter.h>

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

	// TODO: ÉtÉ@ÉCÉãî‰är
}

uint16_t jis_to_sjis(uint8_t c1, uint8_t c2)
{
	if (c1 % 2) {
		c1 = ((c1 + 1) / 2) + 0x70;
		c2 = c2 + 0x1f;
	}
	else {
		c1 = (c1 / 2) + 0x70;
		c2 = c2 + 0x7d;
	}
	if (c1 >= 0xa0) { c1 = c1 + 0x40; }
	if (c2 >= 0x7f) { c2 = c2 + 1; }

	return (c1 << 8) | c2;
}

//---------------------------------------------------------------------
TEST_F(Test_Xml_XmlWriter, UnitTest)
{
	//uint16_t r = jis_to_sjis(0x74, 0x26);

//	// <Unit> WriteStartDocument
//	{
//		StringWriter strWriter;
//		XmlWriter xmlWriter(&strWriter);
//		xmlWriter.WriteStartDocument();
//		xmlWriter.WriteEndDocument();
//#ifdef _WIN32
//		ASSERT_EQ(_T("<?xml version=\"1.0\" encoding=\"UTF-16\"?>"), strWriter.ToString());
//#else
//		ASSERT_EQ(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"), strWriter.ToString());
//#endif
//		xmlWriter.WriteStartDocument();
//		ASSERT_THROW(xmlWriter.WriteStartDocument(), InvalidOperationException);
//	}

	// <Unit> WriteStartElement
	{
		StringWriter strWriter;
		XmlWriter xmlWriter(&strWriter);
		xmlWriter.WriteStartElement(_T("test"));
		ASSERT_EQ(_T("<test"), strWriter.ToString());
	}

	// <Unit> WriteEndElement
	{
		StringWriter strWriter;
		XmlWriter xmlWriter(&strWriter);
		xmlWriter.WriteStartElement(_T("test"));
		xmlWriter.WriteEndElement();
		ASSERT_EQ(_T("<test />"), strWriter.ToString());
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
		ASSERT_EQ(_T("<test a1=\"v1\" a2=\"&amp;&quot;\" a3=\"v&lt;v\" a4=\"&gt;v&gt;\" a5=\"&gt;&gt;v\" a6=\"v&lt;&lt;\" />"), strWriter.ToString());
	}

	// <Unit> WriteComment
	{
		StringWriter strWriter;
		strWriter.SetNewLine(_T("\n"));
		XmlWriter xmlWriter(&strWriter);
		xmlWriter.WriteStartElement(_T("test"));
		xmlWriter.WriteComment(_T("comment1"));
		ASSERT_THROW(xmlWriter.WriteComment(_T("comment2--")), ArgumentException);
		xmlWriter.WriteComment(_T("comment3"));
		xmlWriter.WriteEndElement();
		ASSERT_EQ(_T("<test>\n  <!--comment1-->\n  <!--comment3-->\n</test>"), strWriter.ToString());
	}

	// <Unit> WriteCData
	{
		StringWriter strWriter;
		XmlWriter xmlWriter(&strWriter);
		xmlWriter.WriteStartElement(_T("test"));
		xmlWriter.WriteCData(_T("AAA && BBB"));
		ASSERT_THROW(xmlWriter.WriteCData(_T("XX]]>")), ArgumentException);
		xmlWriter.WriteCData(_T("CCC\nDDD"));
		xmlWriter.WriteEndElement();
		ASSERT_EQ(_T("<test><![CDATA[AAA && BBB]]><![CDATA[CCC\nDDD]]></test>"), strWriter.ToString());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Xml_XmlWriter, SystemTest)
{
	// <Test> XML êÈåæÇ∆óvëf
	{
		XmlStringWriter writer;
		writer.SetNewLine(_T("\n"));
		writer.WriteStartDocument();
		writer.WriteStartElement(_T("EnvData"));
		writer.WriteElementString(_T("ToolPath"), _T("path"));
		writer.WriteEndElement();
		writer.WriteEndDocument();

		String str = writer.ToString();
		String exp =
			_T("<?xml version=\"1.0\" encoding=\"_ENC_\"?>\n")
			_T("<EnvData>\n")
			_T("  <ToolPath>path</ToolPath>\n")
			_T("</EnvData>");
		exp = exp.Replace(_T("_ENC_"), Encoding::GetTCharEncoding()->GetName());
		ASSERT_EQ(exp, str);

	}
}
