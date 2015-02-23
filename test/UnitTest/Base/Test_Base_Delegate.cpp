#include <TestConfig.h>

class Test_Base_Delegate : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static int g_Value = 0;


//---------------------------------------------------------------------

class Test1		// GCC (Cygwin?) では関数内ローカルクラスのメンバ関数ポインタを取ろうとするとコンパイルエラーになるので、外に置いておく
{
public:

	int* mValue;

	void Callback1()
	{
		*mValue = 10;
	}
	void Callback2()
	{
		*mValue = 20;
	}
	static void LN_STDCALL Callback4()
	{
		g_Value = 40;
	}
} ;
	
TEST_F(Test_Base_Delegate, Delegate00)
{
	int value = 0;

	Test1 t1;

	t1.mValue = &value;

	// IsEmpty
	Delegate00 delegate0;
	ASSERT_TRUE(delegate0.IsEmpty());

	// Call
	Delegate00 delegate1(&t1, &Test1::Callback1);
	delegate1.Call();
	ASSERT_EQ(10, value);
	ASSERT_FALSE(delegate1.IsEmpty());

	// oeprator ()()
	Delegate00 delegate2(&t1, &Test1::Callback2);
	delegate2();
	ASSERT_EQ(20, value);

	// oeprator =
	Delegate00 delegate3;
	delegate3 = delegate1;
	delegate3();
	ASSERT_EQ(10, value);

	// static method
	Delegate00 delegate4(&Test1::Callback4);
	delegate4();
	ASSERT_EQ(40, g_Value);

	// copy
	Delegate00 delegate5(delegate2);
	delegate5();
	ASSERT_EQ(20, value);
}

//---------------------------------------------------------------------

class Test_Delegate01
{
public:
	int* mValue;

	void Callback1(int a)
	{
		*mValue = a;
	}
	static void LN_STDCALL Callback2(int a)
	{
		g_Value = a + 10;
	}
};
	
	
TEST_F(Test_Base_Delegate, Delegate01)
{
	int value = 0;

	Test_Delegate01 t1;
	t1.mValue = &value;

	// デフォルトコンストラクタ
	// IsEmpty
	Delegate01<int> d1;
	ASSERT_TRUE(d1.IsEmpty());

	// コンストラクタ (インスタンスメソッド)
	// Call
	Delegate01<int> d2(&t1, &Test_Delegate01::Callback1);
	d2.Call(1);
	ASSERT_FALSE(d2.IsEmpty());
	ASSERT_EQ(1, value);

	// コンストラクタ (static メソッド)
	// oeprator ()()
	Delegate01<int> d3(&Test_Delegate01::Callback2);
	d3(2);
	ASSERT_EQ(12, g_Value);

	// copy
	Delegate01<int> d4(d2);
	d4(5);
	ASSERT_EQ(5, value);

	// oeprator =
	Delegate01<int> d5;
	d5 = d3;
	d5(6);
	ASSERT_EQ(16, g_Value);
}
