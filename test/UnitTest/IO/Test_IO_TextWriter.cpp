﻿#include <TestConfig.h>

class Test_IO_TextWriter : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_IO_TextWriter, Basic)
{
	{
		StreamWriter writer(TEMPFILE("Test1.txt"), Encoding::GetUTF8Encoding());
		writer.WriteInt32(32);
	}

	String str = FileSystem::ReadAllText(TEMPFILE("Test1.txt"));
	ASSERT_EQ(_T("32"), str);
}
