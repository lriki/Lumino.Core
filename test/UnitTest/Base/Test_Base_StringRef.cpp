#include <TestConfig.h>

class IntegrateTest_Base_StringRef : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static int Func1(const StringRef& str) { return str.GetLength(); }

//-----------------------------------------------------------------------------
TEST_F(IntegrateTest_Base_StringRef, Constructor)
{
	// <Test> 文字列リテラルと String でオーバーロードできること。
	int c = 0;
	c += Func1(_T("abc"));
	c += Func1(String(_T("d")));
	ASSERT_EQ(4, c);
}
