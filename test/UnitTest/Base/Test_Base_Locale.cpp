#include <TestConfig.h>

static int g_Value = 0;

class Test_Base_Locale : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}

};

//---------------------------------------------------------------------
TEST_F(Test_Base_Locale, Constructor)
{
	Locale locale;
}
