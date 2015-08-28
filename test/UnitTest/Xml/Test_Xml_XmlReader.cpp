#include <TestConfig.h>
using namespace Text;

class Test_Xml_XmlReader : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

// TODO: ' ������ɑΉ����Ė�������

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

//---------------------------------------------------------------------
// Attribute
TEST_F(Test_Xml_XmlReader, Attribute)
{
	// �P�ꑮ��
	{
		String xml = _T("<test a=\"10\" />");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(1, reader.GetAttributeCount());		// ����1��
		ASSERT_FALSE(reader.MoveToElement());			// �Ӗ�����

		ASSERT_TRUE(reader.MoveToFirstAttribute());
		ASSERT_EQ(XmlNodeType_Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("a"), reader.GetName());
		ASSERT_STREQ(_T("10"), reader.GetValue());

		ASSERT_TRUE(reader.MoveToElement());	// Element �ɖ߂�Ȃ��ƁAIsEmptyElement ���@�\���Ȃ�
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_STREQ(_T("test"), reader.GetName());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(1, reader.GetAttributeCount());		// ����1��

		ASSERT_FALSE(reader.Read());					// EOF
	}

	//�E��������
	//�E���O�� . �� :
	{
		String xml = _T("<test v.a=\"10\" v:b=\"ABC\" />");
		XmlReader reader(xml);

		ASSERT_TRUE(reader.Read());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(2, reader.GetAttributeCount());		// ����2��

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

		ASSERT_FALSE(reader.MoveToNextAttribute());		// ���͖���

		ASSERT_FALSE(reader.Read());					// EOF
	}
}

//---------------------------------------------------------------------
// �\��ς� Entity
TEST_F(Test_Xml_XmlReader, ReservedEntity)
{
	//�E�S�Ă̗\��ς� Entity �̓W�J
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

	//�E����
	//�E����`Entity
	//�E�O��̕����Ƌ�
	{
		String xml = _T("<test a=\"&lt;\" b=\"&AAAA;\"  c=\"A&lt; \" />");
		XmlReader reader(xml);
		ASSERT_TRUE(reader.Read());
		ASSERT_EQ(XmlNodeType_Element, reader.GetNodeType());
		ASSERT_TRUE(reader.IsEmptyElement());
		ASSERT_EQ(3, reader.GetAttributeCount());		// ����1��

		ASSERT_TRUE(reader.MoveToFirstAttribute());
		ASSERT_EQ(XmlNodeType_Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("a"), reader.GetName());
		ASSERT_STREQ(_T("<"), reader.GetValue());

		ASSERT_TRUE(reader.MoveToNextAttribute());
		ASSERT_EQ(XmlNodeType_Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("b"), reader.GetName());
		ASSERT_STREQ(_T("&AAAA;"), reader.GetValue());	// �F���ł��Ȃ� Entity �̓G���[�ɂ����A�e�L�X�g�Ƃ��ĕԂ�

		ASSERT_TRUE(reader.MoveToNextAttribute());
		ASSERT_EQ(XmlNodeType_Attribute, reader.GetNodeType());
		ASSERT_STREQ(_T("c"), reader.GetName());
		ASSERT_STREQ(_T("A< "), reader.GetValue());

		ASSERT_FALSE(reader.Read());					// EOF
	}
}

//---------------------------------------------------------------------
// Entity �Q��
TEST_F(Test_Xml_XmlReader, EntityReference)
{
	// �E�m�[�h�O��̃e�L�X�g�Ƌ�
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
		ASSERT_TRUE(reader.GetValue().IsEmpty());	// �l�͋�

		ASSERT_TRUE(reader.Read());		// " "
		ASSERT_EQ(XmlNodeType_Whitespace, reader.GetNodeType());
		ASSERT_STREQ(_T(" "), reader.GetValue());

		ASSERT_TRUE(reader.Read());		// </a>

		ASSERT_FALSE(reader.Read());	// EOF
	}
}
