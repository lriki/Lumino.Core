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
	// Windows 別ユーザーフォルダは false
	//ASSERT_FALSE(FileUtils::Exists("C:/Users/user2/Documents/Visual Studio 2013/Settings/CurrentSettings.vssettings"));

	ASSERT_TRUE(FileUtils::Exists(LN_TEST_GET_FILE_PATH("TestData/test1.txt")));
	// ディレクトリは false
	ASSERT_FALSE(FileUtils::Exists(LN_TEST_GET_FILE_PATH("TestData")));
	// 空文字列は false
	ASSERT_FALSE(FileUtils::Exists(""));
	// 空文字列は false
	ASSERT_FALSE(FileUtils::Exists(L""));
	// NULL は false
	ASSERT_FALSE(FileUtils::Exists((char*)NULL));
	// NULL は false
	ASSERT_FALSE(FileUtils::Exists((wchar_t*)NULL));
}

//-----------------------------------------------------------------------------
TEST_F(Test_IO_FileUtils, GetAttribute)
{
	uint32_t attr;

	// 
	attr = FileUtils::GetAttribute(LOCALFILE("TestData/readonly.txt"));
	attr |= FileAttribute_ReadOnly;
	FileUtils::SetAttribute(LOCALFILE("TestData/readonly.txt"), attr);

	// ディレクトリ
	attr = FileUtils::GetAttribute(LOCALFILE("TestData"));
	ASSERT_EQ(FileAttribute_Directory, attr);

	// 読み取り専用
	attr = FileUtils::GetAttribute(LOCALFILE("TestData/readonly.txt"));
	ASSERT_EQ(FileAttribute_ReadOnly, attr);

#ifdef LN_WIN32
#else
	// 隠しファイル
	attr = FileUtils::GetAttribute(LOCALFILE("TestData/.test"));
	ASSERT_EQ(FileAttribute_Hidden, attr);
#endif

	//try
	//{
	//	// 非常に長いパス
	//	String longPath(1000, _T('a'));
	//	FileUtils::GetAttribute(longPath);
	//}
	//catch (Exception& e)
	//{
	//	e;
	//}
}


//-----------------------------------------------------------------------------
TEST_F(Test_IO_FileUtils, Copy_Delete)
{
	String src1 = LOCALFILE("TestData/test1.txt");
	String src2 = LOCALFILE("TestData/test2.txt");
	String dest = LOCALFILE("TestData/test_copy.txt");
	size_t src1Size = FileUtils::GetFileSize(src1);
	size_t src2Size = FileUtils::GetFileSize(src2);

	// 最初はコピー先ファイルが無いこと。
	ASSERT_FALSE(FileUtils::Exists(dest));

	// コピー
	FileUtils::Copy(src1, dest, false);

	// コピーしたファイル(サイズ)が同じ
	ASSERT_EQ(src1Size, FileUtils::GetFileSize(dest));

	// 上書きしようとすると IOException
	ASSERT_THROW(
		FileUtils::Copy(src2, dest, false),
		IOException);

	// 上書き許可でコピー
	FileUtils::Copy(src2, dest, true);

	// コピーしたファイル(サイズ)が同じ
	ASSERT_EQ(src2Size, FileUtils::GetFileSize(dest));

	// ファイル削除
	FileUtils::Delete(dest);

	// 消えている
	ASSERT_FALSE(FileUtils::Exists(dest));
}

