#include <TestConfig.h>

class Test_IO_FileUtils : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_IO_FileUtils, Exists)
{
	//LN_THROW(0, IOException, "test");
	ASSERT_TRUE(FileUtils::Exists(LN_TEST_GET_FILE_PATH("TestData/test1.txt")));
	// ディレクトリは false
	ASSERT_FALSE(FileUtils::Exists(LN_TEST_GET_FILE_PATH("TestData")));
}

