﻿#include <TestConfig.h>

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

//-----------------------------------------------------------------------------
TEST_F(Test_IO_PathName, CanonicalizePath)
{
// いろいろなケースをテストしやすいようにマクロ化
#define TEST_CASE(result, src) \
	{ \
		TCHAR path1[LN_MAX_PATH] = _T(src); \
		TCHAR path2[LN_MAX_PATH]; \
		PathUtils::CanonicalizePath(path1, _tcslen(path1), path2); \
		ASSERT_STREQ(_T(result), path2); \
	}

	TEST_CASE("A/C", "A/B/../C");
	TEST_CASE("", "");
	TEST_CASE("B.txt", "A1/../A2/A3/../A4/../../B.txt");
	TEST_CASE("A2/B.txt", "A1/../A2/A3/../A4/../A5/../B.txt");
	TEST_CASE("A1/", "A1/");


	TEST_CASE("/", "./");
	TEST_CASE("/", "/.");
	TEST_CASE("", "A/..");
	TEST_CASE("/", "A/../");
	TEST_CASE("", "..");

	TEST_CASE("A/", "A//");		// .NET の動作。
	TEST_CASE("A/", "A////");	// .NET の動作。[A/][/][/][/] とみなし、[/] は破棄
	TEST_CASE("A/B", "A//B");

	TEST_CASE("A", "./A");
	TEST_CASE("A", "././A");

#ifdef LN_WIN32
	TEST_CASE("C:/Projects/bin/Debug/", "C:/Projects/bin/Debug/");
	TEST_CASE("C:/Projects/Debug", "C:/../Projects/Debug");
	TEST_CASE("C:/Debug", "C:/Projects/../Debug");
	TEST_CASE("C:/Debug", "C:/../../Debug");		// .NET の動作。ルートの外側に出ても、.. が消えるだけでフォルダ名は残る
	TEST_CASE("C:/", "C:/");
#endif

#undef TEST_CASE
	//char path[LN_MAX_PATH] = "C:/../dir/file.txt";
	//size_t len = strlen(path);
	//bool r = CanonicalizePath(path, &len);


#ifdef LN_WIN32
	PathName path1(PathName::GetCurrentDirectory(), _T("dir\\Dir"));
	PathName path12 = _T("dir/bin/../Dir");
	path12 = path12.CanonicalizePath();
	ASSERT_STREQ(path1, path12);

	PathName path2(_T("C:\\file.txt"));
	PathName path22 = _T("C:\\dir/..\\file.txt");
	path22 = path22.CanonicalizePath();
	ASSERT_STREQ(path2, path22);
#else
	PathName path1(PathName::GetCurrentDirectory(), _T("dir/Dir"));
	PathName path12 = _T("dir/bin/../Dir");
	path12 = path12.CanonicalizePath();
	ASSERT_STREQ(path1, path12);

	PathName path2(_T("/file.txt"));
	PathName path22 = _T("/dir/../file.txt");
	path22 = path22.CanonicalizePath();
	ASSERT_STREQ(path2, path22);
#endif
}

