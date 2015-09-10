#include <TestConfig.h>

class Test_Json_JsonDocument : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonDocument, Basic)
{
	String str = FileSystem::ReadAllText(LOCALFILE("TestData/ReaderTest1.txt"), Encoding::GetSystemMultiByteEncoding());
	Json::JsonDocument doc;

	ElapsedTimer t;
	t.Start();
	doc.Parse(str);
	

	printf("%d", t.GetElapsedTime());
}



