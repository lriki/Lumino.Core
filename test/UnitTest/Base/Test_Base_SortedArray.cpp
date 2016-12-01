#include <TestConfig.h>
#include <Lumino/Base/SortedArray.h>
#include <Lumino/Base/ElapsedTimer.h>
#include <map>
#include <unordered_map>
#include <random>

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
	struct Pair
	{
		int key;
		int value;
	};

	const int TryCount = 100;
	const int Count = 10;
	//String keys[Count];
	std::map<int, int>				map1;
	std::unordered_map<int, int>	hashmap1;
	SortedArray<int, int>			sary1;
	List<Pair>						list1;

	hashmap1.reserve(1000);

	for (int i = 0; i < Count; ++i)
	{
		//String key = String::Format(_T("key%d"), i);
		//keys[i] = key;
		map1[i] = i;
		list1.Add(Pair{i, i});
	}
	printf("unordered_map insert ----\n");
	{
		ElapsedTimer timer;
		timer.Start();
		for (int i = 0; i < Count; ++i)
		{
			hashmap1[i] = i;
		}
		printf("t:%llu\n", timer.GetElapsedTimeNS());	// VS2015:73,552ns
		// ※ clear や reserve してもあまり変わらない。
	}
	printf("unordered_map copy ----\n");
	{
		std::unordered_map<int, int>	hashmap2;
		hashmap2.reserve(1000);
		for (int j = 0; j < TryCount; j++)
		{
			ElapsedTimer timer;
			timer.Start();
			hashmap2 = hashmap1;
			printf("t:%llu\n", timer.GetElapsedTimeNS());	// VS2015:82,675ns
		}
	}
	printf("SortedArray insert ----\n");
	{
		ElapsedTimer timer;
		timer.Start();
		for (int i = 0; i < Count; ++i)
		{
			sary1.Add(i, i);
		}
		printf("t:%llu\n", timer.GetElapsedTimeNS());
	}
	printf("map find ----\n");
	for (int j = 0; j < TryCount; j++)
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
	printf("unordered_map find ----\n");
	{
		ElapsedTimer timer;
		timer.Start();
		for (int j = 0; j < TryCount; j++)
		{
			int sum = 0;
			for (int i = Count - 1; i >= 0; --i)
			{
				sum += hashmap1[i];
			}
		}
		printf("t:%llu\n", timer.GetElapsedTimeNS());		// VS2015:8,837ns
	}
	printf("SortedArray find ----\n");
	{
		ElapsedTimer timer;
		timer.Start();
		for (int j = 0; j < TryCount; j++)
		{
			int sum = 0;
			for (int i = Count - 1; i >= 0; --i)
			{
				int v;
				sary1.TryGetValue(i, &v);
				sum += v;
			}
		}
		printf("t:%llu\n", timer.GetElapsedTimeNS());
	}
	printf("List find ----\n");
	{
		ElapsedTimer timer;
		timer.Start();
		for (int j = 0; j < TryCount; j++)
		{
			int sum = 0;
			for (int i = Count - 1; i >= 0; --i)
			{
				sum += list1.Find([i](const Pair& p) { return p.key == i; })->value;
			}
		}
		printf("t:%llu\n", timer.GetElapsedTimeNS());
	}


	//printf("map ----\n");
	//for (int j = 0; j < TryCount; j++)
	//{
	//	ElapsedTimer timer;
	//	timer.Start();
	//	int sum = 0;
	//	for (auto& pair : map1)
	//	{
	//		sum += pair.second;
	//	}
	//	printf("t:%llu\n", timer.GetElapsedTimeNS());
	//	//printf("sum:%d\n", sum);
	//}
	//printf("SortedArray ----\n");
	//for (int j = 0; j < TryCount; j++)
	//{
	//	ElapsedTimer timer;
	//	timer.Start();
	//	int sum = 0;
	//	for (auto& pair : map1)
	//	{
	//		sum += pair.second;
	//	}
	//	printf("t:%llu\n", timer.GetElapsedTimeNS());
	//	//printf("sum:%d\n", sum);
	//}
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

