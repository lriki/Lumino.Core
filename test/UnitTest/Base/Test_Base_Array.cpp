#include <TestConfig.h>

class Test_Base_Array : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, IsEmpty)
{
	// <Test> 配列が空かを確認する。
	{
		Array<int> a1;
		ASSERT_TRUE(a1.IsEmpty());
		a1.Add(1);
		ASSERT_FALSE(a1.IsEmpty());
		a1.Clear();
		ASSERT_TRUE(a1.IsEmpty());
	}
	

#if 0
	String str = "";
	Array<String> ary1;
	Array<String> ary2;

	printf("----\n");
	for (int j = 0; j < 10; j++)
	{
		ElapsedTimer timer;
		timer.Start();

		for (int i = 0; i < 1000000; ++i)
		{
			ary1 = StringTraits::Split(str, _T(","));
			Array<String> a11 = ary1;
		}
		printf("t:%llu\n", timer.GetElapsedTime());
	}
	printf("----\n");
	//for (int j = 0; j < 10; j++)
	//{
	//	ElapsedTimer timer;
	//	timer.Start();

	//	for (int i = 0; i < 1000000; ++i)
	//	{
	//		ary2 = StringTraits::Split2(str, _T(","));
	//		Array<String> a22 = ary2;
	//	}
	//	printf("t:%llu\n", timer.GetElapsedTime());
	//}
#endif
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, GetCount)
{
	// <Test> 要素数を取得する。
	{
		Array<int> a1;
		ASSERT_EQ(0, a1.GetCount());
		a1.Add(1);
		ASSERT_EQ(1, a1.GetCount());
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, Add)
{
	// <Test> 要素を追加する。
	{
		Array<int> a1;
		a1.Add(1);
		a1.Add(2);
		ASSERT_EQ(2, a1.GetCount());
		ASSERT_EQ(1, a1[0]);
		ASSERT_EQ(2, a1[1]);
	}
	// <Test> 配列を末尾に結合する。
	{
		Array<String> a1, a2, a3;
		a2.Add(_T("1")); a2.Add(_T("2"));
		a3.Add(_T("3")); a3.Add(_T("4"));
		a1.Add(a2); a1.Add(a3);
		ASSERT_EQ(4, a1.GetCount());
		ASSERT_STREQ(_T("1"), a1[0]);
		ASSERT_STREQ(_T("2"), a1[1]);
		ASSERT_STREQ(_T("3"), a1[2]);
		ASSERT_STREQ(_T("4"), a1[3]);
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, Insert)
{
	// <Test> 要素を挿入する。
	{
		Array<int> a1;
		a1.Add(1);
		a1.Add(2);
		a1.Insert(0, 3);
		a1.Insert(2, 4);
		a1.Insert(4, 5);
		ASSERT_EQ(3, a1[0]);
		ASSERT_EQ(1, a1[1]);
		ASSERT_EQ(4, a1[2]);
		ASSERT_EQ(2, a1[3]);
		ASSERT_EQ(5, a1[4]);
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, Remove)
{
	// ついでにコピーで元の配列が壊れないかも見てみる
	Array<int> ary;
	ary.Add(1); ary.Add(2); ary.Add(3); ary.Add(2);

	// <Test> Remove
	{
		Array<int> a1 = ary;
		a1.Remove(2);
		ASSERT_EQ(3, a1.GetCount());
		ASSERT_EQ(1, a1[0]);
		ASSERT_EQ(3, a1[1]);
		ASSERT_EQ(2, a1[2]);
	}
	// <Test> RemoveAt
	{
		Array<int> a1 = ary;
		a1.RemoveAt(2);
		ASSERT_EQ(3, a1.GetCount());
		ASSERT_EQ(1, a1[0]);
		ASSERT_EQ(2, a1[1]);
		ASSERT_EQ(2, a1[2]);
	}
	// <Test> RemoveAll
	{
		Array<int> a1 = ary;
		a1.RemoveAll(2);
		ASSERT_EQ(2, a1.GetCount());
		ASSERT_EQ(1, a1[0]);
		ASSERT_EQ(3, a1[1]);
	}
	// <Test> RemoveLast
	{
		Array<int> a1 = ary;
		a1.RemoveLast();
		ASSERT_EQ(3, a1.GetCount());
		ASSERT_EQ(1, a1[0]);
		ASSERT_EQ(2, a1[1]);
		ASSERT_EQ(3, a1[2]);
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, Reserve)
{
	// <Test> Reserve
	{
		Array<int> a1;
		a1.Reserve(2);
		ASSERT_EQ(2, a1.GetCapacity());
		a1.Add(1); a1.Add(2);
		ASSERT_EQ(2, a1.GetCapacity());
		a1.Add(3);
		ASSERT_LT(2, a1.GetCapacity());	// 2 < cap
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, Resize)
{
	// <Test> Resize
	{
		Array<int> a1;
		a1.Add(1);
		a1.Resize(2);
		ASSERT_EQ(2, a1.GetCount());
		ASSERT_EQ(0, a1[1]);
		a1[1] = 10;
		ASSERT_EQ(10, a1[1]);
		a1.Resize(1);
		ASSERT_EQ(1, a1.GetCount());
		ASSERT_EQ(1, a1[0]);
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, Contains)
{
	// <Test> Contains
	{
		PathNameA p1("LuminoCore/include/Lumino/Base/NonCopyable.h");
		PathNameA p2("LuminoCore/include/Lumino/Base/NonCopyable.h");
		PathNameA p3("LuminoCore/include/Lumino/Base/NonCopyable.c");
		Array<PathNameA> pathes;
		pathes.Add(p1);
		ASSERT_TRUE(pathes.Contains(p2));
		ASSERT_FALSE(pathes.Contains(p3));
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, IndexOf)
{
	// <Test> IndexOf
	{
		Array<int> ary;
		ary.Add(1); ary.Add(2); ary.Add(3); ary.Add(2);
		ASSERT_EQ(1, ary.IndexOf(2));
		ASSERT_EQ(1, ary.IndexOf(2, 1));
		ASSERT_EQ(3, ary.IndexOf(2, 2));
		ASSERT_EQ(-1, ary.IndexOf(10));
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, Front_Last)
{
	// <Test> GetFront, GetLast
	{
		Array<int> ary;
		ary.Add(1); ary.Add(2); ary.Add(3); ary.Add(4);
		ASSERT_EQ(1, ary.GetFront());
		ASSERT_EQ(4, ary.GetLast());

		const Array<int>& ary2 = ary;
		ASSERT_EQ(1, ary2.GetFront());
		ASSERT_EQ(4, ary2.GetLast());
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Array, Iterate)
{
	// <Test> 範囲 for で使用できる。
	{
		Array<int> a1;
		a1.Add(1); a1.Add(10); a1.Add(100);
		int r = 0;
		for (auto v : a1) {
			r += v;
		}
		ASSERT_EQ(r, 111);
	}
}
