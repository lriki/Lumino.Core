#include <TestConfig.h>

class Test_IO_TextWriter : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_IO_TextWriter, Basic)
{
	StreamWriter writer(LOCALFILE("Test1.txt"), Text::Encoding::GetUTF8Encoding());
	writer.WriteInt32(32);
}
