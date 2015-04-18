#include <TestConfig.h>

class Test_IO_ASync : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static int g_count = 0;

class ASyncTest1
	: public AsyncIOTask
{
public:
	virtual void Execute()
	{
		++g_count;
	}
};

//-----------------------------------------------------------------------------
TEST_F(Test_IO_ASync, Basic)
{
	g_count = 0;
	ASyncTest1 test1;
	FileManager::GetInstance().RequestASyncTask(&test1);
	FileManager::GetInstance().WaitForAllASyncTask();

	ASSERT_EQ(AsyncIOState_Completed, test1.GetAsyncIOState());
	ASSERT_EQ(NULL, test1.GetException());
	ASSERT_EQ(1, g_count);
}
