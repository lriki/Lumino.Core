#include <TestConfig.h>

class Test_Base_Array : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, Add)
{
	// 配列の結合
	ArrayList<String> list;

	String str1(_T(".c;.cc;"));
	list.Add(str1.Split(_T(";"), StringSplitOptions_RemoveEmptyEntries));
	String str2(_T(".cpp;.cxx;"));
	list.Add(str2.Split(_T(";"), StringSplitOptions_RemoveEmptyEntries));

	ASSERT_EQ(4, list.GetCount());
	ASSERT_STREQ(_T(".c"), list[0]);
	ASSERT_STREQ(_T(".cc"), list[1]);
	ASSERT_STREQ(_T(".cpp"), list[2]);
	ASSERT_STREQ(_T(".cxx"), list[3]);
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, Contains)
{
	PathNameA p1("C:/LuminoCore/include/Lumino/Base/NonCopyable.h");
	PathNameA p2("C:/LuminoCore/include/Lumino/Base/NonCopyable.h");
	Array<PathNameA> pathes;
	pathes.Add(p1);
	ASSERT_TRUE(pathes.Contains(p2));




	/*
	try
	{
		LN_THROW(0, ArgumentException);
	}
	catch (ArgumentException& e)
	{
		printf(e.what());
	}
	*/
}

