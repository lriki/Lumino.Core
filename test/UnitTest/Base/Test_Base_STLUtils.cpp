#include <TestConfig.h>

class Test_Base_STLUtils : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_Base_STLUtils, Foreach)
{
	// 単純なイテレート
	{
		Array<String> ary1;
		ary1.Add(_T("a"));
		ary1.Add(_T("b"));
		ary1.Add(_T("c"));

		String result;
		LN_FOREACH(String& str, ary1)
		{
			result += str;
		}
		ASSERT_EQ(_T("abc"), result);
	}
	// 内容の変更
	{
		Array<String> ary1;
		ary1.Add(_T("a"));
		ary1.Add(_T("b"));

		ary1.Add(_T("c"));
		String result;
		LN_FOREACH(String& str, ary1)
		{
			str = _T("A");
		}
		ASSERT_EQ(_T("A"), ary1[0]);
		ASSERT_EQ(_T("A"), ary1[1]);
		ASSERT_EQ(_T("A"), ary1[2]);
	}
	// break
	{
		Array<String> ary1;
		ary1.Add(_T("a"));
		ary1.Add(_T("b"));
		ary1.Add(_T("c"));

		String result;
		LN_FOREACH(String& str, ary1)
		{
			result += str;
			break;
		}
		ASSERT_EQ(_T("a"), result);
	}
	// continue
	{
		Array<String> ary1;
		ary1.Add(_T("a"));
		ary1.Add(_T("b"));
		ary1.Add(_T("c"));

		String result;
		LN_FOREACH(String& str, ary1)
		{
			if (result.GetLength() == 1) {
				result += _T("|");
				continue;
			}
			result += str;
		}
		ASSERT_EQ(_T("a|c"), result);
	}
	// const
	{
		Array<String> ary1;
		ary1.Add(_T("a"));
		ary1.Add(_T("b"));
		ary1.Add(_T("c"));

		const Array<String>& ary2 = ary1;

		String result;
		LN_FOREACH(const String& str, ary2)
		{
			result += str;
		}
		ASSERT_EQ(_T("abc"), result);
	}
	// 2重ループ

	{
		Array<String> ary1;
		ary1.Add(_T("a"));
		ary1.Add(_T("b"));
		ary1.Add(_T("c"));
		Array<String> ary2;
		ary2.Add(_T("d"));
		ary2.Add(_T("e"));
		ary2.Add(_T("f"));
		String result;

		LN_FOREACH(String& str1, ary1)
		{
			result += str1;
			LN_FOREACH(String& str2, ary2)
			{
				result += str2;
			}
		}
		ASSERT_EQ(_T("adefbdefcdef"), result);
	}
}

