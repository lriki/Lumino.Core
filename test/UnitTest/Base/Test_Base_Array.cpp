#include <TestConfig.h>

class Test_Base_Array : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

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

