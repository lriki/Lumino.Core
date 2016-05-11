#include <TestConfig.h>
#include <map>

class Test_Base_SortedArray : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//------------------------------------------------------------------------------
TEST_F(Test_Base_SortedArray, Add)
{
	// <Test> Add
	{
		SortedArray<int, int> ary1;
		ary1.Add(1, 10);
		ary1.Add(2, 20);
		ASSERT_EQ(2, ary1.GetCount());
	}

	// <Test> キーが既に格納されていれば ArgumentException
	{
		SortedArray<int, int> ary1;
		ary1.Add(1, 10);
		ASSERT_THROW(
			ary1.Add(1, 20),
			ArgumentException);
	}
	
#if 0
	const int Count = 100000;
	//String keys[Count];
	std::map<int, int>		map1;
	SortedArray<int, int>	sary1;
	for (int i = 0; i < Count; ++i)
	{
		//String key = String::Format(_T("key%d"), i);
		//keys[i] = key;
		map1[i] = i;
		sary1.Add(i, i);
	}
	printf("map ----\n");
	for (int j = 0; j < 10; j++)
	{
		ElapsedTimer timer;
		timer.Start();
		int sum = 0;
		//for (int i = 0; i < Count; ++i)
		for (int i = Count - 1; i >= 0; --i)
		{
			sum += map1[i];
		}
		printf("t:%llu\n", timer.GetElapsedTimeNS());
		//printf("sum:%d\n", sum);
	}
	printf("SortedArray ----\n");
	for (int j = 0; j < 10; j++)
	{
		ElapsedTimer timer;
		timer.Start();
		int sum = 0;
		//for (int i = 0; i < Count; ++i)
		for (int i = Count - 1; i >= 0; --i)
		{
			int v;
			sary1.TryGetValue(i, &v);
			sum += v;
		}
		printf("t:%llu\n", timer.GetElapsedTimeNS());
		//printf("sum:%d\n", sum);
	}


	printf("map ----\n");
	for (int j = 0; j < 10; j++)
	{
		ElapsedTimer timer;
		timer.Start();
		int sum = 0;
		for (auto& pair : map1)
		{
			sum += pair.second;
		}
		printf("t:%llu\n", timer.GetElapsedTimeNS());
		//printf("sum:%d\n", sum);
	}
	printf("SortedArray ----\n");
	for (int j = 0; j < 10; j++)
	{
		ElapsedTimer timer;
		timer.Start();
		int sum = 0;
		for (auto& pair : map1)
		{
			sum += pair.second;
		}
		printf("t:%llu\n", timer.GetElapsedTimeNS());
		//printf("sum:%d\n", sum);
	}
#endif
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_SortedArray, RemoveAllValue)
{
	// <Test> RemoveAllValue
	{
		SortedArray<int, int> ary1;
		ary1.Add(1, 10);
		ary1.Add(2, 20);
		ary1.Add(3, 10);
		ary1.RemoveAllValue(10);
		ASSERT_EQ(1, ary1.GetCount());
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_SortedArray, ContainsKey)
{
	// <Test> ContainsKey
	{
		SortedArray<int, int> ary1;
		ASSERT_FALSE(ary1.ContainsKey(1));
		ary1.Add(1, 10);
		ASSERT_TRUE(ary1.ContainsKey(1));
		ASSERT_FALSE(ary1.ContainsKey(2));
		ASSERT_FALSE(ary1.ContainsKey(0));
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_SortedArray, SetValue_GetValue)
{
	// <Test> SetValue
	{
		SortedArray<int, int> ary1;
		ary1.SetValue(1, 10);
		ASSERT_EQ(10, ary1.GetValue(1));
		ary1.SetValue(1, 20);	// 更新
		ASSERT_EQ(20, ary1.GetValue(1));
		ary1.SetValue(2, 20);
		ASSERT_EQ(20, ary1.GetValue(2));
	}

	// <Test> キーが無ければ KeyNotFoundException
	{
		SortedArray<int, int> ary1;
		ASSERT_THROW(
			ary1.GetValue(1),
			KeyNotFoundException);
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_SortedArray, operators)
{
	// <Test> oeprator[]
	{
		SortedArray<int, int> ary1;
		ary1[1] = 10;
		ASSERT_EQ(10, ary1[1]);

		const SortedArray<int, int>& ary2 = ary1;
		ASSERT_THROW(
			ary2[100],
			KeyNotFoundException);
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_SortedArray, iterator)
{
	// <Test> iterator
	{
		int sum = 0;
		SortedArray<int, int> ary1;
		ary1[1] = 1;
		ary1[10] = 2;
		ary1[100] = 3;

		for (auto& v : ary1) {
			sum += v.second;
		}
		ASSERT_EQ(6, sum);

		const SortedArray<int, int>& ary2 = ary1;
		for (const auto& v : ary2) {
			sum += v.second;
		}
		ASSERT_EQ(12, sum);
	}
}
