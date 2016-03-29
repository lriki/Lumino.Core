#include <TestConfig.h>

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

	ASSERT_TRUE(FileSystem::ExistsFile(LN_LOCALFILE("TestData/test1.txt")));
	// ディレクトリは false
	ASSERT_FALSE(FileSystem::ExistsFile(LN_LOCALFILE("TestData")));
	// 空文字列は false
	ASSERT_FALSE(FileSystem::ExistsFile(""));
	// 空文字列は false
	ASSERT_FALSE(FileSystem::ExistsFile(L""));
	// NULL は false
	ASSERT_FALSE(FileSystem::ExistsFile((char*)NULL));
	// NULL は false
	ASSERT_FALSE(FileSystem::ExistsFile((wchar_t*)NULL));
}

//-----------------------------------------------------------------------------
TEST_F(Test_IO_FileSystem, GetAttribute)
{
	uint32_t attr;

	// <Test> ファイルを読み取り専用にする
	// <Test> ファイルの読み取り専用を確認する
	{
		attr = FileSystem::GetAttribute(LOCALFILE("TestData/readonly.txt"));
		attr |= FileAttribute::ReadOnly;
		FileSystem::SetAttribute(LOCALFILE("TestData/readonly.txt"), attr);
	}

	// <Test> ファイルの読み取り専用を確認する
	{
		attr = FileSystem::GetAttribute(LOCALFILE("TestData/readonly.txt"));
		ASSERT_EQ(FileAttribute::ReadOnly, attr);
	}

	// <Test> ファイルの読み取り専用を解除する
	{
		attr = FileSystem::GetAttribute(LOCALFILE("TestData/readonly.txt"));
		attr &=~ FileAttribute::ReadOnly;
		FileSystem::SetAttribute(LOCALFILE("TestData/readonly.txt"), attr);

		// 確認
		attr = FileSystem::GetAttribute(LOCALFILE("TestData/readonly.txt"));
		ASSERT_EQ(FileAttribute::Normal, attr);
	}

	// <Test> ディレクトリ属性を確認する
	{
		attr = FileSystem::GetAttribute(LOCALFILE("TestData"));
		ASSERT_EQ(FileAttribute::Directory, attr);
	}

#ifdef LN_OS_WIN32
#else
	// 隠しファイル
	attr = FileSystem::GetAttribute(LOCALFILE("TestData/.test"));
	ASSERT_EQ(FileAttribute::Hidden, attr);
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
	String dest = TEMPFILE("test_copy.txt");
	uint64_t src1Size = FileSystem::GetFileSize(src1.c_str());
	uint64_t src2Size = FileSystem::GetFileSize(src2.c_str());

	// 最初はコピー先ファイルが無いこと。
	ASSERT_FALSE(FileSystem::ExistsFile(dest.c_str()));

	// コピー
	FileSystem::Copy(src1.c_str(), dest.c_str(), false);

	// コピーしたファイル(サイズ)が同じ
	ASSERT_EQ(src1Size, FileSystem::GetFileSize(dest.c_str()));

	// 上書きしようとすると IOException
	ASSERT_THROW(
		FileSystem::Copy(src2.c_str(), dest.c_str(), false),
		IOException);

	// 上書き許可でコピー
	FileSystem::Copy(src2.c_str(), dest.c_str(), true);

	// コピーしたファイル(サイズ)が同じ
	ASSERT_EQ(src2Size, FileSystem::GetFileSize(dest.c_str()));

	// ファイル削除
	FileSystem::Delete(dest.c_str());

	// 消えている
	ASSERT_FALSE(FileSystem::ExistsFile(dest.c_str()));
}

//-----------------------------------------------------------------------------
TEST_F(Test_IO_FileSystem, CreateDirectory)
{
	FileSystem::CreateDirectory(TEMPFILE("Test_IO_FileSystem/CreateDirectory"));
	FileSystem::WriteAllText(TEMPFILE("Test_IO_FileSystem/CreateDirectory/file"), _T("test"));
	FileSystem::DeleteDirectory(TEMPFILE("Test_IO_FileSystem"), true);
	ASSERT_FALSE(FileSystem::ExistsDirectory(TEMPFILE("Test_IO_FileSystem")));	// 消えている
}

//-----------------------------------------------------------------------------
TEST_F(Test_IO_FileSystem, ForEachFilesInDirectory)
{
	FileSystem::CreateDirectory(TEMPFILE("Test_IO_FileSystem/ForEachFilesInDirectory"));
	FileSystem::CreateDirectory(TEMPFILE("Test_IO_FileSystem/ForEachFilesInDirectory/dir1"));
	FileSystem::CreateDirectory(TEMPFILE("Test_IO_FileSystem/ForEachFilesInDirectory/dir2"));
	FileSystem::WriteAllText(TEMPFILE("Test_IO_FileSystem/ForEachFilesInDirectory/file1"), _T("test"));
	FileSystem::WriteAllText(TEMPFILE("Test_IO_FileSystem/ForEachFilesInDirectory/file2"), _T("test"));

	StringArray list;
	FileSystem::ForEachFilesInDirectory<TCHAR>(TEMPFILE("Test_IO_FileSystem/ForEachFilesInDirectory"), [&list](const TCHAR* path) { list.Add(path); });

	ASSERT_EQ(4, list.GetCount());
	ASSERT_EQ(_T("dir1"), list[0]);
	ASSERT_EQ(_T("dir2"), list[1]);
	ASSERT_EQ(_T("file1"), list[2]);
	ASSERT_EQ(_T("file2"), list[3]);
}
