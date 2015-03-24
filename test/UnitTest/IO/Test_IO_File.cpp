#include <TestConfig.h>

class Test_IO_File : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};
//-----------------------------------------------------------------------------
TEST_F(Test_IO_File, Open)
{
	File file1(LN_TEST_GET_FILE_PATH("TestData/test1.txt"));
	file1.Open(FileMode_Open, FileAccess_Read);
}
