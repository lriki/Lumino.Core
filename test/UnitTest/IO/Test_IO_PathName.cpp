﻿#include <TestConfig.h>

class Test_IO_PathName : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//------------------------------------------------------------------------------
TEST_F(Test_IO_PathName, Constructor)
{
	// <Test> String から変換できること
	{
		PathName path = String("dir");
	}
	// <Test> 
	{
		PathName base = _T("dir1/dir2");
		PathName path(base, _T("../file1.txt"));
#ifdef LN_OS_WIN32
		ASSERT_STREQ(_T("dir1/dir2\\../file1.txt"), path.c_str());
#else
		ASSERT_STREQ(_T("dir1/dir2/../file1.txt"), path.c_str());
#endif
	}
	// <Test> 
	{
		PathName path(_T("a/"), _T("b"));
		ASSERT_STREQ(_T("a/b"), path.c_str());

	}
}

//------------------------------------------------------------------------------
TEST_F(Test_IO_PathName, GetFileName)
{
	PathName path1(_T("dir/file.txt"));
	ASSERT_EQ(_T("file.txt"), path1.GetFileName());

	PathName path2(_T("file.txt"));
	ASSERT_EQ(_T("file.txt"), path2.GetFileName());
}

//------------------------------------------------------------------------------
TEST_F(Test_IO_PathName, GetWithoutExtension)
{
	PathName path;

	path = _T("C:/dir/file.txt");
	ASSERT_STREQ(_T("C:/dir/file"), path.GetWithoutExtension().c_str());

	path = _T("file.txt");
	ASSERT_STREQ(_T("file"), path.GetWithoutExtension().c_str());

	path = _T("file");
	ASSERT_STREQ(_T("file"), path.GetWithoutExtension().c_str());

	path = _T("");
	ASSERT_STREQ(_T(""), path.GetWithoutExtension().c_str());

	path = _T("C:/dir.sub/file");
	ASSERT_STREQ(_T("C:/dir.sub/file"), path.GetWithoutExtension().c_str());

	path = _T("dir/.git");
	ASSERT_STREQ(_T("dir/"), path.GetWithoutExtension().c_str());

	path = _T(".git");
	ASSERT_STREQ(_T(""), path.GetWithoutExtension().c_str());
}

//------------------------------------------------------------------------------
TEST_F(Test_IO_PathName, GetExtension)
{
	ASSERT_EQ(_T(".txt"), PathName(_T("file.txt")).GetExtension());
	ASSERT_EQ(_T("txt"), PathName(_T("file.txt")).GetExtension(false));
	ASSERT_EQ(_T(".txt"), PathName(_T("file.tmp.txt")).GetExtension());
	ASSERT_EQ(_T(""), PathName(_T("file")).GetExtension());
	ASSERT_EQ(_T(""), PathName(_T("")).GetExtension());
	ASSERT_EQ(_T(""), PathName(_T(".")).GetExtension());

	ASSERT_EQ(_T(""), PathName(_T("dir.a\file")).GetExtension());
	ASSERT_EQ(_T(""), PathName(_T("file.")).GetExtension());
	ASSERT_EQ(_T(""), PathName(_T("..")).GetExtension());
	ASSERT_EQ(_T(""), PathName(_T("a/")).GetExtension());
	ASSERT_EQ(_T(""), PathName(_T("/")).GetExtension());
}

//------------------------------------------------------------------------------
TEST_F(Test_IO_PathName, Compare)
{
#ifdef LN_OS_WIN32
	PathName path1(_T("C:/dir/file.txt"));
	PathName path2(_T("C:\\dir\\file.txt"));
	ASSERT_TRUE(path1.Equals(path2));
	ASSERT_TRUE(path1 == path2);
#endif
}

