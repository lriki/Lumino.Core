#include <TestConfig.h>

class Test_IO_PathName : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};


//-----------------------------------------------------------------------------
TEST_F(Test_IO_PathName, GetFileName)
{
	PathName path1(_T("dir/file.txt"));
	ASSERT_STREQ(_T("file.txt"), path1.GetFileName());

	PathName path2(_T("file.txt"));
	ASSERT_STREQ(_T("file.txt"), path2.GetFileName());
}

//-----------------------------------------------------------------------------
TEST_F(Test_IO_PathName, GetWithoutExtension)
{
	PathName path;

	path = _T("C:/dir/file.txt");
	ASSERT_STREQ(_T("C:/dir/file"), path.GetWithoutExtension().GetCStr());

	path = _T("file.txt");
	ASSERT_STREQ(_T("file"), path.GetWithoutExtension().GetCStr());

	path = _T("file");
	ASSERT_STREQ(_T("file"), path.GetWithoutExtension().GetCStr());

	path = _T("");
	ASSERT_STREQ(_T(""), path.GetWithoutExtension().GetCStr());

	path = _T("C:/dir.sub/file");
	ASSERT_STREQ(_T("C:/dir.sub/file"), path.GetWithoutExtension().GetCStr());

	path = _T("dir/.git");
	ASSERT_STREQ(_T("dir/"), path.GetWithoutExtension().GetCStr());

	path = _T(".git");
	ASSERT_STREQ(_T(""), path.GetWithoutExtension().GetCStr());
}

//-----------------------------------------------------------------------------
TEST_F(Test_IO_PathName, Compare)
{
#ifdef LN_WIN32
	PathName path1(_T("C:/dir/file.txt"));
	PathName path2(_T("C:\\dir\\file.txt"));
	ASSERT_TRUE(path1.Equals(path2));
	ASSERT_TRUE(path1 == path2);
#endif
}
