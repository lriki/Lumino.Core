#include <TestConfig.h>

class Test_Base_StringUtils : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, StrLen)
{
	UTF8 str_u8[] = { 'A', 'B', 'C', 0x00 };
	UTF16 str_u16[] = { 'A', 'B', 'C', 0x00 };
	UTF32 str_u32[] = { 'A', 'B', 'C', 0x00 };

	ASSERT_EQ(3, StringUtils::StrLen(str_u8));
	ASSERT_EQ(3, StringUtils::StrLen(str_u16));
	ASSERT_EQ(3, StringUtils::StrLen(str_u32));
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, ToUpper)
{
	{
		UTF8 ch_u8 = 'a';
		UTF16 ch_u16 = 'a';
		UTF32 ch_u32 = 'a';

		ASSERT_EQ(0x41, StringUtils::ToUpper(ch_u8));
		ASSERT_EQ(0x41, StringUtils::ToUpper(ch_u16));
		ASSERT_EQ(0x41, StringUtils::ToUpper(ch_u32));
	}
	{
		UTF8 ch_u8 = '+';
		UTF16 ch_u16 = '+';
		UTF32 ch_u32 = '+';

		ASSERT_EQ(0x2B, StringUtils::ToUpper(ch_u8));
		ASSERT_EQ(0x2B, StringUtils::ToUpper(ch_u16));
		ASSERT_EQ(0x2B, StringUtils::ToUpper(ch_u32));
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, IsSpace)
{
	{
		UTF8 str_u8[] = { '\t', '\n', '\v', '\f', '\r', ' ' };
		UTF16 str_u16[] = { '\t', '\n', '\v', '\f', '\r', ' ' };
		UTF32 str_u32[] = { '\t', '\n', '\v', '\f', '\r', ' ' };
		for (int i = 0; i < LN_ARRAY_SIZE_OF(str_u8); i++)
		{
			ASSERT_TRUE(StringUtils::IsSpace(str_u8[i]));
			ASSERT_TRUE(StringUtils::IsSpace(str_u16[i]));
			ASSERT_TRUE(StringUtils::IsSpace(str_u32[i]));
		}
	}
	{
		UTF8 str_u8[] = { 'A' };
		UTF16 str_u16[] = { 'A' };
		UTF32 str_u32[] = { 'A' };
		for (int i = 0; i < LN_ARRAY_SIZE_OF(str_u8); i++)
		{
			ASSERT_FALSE(StringUtils::IsSpace(str_u8[i]));
			ASSERT_FALSE(StringUtils::IsSpace(str_u16[i]));
			ASSERT_FALSE(StringUtils::IsSpace(str_u32[i]));
		}
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_StringUtils, EndsWith)
{
	// 普通に比較
	{
		const TCHAR* str1 = _T("abc def");
		const TCHAR* str2 = _T("def");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseSensitive));
	}
	// 完全一致
	{
		const TCHAR* str1 = _T("def");
		const TCHAR* str2 = _T("def");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseSensitive));
	}
	// 不一致 (後ろ方向)
	{
		const TCHAR* str1 = _T("def");
		const TCHAR* str2 = _T("de");
		ASSERT_EQ(false, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseSensitive));
	}
	// 一致 (前方向方向)
	{
		const TCHAR* str1 = _T("def");
		const TCHAR* str2 = _T("ef");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseSensitive));
	}
	// 不一致 (str2が長い)
	{
		const TCHAR* str1 = _T("def");
		const TCHAR* str2 = _T("defghi");
		ASSERT_EQ(false, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseSensitive));
	}
	// 空文字どうし
	{
		const TCHAR* str1 = _T("");
		const TCHAR* str2 = _T("");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseSensitive));
	}
	// 空文字
	{
		const TCHAR* str1 = _T("a");
		const TCHAR* str2 = _T("");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseSensitive));
	}
	// 空文字
	{
		const TCHAR* str1 = _T("");
		const TCHAR* str2 = _T("a");
		ASSERT_EQ(false, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseSensitive));
	}

	// 普通に比較 (IgnoreCase)
	{
		const TCHAR* str1 = _T("abc def");
		const TCHAR* str2 = _T("DeF");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseInsensitive));
	}
	// 完全一致 (IgnoreCase)
	{
		const TCHAR* str1 = _T("DEF");
		const TCHAR* str2 = _T("def");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseInsensitive));
	}
	// 不一致 (後ろ方向) (IgnoreCase)
	{
		const TCHAR* str1 = _T("def");
		const TCHAR* str2 = _T("DE");
		ASSERT_EQ(false, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseInsensitive));
	}
	// 一致 (前方向方向) (IgnoreCase)
	{
		const TCHAR* str1 = _T("DEF");
		const TCHAR* str2 = _T("ef");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseInsensitive));
	}
	// 不一致 (str2が長い) (IgnoreCase)
	{
		const TCHAR* str1 = _T("def");
		const TCHAR* str2 = _T("defGHI");
		ASSERT_EQ(false, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseInsensitive));
	}
	// 空文字どうし (IgnoreCase)
	{
		const TCHAR* str1 = _T("");
		const TCHAR* str2 = _T("");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseInsensitive));
	}
	// 空文字 (IgnoreCase)
	{
		const TCHAR* str1 = _T("A");
		const TCHAR* str2 = _T("");
		ASSERT_EQ(true, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseInsensitive));
	}
	// 空文字 (IgnoreCase)
	{
		const TCHAR* str1 = _T("");
		const TCHAR* str2 = _T("A");
		ASSERT_EQ(false, StringUtils::EndsWith(str1, -1, str2, -1, CaseSensitivity_CaseInsensitive));
	}
}