//------------------------------------------------------------------------------
TEST_F(Test_IO_PathName, CanonicalizePath)
{
// いろいろなケースをテストしやすいようにマクロ化
#define TEST_CASE(result, src) \
	{ \
		TCHAR path1[LN_MAX_PATH] = _T(src); \
		TCHAR path2[LN_MAX_PATH]; \
		PathTraits::CanonicalizePath(path1, _tcslen(path1), path2); \
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

#ifdef LN_OS_WIN32
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


#ifdef LN_OS_WIN32
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

//------------------------------------------------------------------------------
TEST_F(Test_IO_PathName, GetSpecialFolderPath)
{
	// 何が取れるかはすごく環境依存なので、取ったパスの先がフォルダであるかだけを確認しておく。

	// <Test> アプリケーションデータフォルダ
	{
		PathName path1 = PathName::GetSpecialFolderPath(SpecialFolder::ApplicationData);
		ASSERT_TRUE(path1.ExistsDirectory());
	}
	// <Test> 一時ファイルフォルダ
	{
		PathName path1 = PathName::GetSpecialFolderPath(SpecialFolder::Temporary);
		ASSERT_TRUE(path1.ExistsDirectory());
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_IO_PathName, Unit_MakeRelative)
{
	// <Test> パスが一致する場合は "." を返す
	{
		PathName path1(_T("d1/d2/d3"));
		PathName path2(_T("d1/d2/d3"));
		path1 = path1.CanonicalizePath();
		path2 = path2.CanonicalizePath();
		ASSERT_EQ(_T("."), path1.MakeRelative(path2).GetString());

		// 末尾がセパレータのパターンを見る
		{
			PathName path1 = PathName(_T("d1/d2/d3/")).CanonicalizePath();
			PathName path2 = PathName(_T("d1/d2/d3")).CanonicalizePath();
			ASSERT_EQ(_T("."), path1.MakeRelative(path2).GetString());
		}
		{
			PathName path1 = PathName(_T("d1/d2/d3")).CanonicalizePath();
			PathName path2 = PathName(_T("d1/d2/d3/")).CanonicalizePath();
			ASSERT_EQ(_T("."), path1.MakeRelative(path2).GetString());
		}
		{
			PathName path1 = PathName(_T("d1/d2/d3/")).CanonicalizePath();
			PathName path2 = PathName(_T("d1/d2/d3/")).CanonicalizePath();
			ASSERT_EQ(_T("."), path1.MakeRelative(path2).GetString());
		}
	}
	// <Test> パスの末尾は / があっても無くても良い
	{
		PathName path1(_T("d1/d2/d3"));
		PathName path2(_T("d1/d2/d3"));
		path1 = path1.CanonicalizePath();
		path2 = path2.CanonicalizePath();
		ASSERT_EQ(_T("."), path1.MakeRelative(path2).GetString());
	}
	// <Test> 1つ上のディレクトリへ戻る場合は ".." を返す
	{
		PathName path1(_T("d1/d2/d3/"));
		PathName path2(_T("d1/d2/"));
		path1 = path1.CanonicalizePath();
		path2 = path2.CanonicalizePath();
		ASSERT_EQ(_T(".."), path1.MakeRelative(path2).GetString());

		// 末尾がセパレータのパターンを見る
		{
			PathName path1 = PathName(_T("d1/d2/d3")).CanonicalizePath();
			PathName path2 = PathName(_T("d1/d2/")).CanonicalizePath();
			ASSERT_EQ(_T(".."), path1.MakeRelative(path2).GetString());
		}
		{
			PathName path1 = PathName(_T("d1/d2/d3/")).CanonicalizePath();
			PathName path2 = PathName(_T("d1/d2")).CanonicalizePath();
			ASSERT_EQ(_T(".."), path1.MakeRelative(path2).GetString());
		}
		{
			PathName path1 = PathName(_T("d1/d2/d3")).CanonicalizePath();
			PathName path2 = PathName(_T("d1/d2")).CanonicalizePath();
			ASSERT_EQ(_T(".."), path1.MakeRelative(path2).GetString());
		}
	}
	// <Test> 2つ上のディレクトリへ戻る場合は "../.." を返す
	{
		PathName path1 = PathName(_T("d1/d2/d3/")).CanonicalizePath();
		PathName path2 = PathName(_T("d1/")).CanonicalizePath();
		ASSERT_EQ(_T("../.."), path1.MakeRelative(path2).GetString());
		// 末尾がセパレータのパターンを見る
		{
			PathName path1 = PathName(_T("d1/d2/d3")).CanonicalizePath();
			PathName path2 = PathName(_T("d1/")).CanonicalizePath();
			ASSERT_EQ(_T("../.."), path1.MakeRelative(path2).GetString());
		}
		{
			PathName path1 = PathName(_T("d1/d2/d3/")).CanonicalizePath();
			PathName path2 = PathName(_T("d1")).CanonicalizePath();
			ASSERT_EQ(_T("../.."), path1.MakeRelative(path2).GetString());
		}
		{
			PathName path1 = PathName(_T("d1/d2/d3")).CanonicalizePath();
			PathName path2 = PathName(_T("d1")).CanonicalizePath();
			ASSERT_EQ(_T("../.."), path1.MakeRelative(path2).GetString());
		}
	}
	// <Test> 1つ上のディレクトリへ戻る場合は ".." を返す
	{
		PathName path1 = PathName(_T("d1/d2/")).CanonicalizePath();
		PathName path2 = PathName(_T("d1/d2/d3/")).CanonicalizePath();
		ASSERT_EQ(_T("d3"), path1.MakeRelative(path2).GetString());
		// 末尾がセパレータのパターンを見る
		{
			PathName path1 = PathName(_T("d1/d2")).CanonicalizePath();
			PathName path2 = PathName(_T("d1/d2/d3/")).CanonicalizePath();
			ASSERT_EQ(_T("d3"), path1.MakeRelative(path2).GetString());
		}
		{
			PathName path1 = PathName(_T("d1/d2/")).CanonicalizePath();
			PathName path2 = PathName(_T("d1/d2/d3")).CanonicalizePath();
			ASSERT_EQ(_T("d3"), path1.MakeRelative(path2).GetString());
		}
		{
			PathName path1 = PathName(_T("d1/d2")).CanonicalizePath();
			PathName path2 = PathName(_T("d1/d2/d3")).CanonicalizePath();
			ASSERT_EQ(_T("d3"), path1.MakeRelative(path2).GetString());
		}
	}
	// <Test> 2つ上のディレクトリへ戻る場合は "../.." を返す
	{
		PathName path1 = PathName(_T("d1/")).CanonicalizePath();
		PathName path2 = PathName(_T("d1/d2/d3")).CanonicalizePath();
		ASSERT_EQ(_T("d2/d3"), path1.MakeRelative(path2).GetString().Replace(_T("\\"), _T("/")));
	}
}

//------------------------------------------------------------------------------
// internal クラスだけど一応テストしておく
TEST_F(Test_IO_PathName, GenericStaticallyLocalPath)
{
	const char* a255 = "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345";
	const wchar_t* w255 = L"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345";
	const char* a256 = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456";
	const wchar_t* w256 = L"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456";

	{
		// 基本
		detail::StaticallyLocalPathA path1("abc");
		ASSERT_EQ(true, path1.IsStatic());
		ASSERT_STREQ("abc", path1.c_str());

		// 255 文字
		detail::StaticallyLocalPathA path2(a255);
		ASSERT_EQ(true, path2.IsStatic());
		ASSERT_STREQ(a255, path2.c_str());

		// 256 文字
		detail::StaticallyLocalPathA path3(a256);
		ASSERT_EQ(false, path3.IsStatic());
		ASSERT_STREQ(a256, path3.c_str());

		// 255 変換
		detail::StaticallyLocalPathA path4(w255);
		ASSERT_EQ(true, path4.IsStatic());
		ASSERT_STREQ(a255, path4.c_str());

		// 256 変換
		detail::StaticallyLocalPathA path5(w256);
		ASSERT_EQ(false, path5.IsStatic());
		ASSERT_STREQ(a256, path5.c_str());
	}

	{
		// 基本
		detail::StaticallyLocalPathW path1("abc");
		ASSERT_EQ(true, path1.IsStatic());
		ASSERT_STREQ(L"abc", path1.c_str());

		// 255 文字
		detail::StaticallyLocalPathW path2(w255);
		ASSERT_EQ(true, path2.IsStatic());
		ASSERT_STREQ(w255, path2.c_str());

		// 256 文字
		detail::StaticallyLocalPathW path3(w256);
		ASSERT_EQ(false, path3.IsStatic());
		ASSERT_STREQ(w256, path3.c_str());

		// 255 変換
		detail::StaticallyLocalPathW path4(a255);
		ASSERT_EQ(true, path4.IsStatic());
		ASSERT_STREQ(w255, path4.c_str());

		// 256 変換
		detail::StaticallyLocalPathW path5(a256);
		ASSERT_EQ(false, path5.IsStatic());
		ASSERT_STREQ(w256, path5.c_str());
	}
}

