#include <TestConfig.h>
#include <Lumino/Base/Formatter.h>

class Test_Base_Formatter : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

#if 0
//---------------------------------------------------------------------
TEST_F(Test_Base_Formatter, Basic)
{
	ASSERT_EQ(_T(""), String::Format(_T("")));
	ASSERT_EQ(_T("a"), String::Format(_T("a")));
	ASSERT_EQ(_T("a"), String::Format(_T("{0}"), _T("a")));
	ASSERT_EQ(_T("ab"), String::Format(_T("{0}{1}"), _T("a"), _T("b")));
	ASSERT_EQ(_T("ba"), String::Format(_T("{1}{0}"), _T("a"), _T("b")));
	ASSERT_EQ(_T("aba"), String::Format(_T("{0}{1}{0}"), _T("a"), _T("b")));
	ASSERT_EQ(_T("{0}"), String::Format(_T("{{0}}"), _T("a"), _T("b")));

	ASSERT_EQ(_T("   a"), String::Format(_T("{0,4}"), _T("a")));
	ASSERT_EQ(_T("  ab"), String::Format(_T("{0,4}"), _T("ab")));
	ASSERT_EQ(_T("a   "), String::Format(_T("{0,-4}"), _T("a")));
	ASSERT_EQ(_T("ab  "), String::Format(_T("{0,-4}"), _T("ab")));
	ASSERT_EQ(_T("   aa   ab  "), String::Format(_T("{0,4}{0,-4}{1,-4}"), _T("a"), _T("ab")));
	ASSERT_EQ(_T("abcd"), String::Format(_T("{0,3}"), _T("abcd")));
	ASSERT_EQ(_T("abcd"), String::Format(_T("{0,4}"), _T("abcd")));
	ASSERT_EQ(_T(" abcd"), String::Format(_T("{0,5}"), _T("abcd")));
	ASSERT_EQ(_T("abcd"), String::Format(_T("{0,-3}"), _T("abcd")));
	ASSERT_EQ(_T("abcd"), String::Format(_T("{0,-4}"), _T("abcd")));
	ASSERT_EQ(_T("abcd "), String::Format(_T("{0,-5}"), _T("abcd")));

	//ASSERT_EQ(_T("2147483647"), String::Format(_T("{0}"), INT32_MAX));
}

//---------------------------------------------------------------------
TEST_F(Test_Base_Formatter, Illigal)
{
	ASSERT_THROW(String::Format(_T("{0}")), InvalidFormatException);
	ASSERT_THROW(String::Format(_T("{1}"), _T("a")), InvalidFormatException);
	ASSERT_THROW(String::Format(_T("{")), InvalidFormatException);
	ASSERT_THROW(String::Format(_T("}")), InvalidFormatException);
	ASSERT_THROW(String::Format(_T("{a")), InvalidFormatException);
	ASSERT_THROW(String::Format(_T("a}")), InvalidFormatException);
	ASSERT_THROW(String::Format(_T("{0,a}"), _T("a")), InvalidFormatException);
	ASSERT_THROW(String::Format(_T("{0,-a}"), _T("a")), InvalidFormatException);
	ASSERT_THROW(String::Format(_T("{0, "), _T("a")), InvalidFormatException);
	ASSERT_THROW(String::Format(_T("{0:"), _T("a")), InvalidFormatException);
	ASSERT_THROW(String::Format(_T("{0:D"), _T("a")), InvalidFormatException);
	ASSERT_THROW(String::Format(_T("{0:D "), _T("a")), InvalidFormatException);
}
#endif
