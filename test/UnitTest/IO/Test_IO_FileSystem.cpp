﻿#include <TestConfig.h>

class Test_IO_FileSystem : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_IO_FileSystem, Exists)
{
	// Windows 別ユーザーフォルダは false
	//ASSERT_FALSE(FileSystem::Exists("C:/Users/user2/Documents/Visual Studio 2013/Settings/CurrentSettings.vssettings"));

	ASSERT_TRUE(FileSystem::Exists(LN_TEST_GET_FILE_PATH("TestData/test1.txt")));
	// ディレクトリは false
	ASSERT_FALSE(FileSystem::Exists(LN_TEST_GET_FILE_PATH("TestData")));
	// 空文字列は false
	ASSERT_FALSE(FileSystem::Exists(""));
	// 空文字列は false
	ASSERT_FALSE(FileSystem::Exists(L""));
	// NULL は false
	ASSERT_FALSE(FileSystem::Exists((char*)NULL));
	// NULL は false
	ASSERT_FALSE(FileSystem::Exists((wchar_t*)NULL));
}

//-----------------------------------------------------------------------------
TEST_F(Test_IO_FileSystem, GetAttribute)
{
	uint32_t attr;

	// 
	attr = FileSystem::GetAttribute(LOCALFILE("TestData/readonly.txt"));
	attr |= FileAttribute_ReadOnly;
	FileSystem::SetAttribute(LOCALFILE("TestData/readonly.txt"), attr);

	// ディレクトリ
	attr = FileSystem::GetAttribute(LOCALFILE("TestData"));
	ASSERT_EQ(FileAttribute_Directory, attr);

	// 読み取り専用
	attr = FileSystem::GetAttribute(LOCALFILE("TestData/readonly.txt"));
	ASSERT_EQ(FileAttribute_ReadOnly, attr);

#ifdef LN_WIN32
#else
	// 隠しファイル
	attr = FileSystem::GetAttribute(LOCALFILE("TestData/.test"));
	ASSERT_EQ(FileAttribute_Hidden, attr);
#endif

	//try
	//{
	//	// 非常に長いパス
	//	String longPath(1000, _T('a'));
	//	FileSystem::GetAttribute(longPath);
	//}
	//catch (Exception& e)
	//{
	//	e;
	//}
}


//-----------------------------------------------------------------------------
TEST_F(Test_IO_FileSystem, Copy_Delete)
{
	String src1 = LOCALFILE("TestData/test1.txt");
	String src2 = LOCALFILE("TestData/test2.txt");
	String dest = LOCALFILE("TestData/test_copy.txt");
	uint64_t src1Size = FileSystem::GetFileSize(src1);
	uint64_t src2Size = FileSystem::GetFileSize(src2);

	// 最初はコピー先ファイルが無いこと。
	ASSERT_FALSE(FileSystem::Exists(dest));

	// コピー
	FileSystem::Copy(src1, dest, false);

	// コピーしたファイル(サイズ)が同じ
	ASSERT_EQ(src1Size, FileSystem::GetFileSize(dest));

	// 上書きしようとすると IOException
	ASSERT_THROW(
		FileSystem::Copy(src2, dest, false),
		IOException);

	// 上書き許可でコピー
	FileSystem::Copy(src2, dest, true);

	// コピーしたファイル(サイズ)が同じ
	ASSERT_EQ(src2Size, FileSystem::GetFileSize(dest));

	// ファイル削除
	FileSystem::Delete(dest);

	// 消えている
	ASSERT_FALSE(FileSystem::Exists(dest));
}
