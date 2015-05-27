#include <TestConfig.h>
using namespace Text;

class Test_Xml_XmlReader : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
// Element �m�[�h�̃e�X�g
TEST_F(Test_Xml_XmlReader, Element)
{
	// �E��v�f
	{
		String xml = _T("<test />");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());			// ��v�f�ł���
		ASSERT_FALSE(reader.Read());					// EOF
	}

	// �EEnd Element
	{
		String xml = _T("<test></test>");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// ��v�f�ł͂Ȃ�

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_EndElement, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// ��v�f�ł͂Ȃ�

		ASSERT_FALSE(reader.Read());					// EOF
	}

	// �E�l�X�g
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
		ASSERT_FALSE(reader.IsEmptyElement());			// EndElement �͋�v�f�����ł͂Ȃ�

		ASSERT_FALSE(reader.Read());						// EOF
	}

	// �E���O
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
// Text �m�[�h�̃e�X�g
TEST_F(Test_Xml_XmlReader, Text)
{
	// �EText �m�[�h
	// �E�^�O�����̋�
	{
		String xml = _T("<test >Text</test >");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// ��v�f�ł͂Ȃ�

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Text, reader.GetNodeType());
		ASSERT_TRUE(reader.GetName().IsEmpty());		// Text �m�[�h�̖��O�͋�
		ASSERT_STREQ(_T("Text"), reader.GetValue());	// �e�L�X�g�l
		ASSERT_FALSE(reader.IsEmptyElement());			// ��v�f�ł͂Ȃ� (�Ƃ������v�f�ł͂Ȃ�)

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_EndElement, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_FALSE(reader.IsEmptyElement());			// ��v�f�ł͂Ȃ�

		ASSERT_FALSE(reader.Read());					// EOF
	}

	// �EText �m�[�h�O��̋�
	{
		String xml = _T("<test> \nText\r\t</test>");
		XmlReader reader(xml);
		reader.Read();

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Text, reader.GetNodeType());
		ASSERT_STREQ(_T(" \nText\r\t"), reader.GetValue());	// �e�L�X�g�l

		reader.Read();
		ASSERT_FALSE(reader.Read());						// EOF
	}
}

//---------------------------------------------------------------------
// Comment �m�[�h�̃e�X�g
TEST_F(Test_Xml_XmlReader, Comment)
{
	{
		String xml = _T("<!-- Comment -->");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Comment, reader.GetNodeType());
		ASSERT_STREQ(_T(" Comment "), reader.GetValue());	// �R�����g�̒l

		ASSERT_FALSE(reader.Read());						// EOF
	}
}



