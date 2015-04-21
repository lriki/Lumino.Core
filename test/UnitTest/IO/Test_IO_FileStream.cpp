﻿#include <TestConfig.h>

class Test_IO_FileStream : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};
//-----------------------------------------------------------------------------
TEST_F(Test_IO_FileStream, Open)
{
	// Write
	{
		FileStream file1(LOCALFILE("TestData/Test_IO_FileStream1.txt"), FileOpenMode_Write);
		ASSERT_EQ(0, file1.GetLength());	// 空ファイルになる
		file1.Write("A", 1);
	}
	// Write | Truncate (↑と同じ動作)
	{
		FileStream file1(LOCALFILE("TestData/Test_IO_FileStream1.txt"), FileOpenMode_Write | FileOpenMode_Truncate);
		ASSERT_EQ(0, file1.GetLength());	// 空ファイルになる
		file1.Write("B", 1);
	}
	// Write | Append
	{
		FileStream file1(LOCALFILE("TestData/Test_IO_FileStream1.txt"), FileOpenMode_Write | FileOpenMode_Append);
		ASSERT_EQ(1, file1.GetLength());	// "B" が書かれている
		file1.Write("C", 1);
	}
	// ReadWrite
	{
		FileStream file1(LOCALFILE("TestData/Test_IO_FileStream1.txt"), FileOpenMode_ReadWrite);
		ASSERT_EQ(2, file1.GetLength());	// "BC" が書かれている
		char buf[2];
		file1.Read(buf, 2);
		ASSERT_EQ('B', buf[0]);
		ASSERT_EQ('C', buf[1]);
		file1.Seek(0, SeekOrigin_Begin);
		file1.Write("D", 1);
		ASSERT_EQ(2, file1.GetLength());	// "DC" が書かれている
	}
	// ReadWrite | Append
	{
		FileStream file1(LOCALFILE("TestData/Test_IO_FileStream1.txt"), FileOpenMode_ReadWrite | FileOpenMode_Append);
		ASSERT_EQ(2, file1.GetLength());	// "DC" が書かれている
		file1.Write("E", 1);
		file1.Seek(0, SeekOrigin_Begin);
		char buf[4] = { 0 };
		file1.Read(buf, 3);
		ASSERT_STREQ("DCE", buf);
	}
	// ReadWrite | Truncate
	{
		FileStream file1(LOCALFILE("TestData/Test_IO_FileStream1.txt"), FileOpenMode_ReadWrite | FileOpenMode_Truncate);
		ASSERT_EQ(0, file1.GetLength());	// 空ファイルになる
		file1.Write("F", 1);
		file1.Seek(0, SeekOrigin_Begin);
		char buf[2] = { 0 };
		file1.Read(buf, 1);
		ASSERT_STREQ("F", buf);
	}
	// Read | Truncate
	{
		FileStream file1(LOCALFILE("TestData/Test_IO_FileStream1.txt"), FileOpenMode_Read);
		char buf[2] = { 0 };
		file1.Read(buf, 1);
		ASSERT_STREQ("F", buf);
	}
}
