#include <TestConfig.h>

class Test_Base_Exception : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Base_Exception, Assertion)
{
	// LN_VERIFY OK
	{
		int* p = (int*)1;
		if (LN_VERIFY(p != NULL)) {
			// ここを通る
		}
		else {
			FAIL();	// ここにきてはダメ
		}
	}

	// LN_VERIFY NG
	{
		int* p = NULL;
		try
		{
			if (LN_VERIFY(p != NULL)) {
			}
			FAIL();	// ここにきてはダメ
		}
		catch (Exception&) {
			// ここを通る
		}
	}

	// LN_VERIFY_ASSERT OK
	{
		int* p = (int*)1;
		if (LN_VERIFY_ASSERT(p != NULL)) {
			FAIL();	// ここにきてはダメ
		}
		else {
			// ここを通る
		}
	}

	// LN_VERIFY_ASSERT NG
	{
		int* p = NULL;
		try
		{
			if (LN_VERIFY_ASSERT(p != NULL)) {
				// ここを通る条件だが、例外なので来ない
			}
			FAIL();	// ここにきてはダメ
		}
		catch (Exception&) {
			// ここを通る
		}
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Base_Exception, Basic)
{



	// char 可変長 message
	try
	{
		LN_THROW(0, IOException, "test%d", 1);
	}
	catch (IOException& e)
	{
		ASSERT_EQ(0, _tcscmp(_T("test1"), e.GetMessage()));
	}
	// wchar_t 可変長 message
	try
	{
#ifdef LN_OS_WIN32
		LN_THROW(0, IOException, L"test%s", L"test");
#else	// 現在 Unix 系の vswprintf は動作保証していないので別の例外になる。
		try {
			LN_THROW(0, IOException, L"test%s", L"test");
		}
		catch (NotImplementedException& e) {
		}
#endif
	}
	catch (IOException& e)
	{
		ASSERT_EQ(0, _tcscmp(_T("testtest"), e.GetMessage()));
	}
}
