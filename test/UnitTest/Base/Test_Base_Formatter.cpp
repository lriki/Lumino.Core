#include <TestConfig.h>
#include <Lumino/Base/Formatter.h>

namespace ln
{

	//template</*typename TChar, */std::size_t N>
	//struct Formatter<TCHAR, std::false_type, wchar_t[N]>
	//{
	//	static GenericString<TCHAR> Format(const Locale& locale, const GenericStringRef<TCHAR>& format, const GenericStringRef<TCHAR>& formatParam,  const wchar_t value[N])
	//	{
	//		return GenericString<TCHAR>(value);
	//	}
	//};
}

class Test_Base_Formatter : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};


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

	// <Test> インデント
	{
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
	}
	// <Test> ロケール
	{
		//Locale lc(_T("fr"));
		//ASSERT_EQ(_T("10"), String::Format(lc, _T("{0}"), 10));
		//ASSERT_EQ(_T("2147483647"), String::Format(_T("{0}"), INT32_MAX));
	}

	// <Test> char/wchar_t
	{
		ASSERT_EQ(_T("a"), String::Format(_T("{0}"), 'a'));
		ASSERT_EQ(_T("a"), String::Format(_T("{0}"), L'a'));
	}
	// <Test> int16_t
	{
		ASSERT_EQ(_T("42"), String::Format(_T("{0}"), (int16_t)42));
		ASSERT_EQ(_T("32767"), String::Format(_T("{0}"), INT16_MAX));
		ASSERT_EQ(_T("-32768"), String::Format(_T("{0}"), INT16_MIN));
	}
	// <Test> uint16_t
	{
		ASSERT_EQ(_T("42"), String::Format(_T("{0}"), (uint16_t)42));
		ASSERT_EQ(_T("ffff"), String::Format(_T("{0:x}"), UINT16_MAX));
	}
	// <Test> int32_t
	{
		ASSERT_EQ(_T("42"), String::Format(_T("{0}"), (int32_t)42));
		ASSERT_EQ(_T("2147483647"), String::Format(_T("{0}"), INT32_MAX));
		ASSERT_EQ(_T("-2147483648"), String::Format(_T("{0}"), INT32_MIN));
	}
	// <Test> uint32_t
	{
		ASSERT_EQ(_T("42"), String::Format(_T("{0}"), (uint32_t)42));
		ASSERT_EQ(_T("ffffffff"), String::Format(_T("{0:x}"), UINT32_MAX));
	}
	// <Test> int64_t
	{
		ASSERT_EQ(_T("42"), String::Format(_T("{0}"), (int64_t)42));
		ASSERT_EQ(_T("9223372036854775807"), String::Format(_T("{0}"), INT64_MAX));
		ASSERT_EQ(_T("-9223372036854775808"), String::Format(_T("{0}"), INT64_MIN));
	}
	// <Test> uint64_t
	{
		ASSERT_EQ(_T("42"), String::Format(_T("{0}"), (uint64_t)42));
		ASSERT_EQ(_T("ffffffffffffffff"), String::Format(_T("{0:x}"), UINT64_MAX));
	}
	// <Test> float
	{
		ASSERT_EQ(_T("1.5"), String::Format(_T("{0}"), 1.5f));
	}
	// <Test> double
	{
		ASSERT_EQ(_T("1.5"), String::Format(_T("{0}"), 1.5));
	}

	// <Test> hex
	{
		ASSERT_EQ(_T("F"), String::Format(_T("{0:X}"), 15));
		ASSERT_EQ(_T("f"), String::Format(_T("{0:x}"), 15));
	}
	// <Test> exp
	{
#if _MSC_VER >= 1900	// VS2015 でちょっと変わった？
		ASSERT_EQ(_T("1.005000e+02"), String::Format(_T("{0:e}"), 100.5));
		ASSERT_EQ(_T("1.005000E+02"), String::Format(_T("{0:E}"), 100.5));
		ASSERT_EQ(_T("1.01e+02"), String::Format(_T("{0:e2}"), 100.5));
		ASSERT_EQ(_T("1.00500000E+02"), String::Format(_T("{0:E8}"), 100.5));
#else
		ASSERT_EQ(_T("1.005000e+002"), String::Format(_T("{0:e}"), 100.5));
		ASSERT_EQ(_T("1.005000E+002"), String::Format(_T("{0:E}"), 100.5));
		ASSERT_EQ(_T("1.01e+002"), String::Format(_T("{0:e2}"), 100.5));
		ASSERT_EQ(_T("1.00500000E+002"), String::Format(_T("{0:E8}"), 100.5));
#endif
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Base_Formatter, Illigal)
{
	// <Test> 引数不足の確認
	{
		ASSERT_THROW(String::Format(_T("{0}")), InvalidFormatException);
		ASSERT_THROW(String::Format(_T("{1}"), _T("a")), InvalidFormatException);
	}
	// <Test> 文法エラーの確認
	{
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
}

