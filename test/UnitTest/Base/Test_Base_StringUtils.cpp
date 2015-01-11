#include <TestConfig.h>

class Test_Base_StringUtils : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, EndsWith)
{
	// 普通に比較
	{
		const TCHAR* str1 = _T("abc def");
		const TCHAR* str2 = _T("def");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_Normal));
	}
	// 完全一致
	{
		const TCHAR* str1 = _T("def");
		const TCHAR* str2 = _T("def");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_Normal));
	}
	// 不一致 (後ろ方向)
	{
		const TCHAR* str1 = _T("def");
		const TCHAR* str2 = _T("de");
		ASSERT_EQ(false, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_Normal));
	}
	// 一致 (前方向方向)
	{
		const TCHAR* str1 = _T("def");
		const TCHAR* str2 = _T("ef");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_Normal));
	}
	// 不一致 (str2が長い)
	{
		const TCHAR* str1 = _T("def");
		const TCHAR* str2 = _T("defghi");
		ASSERT_EQ(false, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_Normal));
	}
	// 空文字どうし
	{
		const TCHAR* str1 = _T("");
		const TCHAR* str2 = _T("");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_Normal));
	}
	// 空文字
	{
		const TCHAR* str1 = _T("a");
		const TCHAR* str2 = _T("");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_Normal));
	}
	// 空文字
	{
		const TCHAR* str1 = _T("");
		const TCHAR* str2 = _T("a");
		ASSERT_EQ(false, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_Normal));
	}

	// 普通に比較 (IgnoreCase)
	{
		const TCHAR* str1 = _T("abc def");
		const TCHAR* str2 = _T("DeF");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_IgnoreCase));
	}
	// 完全一致 (IgnoreCase)
	{
		const TCHAR* str1 = _T("DEF");
		const TCHAR* str2 = _T("def");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_IgnoreCase));
	}
	// 不一致 (後ろ方向) (IgnoreCase)
	{
		const TCHAR* str1 = _T("def");
		const TCHAR* str2 = _T("DE");
		ASSERT_EQ(false, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_IgnoreCase));
	}
	// 一致 (前方向方向) (IgnoreCase)
	{
		const TCHAR* str1 = _T("DEF");
		const TCHAR* str2 = _T("ef");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_IgnoreCase));
	}
	// 不一致 (str2が長い) (IgnoreCase)
	{
		const TCHAR* str1 = _T("def");
		const TCHAR* str2 = _T("defGHI");
		ASSERT_EQ(false, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_IgnoreCase));
	}
	// 空文字どうし (IgnoreCase)
	{
		const TCHAR* str1 = _T("");
		const TCHAR* str2 = _T("");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_IgnoreCase));
	}
	// 空文字 (IgnoreCase)
	{
		const TCHAR* str1 = _T("A");
		const TCHAR* str2 = _T("");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_IgnoreCase));
	}
	// 空文字 (IgnoreCase)
	{
		const TCHAR* str1 = _T("");
		const TCHAR* str2 = _T("A");
		ASSERT_EQ(false, StringUtils::EndsWith(str1, -1, str2, -1, StringComparison_IgnoreCase));
	}
}

