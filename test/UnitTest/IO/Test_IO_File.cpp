#include <TestConfig.h>

class Test_IO_File : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};
//------------------------------------------------------------------------------
TEST_F(Test_IO_File, Open)
{
	File file1(LOCALFILE("TestData/test1.txt"));
	file1.Open(FileOpenMode::Read);
}
