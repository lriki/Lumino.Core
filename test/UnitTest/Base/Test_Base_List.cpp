#include <TestConfig.h>
#include <Lumino/Base/List.h>

class Test_Base_List : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

class Test1
{
public:
	Test1()
	{
		printf("default\n");
	}

	Test1(const Test1& obj)
	{
		printf("copy\n");
	}
};

//-----------------------------------------------------------------------------
TEST_F(Test_Base_List, Basic)
{
	//List<int> list;

	//std::vector<Test1> ary;
	//ary.reserve
	//ary.push_back(Test1());
	//printf("%d\n", ary.capacity());
	//ary.push_back(Test1());
	//printf("%d\n", ary.capacity());
	//ary.push_back(Test1());
	//printf("%d\n", ary.capacity());
	//ary.push_back(Test1());
	//printf("%d\n", ary.capacity());
	//ary.push_back(Test1());
	//printf("%d\n", ary.capacity());
	//ary.push_back(Test1());
	//printf("%d\n", ary.capacity());
	//ary.push_back(Test1());
	//printf("%d\n", ary.capacity());
	//ary.push_back(Test1());
	//printf("%d\n", ary.capacity());
	//ary.push_back(Test1());
	//printf("%d\n", ary.capacity());

}

