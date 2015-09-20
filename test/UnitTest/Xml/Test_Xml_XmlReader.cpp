#include <TestConfig.h>

class Test_Xml_XmlReader : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};
// TODO: ' ï∂éöóÒÇ…ëŒâûÇµÇƒñ≥Ç¢Ç©Ç‡

//---------------------------------------------------------------------
TEST_F(Test_Xml_XmlReader, User)
{
	StreamReader textReader(LOCALFILE("TestData/ReaderExample1.xml"));
	XmlReader reader(&textReader);	// TODO: XmlFileReader Ç∆Ç©ÅB

	String output;
	while (reader.Read())
	{
		if (reader.GetNodeType() == XmlNodeType::Text) {
			output += String::Format(_T("type:%s value:%s\n"), reader.GetNodeType().ToString().GetCStr(), reader.GetValue().GetCStr());
		}
		else if (reader.GetNodeType() == XmlNodeType::Whitespace) {
			output += String::Format(_T("type:%s\n"), reader.GetNodeType().ToString().GetCStr());
		}
		else {
			output += String::Format(_T("type:%s name:%s\n"), reader.GetNodeType().ToString().GetCStr(), reader.GetName().GetCStr());
		}

		if (reader.MoveToFirstAttribute())
		{
			do {
				output += String::Format(_T("type:%s name:%s value:%s\n"), reader.GetNodeType().ToString().GetCStr(), reader.GetName().GetCStr(), reader.GetValue().GetCStr());

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
	ASSERT_STREQ(result, output);
}


//---------------------------------------------------------------------
// Element ÉmÅ[ÉhÇÃÉeÉXÉg
TEST_F(Test_Xml_XmlReader, Element)
{
	// ÅEãÛóvëf
	{
		String xml = _T("<test />");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());			// ãÛóvëfÇ≈Ç†ÇÈ
		ASSERT_FALSE(reader.Read());					// EOF
	}

	// ÅEEnd Element
	{
		String xml = _T("<test></test>");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// ãÛóvëfÇ≈ÇÕÇ»Ç¢

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::EndElement, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// ãÛóvëfÇ≈ÇÕÇ»Ç¢

		ASSERT_FALSE(reader.Read());					// EOF
	}

	// ÅEÉlÉXÉg
	{
		String xml = _T("<n1><n2/></n1>");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_STREQ(_T("n1"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_STREQ(_T("n2"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::EndElement, reader.GetNodeType());
		ASSERT_STREQ(_T("n1"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// EndElement ÇÕãÛóvëfàµÇ¢Ç≈ÇÕÇ»Ç¢

		ASSERT_FALSE(reader.Read());						// EOF
	}

	// ÅEñºëO
	{
		String xml = _T("<v:test/><b.n1/>");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_STREQ(_T("v:test"), reader.GetName());

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_STREQ(_T("b.n1"), reader.GetName());

		ASSERT_FALSE(reader.Read());					// EOF
	}
}

//---------------------------------------------------------------------
// Text ÉmÅ[ÉhÇÃÉeÉXÉg
TEST_F(Test_Xml_XmlReader, Text)
{
	// ÅEText ÉmÅ[Éh
	// ÅEÉ^ÉOññîˆÇÃãÛîí
	{
		String xml = _T("<test >Text</test >");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// ãÛóvëfÇ≈ÇÕÇ»Ç¢

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_TRUE(reader.GetName().IsEmpty());		// Text ÉmÅ[ÉhÇÃñºëOÇÕãÛ
		ASSERT_STREQ(_T("Text"), reader.GetValue());	// ÉeÉLÉXÉgíl
		ASSERT_FALSE(reader.IsEmptyElement());			// ãÛóvëfÇ≈ÇÕÇ»Ç¢ (Ç∆Ç¢Ç§Ç©óvëfÇ≈ÇÕÇ»Ç¢)

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::EndElement, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// ãÛóvëfÇ≈ÇÕÇ»Ç¢

		ASSERT_FALSE(reader.Read());					// EOF
	}

	// ÅEText ÉmÅ[ÉhëOå„ÇÃãÛîí
	{
		String xml = _T("<test> \nText\r\t</test>");
		XmlReader reader(xml);
		reader.Read();

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_STREQ(_T(" \nText\r\t"), reader.GetValue());	// ÉeÉLÉXÉgíl

		reader.Read();
		ASSERT_FALSE(reader.Read());						// EOF
	}
}

//---------------------------------------------------------------------
// Comment ÉmÅ[ÉhÇÃÉeÉXÉg
TEST_F(Test_Xml_XmlReader, Comment)
{
	{
		String xml = _T("<!-- Comment -->");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Comment, reader.GetNodeType());
		ASSERT_STREQ(_T(" Comment "), reader.GetValue());	// ÉRÉÅÉìÉgÇÃíl

		ASSERT_FALSE(reader.Read());						// EOF
	}
}

//---------------------------------------------------------------------
// Attribute
TEST_F(Test_Xml_XmlReader, Attribute)
{
	// íPàÍëÆê´
	{
		String xml = _T("<test a=\"10\" />");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(1, reader.GetAttributeCount());		// ëÆê´1å¬
		ASSERT_FALSE(reader.MoveToElement());			// à”ñ°ñ≥Çµ

		ASSERT_TRUE(reader.MoveToFirstAttribute());
		ASSERT_EQ(XmlNodeType::Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("a"), reader.GetName());
		ASSERT_STREQ(_T("10"), reader.GetValue());

		ASSERT_TRUE(reader.MoveToElement());	// Element Ç…ñﬂÇÁÇ»Ç¢Ç∆ÅAIsEmptyElement Ç™ã@î\ÇµÇ»Ç¢
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(1, reader.GetAttributeCount());		// ëÆê´1å¬

		ASSERT_FALSE(reader.Read());					// EOF
	}

	//ÅEï°êîëÆê´
	//ÅEñºëOÇ… . Ç∆ :
	{
		String xml = _T("<test v.a=\"10\" v:b=\"ABC\" />");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(2, reader.GetAttributeCount());		// ëÆê´2å¬

		// v.a="10"
		ASSERT_TRUE(reader.MoveToFirstAttribute());
		ASSERT_EQ(XmlNodeType::Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("v.a"), reader.GetName());
		ASSERT_STREQ(_T("10"), reader.GetValue());

		// v:b="ABC"
		ASSERT_TRUE(reader.MoveToNextAttribute());
		ASSERT_EQ(XmlNodeType::Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("v:b"), reader.GetName());
		ASSERT_STREQ(_T("ABC"), reader.GetValue());

		ASSERT_FALSE(reader.MoveToNextAttribute());		// éüÇÕñ≥Ç¢

		ASSERT_FALSE(reader.Read());					// EOF
	}
}

//---------------------------------------------------------------------
// ó\ñÒçœÇ› Entity
TEST_F(Test_Xml_XmlReader, ReservedEntity)
{
	//ÅEëSÇƒÇÃó\ñÒçœÇ› Entity ÇÃìWäJ
	{
		String xml = _T("<root><ch>&lt;<ch/><ch>&gt;<ch/><ch>&amp;<ch/><ch>&apos;<ch/><ch>&quot;<ch/><root/>");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());	// <root>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &lt;
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_STREQ(_T("<"), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &gt;
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_STREQ(_T(">"), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &amp;
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_STREQ(_T("&"), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &apos;
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_STREQ(_T("'"), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <ch>

		ASSERT_TRUE(reader.Read());	// &quot;
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_STREQ(_T("\""), reader.GetValue());

		ASSERT_TRUE(reader.Read());	// <ch/>
		ASSERT_TRUE(reader.Read());	// <root/>

		ASSERT_FALSE(reader.Read());// EOF
	}

	//ÅEëÆê´
	//ÅEñ¢íËã`Entity
	//ÅEëOå„ÇÃï∂éöÇ∆ãÛîí
	{
		String xml = _T("<test a=\"&lt;\" b=\"&AAAA;\"  c=\"A&lt; \" />");
		XmlReader reader(xml);
		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType::Element, reader.GetNodeType());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(3, reader.GetAttributeCount());		// ëÆê´1å¬

		ASSERT_TRUE(reader.MoveToFirstAttribute());
		ASSERT_EQ(XmlNodeType::Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("a"), reader.GetName());
		ASSERT_STREQ(_T("<"), reader.GetValue());

		ASSERT_TRUE(reader.MoveToNextAttribute());
		ASSERT_EQ(XmlNodeType::Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("b"), reader.GetName());
		ASSERT_STREQ(_T("&AAAA;"), reader.GetValue());	// îFéØÇ≈Ç´Ç»Ç¢ Entity ÇÕÉGÉâÅ[Ç…ÇπÇ∏ÅAÉeÉLÉXÉgÇ∆ÇµÇƒï‘Ç∑

		ASSERT_TRUE(reader.MoveToNextAttribute());
		ASSERT_EQ(XmlNodeType::Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("c"), reader.GetName());
		ASSERT_STREQ(_T("A< "), reader.GetValue());

		ASSERT_FALSE(reader.Read());					// EOF
	}
}

//---------------------------------------------------------------------
// Entity éQè∆
TEST_F(Test_Xml_XmlReader, EntityReference)
{
	// ÅEÉmÅ[ÉhëOå„ÇÃÉeÉLÉXÉgÇ∆ãÛîí
	{
		String xml = _T("<a> A&book; </a>");
		XmlReader reader(xml);
		ASSERT_TRUE(reader.Read());		// <a>

		ASSERT_TRUE(reader.Read());		// " A"
		ASSERT_EQ(XmlNodeType::Text, reader.GetNodeType());
		ASSERT_STREQ(_T(" A"), reader.GetValue());

		ASSERT_TRUE(reader.Read());		// "&book;"
		ASSERT_EQ(XmlNodeType::EntityReference, reader.GetNodeType());
		ASSERT_STREQ(_T("book"), reader.GetName());
		ASSERT_TRUE(reader.GetValue().IsEmpty());	// ílÇÕãÛ

		ASSERT_TRUE(reader.Read());		// " "
		ASSERT_EQ(XmlNodeType::Whitespace, reader.GetNodeType());
		ASSERT_STREQ(_T(" "), reader.GetValue());

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

		Array<XmlNodeType> types;
		while (reader.Read())
		{
			types.Add(reader.GetNodeType());
			if (reader.IsStartElement("test"))
			{
				types.Add(reader.GetNodeType());
				ASSERT_STREQ(_T("  AAA  "), reader.ReadString());
				types.Add(reader.GetNodeType());	// EndElement
			}
		}
		ASSERT_EQ(5, types.GetCount());
		ASSERT_EQ(XmlNodeType::Element, types[0]);
		ASSERT_EQ(XmlNodeType::Whitespace, types[1]);
		ASSERT_EQ(XmlNodeType::Element, types[2]);
		ASSERT_EQ(XmlNodeType::EndElement, types[3]);
		ASSERT_EQ(XmlNodeType::Whitespace, types[4]);
		//ASSERT_EQ(XmlNodeType::EndElement, types[5]);	ç≈å„ÇÃÇÕ IsStartElement Ç≈è¡îÔÇ≥ÇÍÇÈÇÃÇ≈èoÇƒÇ±Ç»Ç¢
	}
}
