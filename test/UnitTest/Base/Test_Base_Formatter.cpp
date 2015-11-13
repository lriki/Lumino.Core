#include <TestConfig.h>
#include <Lumino/Base/Formatter.h>

class Test_Base_Formatter : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Base_Formatter, Constructor)
{

	Formatter<TCHAR>::Format(_T("{0}"), String(_T("aa")));
}