//------------------------------------------------------------------------------
TEST_F(Test_Base_SortedArray, StringKey)
{
	SortedArray<String, int> ary1;
	ary1.Add(_T("a"), 1);
	ary1.Add(_T("aaa"), 2);
	ary1.Add(_T("aa"), 3);
	ary1.Add(_T("c"), 4);
	ary1.Add(_T("b"), 5);
	ASSERT_EQ(1, *ary1.Find(_T("a")));
	ASSERT_EQ(2, *ary1.Find(_T("aaa")));
	ASSERT_EQ(3, *ary1.Find(_T("aa")));
	ASSERT_EQ(4, *ary1.Find(_T("c")));
	ASSERT_EQ(5, *ary1.Find(_T("b")));
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_SortedArray, StringCaseInsensitiveLess)
{
	SortedArray<String, int, detail::StringCaseInsensitiveLess<String>> ary1;
	ary1.Add(_T("a"), 1);
	ary1.Add(_T("aaa"), 2);
	ary1.Add(_T("aa"), 3);
	ary1.Add(_T("c"), 4);
	ary1.Add(_T("b"), 5);
	ASSERT_EQ(1, *ary1.Find(_T("a")));
	ASSERT_EQ(1, *ary1.Find(_T("A")));
	ASSERT_EQ(2, *ary1.Find(_T("aaa")));
	ASSERT_EQ(2, *ary1.Find(_T("aAa")));
	ASSERT_EQ(2, *ary1.Find(_T("AAA")));
	ASSERT_EQ(3, *ary1.Find(_T("aa")));
	ASSERT_EQ(3, *ary1.Find(_T("AA")));
	ASSERT_EQ(4, *ary1.Find(_T("c")));
	ASSERT_EQ(4, *ary1.Find(_T("C")));

#if defined(LN_DEBUG)
	int cnt = detail::g_testGenericStringCoreAllocCount;	// 一連の検索の中で一時文字列が確保されないことを確認する
	ASSERT_EQ(5, *ary1.Find(_T("b")));
	ASSERT_EQ(5, *ary1.Find(_T("B")));
	ASSERT_EQ(5, *ary1.Find(StringRef(_T("b"))));
	ASSERT_EQ(5, *ary1.Find(StringRef(_T("B"))));
	ASSERT_EQ(cnt, detail::g_testGenericStringCoreAllocCount);
#endif
}

//------------------------------------------------------------------------------
TEST_F(Test_Base_SortedArray, Performance)
{
	const int loop = 1000;
	std::map<std::basic_string<TCHAR>, int> table1;
	std::map<String, int> table2;
	SortedArray<String, int> ary1;
	SortedArray<std::basic_string<TCHAR>, int> ary2;
	std::vector<std::basic_string<TCHAR>> index1;
	List<String> index2;
	for (int i = 0; i < loop; ++i)
	{
		String str = String::Format(_T("{0}"), i);
		table1[std::basic_string<TCHAR>(str.c_str())] = i;
		table2[str] = i;
		ary1.Add(str, i);
		ary2.Add(str.c_str(), i);
		index1.push_back(str.c_str());
		index2.Add(str);
	}
	std::shuffle(index1.begin(), index1.end(), std::mt19937());
	std::shuffle(index2.begin(), index2.end(), std::mt19937());

	// map + std::string
	uint64_t mapTime1;
	{
		int s = 0;
		ElapsedTimer t;
		t.Start();
		for (auto& i : index1)
		{
			s += table1[i];
		}
		mapTime1 = t.GetElapsed();
	}

	// map + String
	uint64_t mapTime2;
	{
		int s = 0;
		ElapsedTimer t;
		t.Start();
		for (String& i : index2)
		{
			s += table2[i];
		}
		mapTime2 = t.GetElapsed();
	}

	// SortedArray + String
	uint64_t saTime;
	{
		int s = 0;
		ElapsedTimer t;
		t.Start();
		for (String& i : index2)
		{
			s += *ary1.Find(i);
		}
		saTime = t.GetElapsed();
	}
	// SortedArray + std::string
	uint64_t saTime2;
	{
		int s = 0;
		ElapsedTimer t;
		t.Start();
		for (auto& i : index1)
		{
			s += *ary2.Find(i);
		}
		saTime2 = t.GetElapsed();
	}

	printf("mapTime1 %llu\n", mapTime1);
	printf("mapTime2 %llu\n", mapTime2);
	printf("saTime1 %llu\n", saTime);
	printf("saTime2 %llu\n", saTime2);

	//2000要素。こんなかんじ。(デバッグ実行)
	//mapTime1 4995276
	//mapTime2 7294779
	//saTime1 3862059
	//saTime2 2363362
	//とりあえず、
	// - String の比較で毎回 strlen 走ってるのが悪い。
	// - std::map < SortedArray
	// - Release + SortedArray + std::string では 10000 ループでようやく 200ns とかスコアが出るくらい高速。

	ASSERT_EQ(true, saTime2 <= mapTime1);
	ASSERT_EQ(true, saTime2 <= mapTime2);
}

