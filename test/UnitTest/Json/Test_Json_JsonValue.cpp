#include <TestConfig.h>

class Test_Json_JsonValue : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonValue, Null)
{
	// <Test> JsonValue のデフォルト値は Null。
	// <Test> IsNull() で Null を確認する。
	// <Test> GetType() で Null を確認する。
	{
		Json::JsonValue v;
		ASSERT_TRUE(v.IsNull());
		ASSERT_EQ(Json::JsonType::Null, v.GetType());
	}
	// <Test> SetNull() で Null を設定。
	{
		Json::JsonValue v;
		v.SetNull();
		ASSERT_TRUE(v.IsNull());
		ASSERT_EQ(Json::JsonType::Null, v.GetType());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonValue, Bool)
{
	// <Test> SetBool() で Bool 値を設定する。
	// <Test> IsBool() で Bool 値を確認する。
	// <Test> GetType() で Bool 値を確認する。
	{
		Json::JsonValue v;
		v.SetBool(true);
		ASSERT_TRUE(v.IsBool());
		ASSERT_EQ(Json::JsonType::Bool, v.GetType());
	}

	// <Test> SetBool() で true を設定する。
	// <Test> SetBool() で false を設定する。
	{
		Json::JsonValue v;
		v.SetBool(true);
		ASSERT_TRUE(v.GetBool());
		v.SetBool(false);
		ASSERT_FALSE(v.GetBool());
	}

	// <Test> コンストラクタで true を設定する。
	// <Test> コンストラクタで false を設定する。
	{
		Json::JsonValue v1(true);
		Json::JsonValue v2(false);
		ASSERT_TRUE(v1.GetBool());
		ASSERT_FALSE(v2.GetBool());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonValue, Double)
{
	// <Test> コンストラクタで Double 値を設定する。
	// <Test> SetDouble() で Double 値を設定する。
	// <Test> GetDouble() で Double 値を取得する。
	// <Test> IsDouble() で Double 値を確認する。
	// <Test> GetType() で Double 値を確認する。
	{
		Json::JsonValue v1(1.0);
		Json::JsonValue v2;
		v2.SetDouble(2.0);
		ASSERT_TRUE(v1.IsDouble());
		ASSERT_EQ(Json::JsonType::Double, v1.GetType());
		ASSERT_EQ(1.0, v1.GetDouble());
		ASSERT_TRUE(v2.IsDouble());
		ASSERT_EQ(Json::JsonType::Double, v2.GetType());
		ASSERT_EQ(2.0, v2.GetDouble());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonValue, String)
{
	// <Test> コンストラクタで String 値を設定する。
	// <Test> SetString() で String 値を設定する。
	// <Test> GetString() で String 値を取得する。
	// <Test> IsString() で String 値を確認する。
	// <Test> GetType() で String 値を確認する。
	{
		Json::JsonValue v1(_T("abc"));
		Json::JsonValue v2;
		v2.SetString(String(_T("def")));
		ASSERT_TRUE(v1.IsString());
		ASSERT_EQ(Json::JsonType::String, v1.GetType());
		ASSERT_STREQ(_T("abc"), v1.GetString());
		ASSERT_TRUE(v2.IsString());
		ASSERT_EQ(Json::JsonType::String, v2.GetType());
		ASSERT_STREQ(_T("def"), v2.GetString());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonValue, Array)
{
	// <Test> SetArray() で空の配列になる。
	{
		Json::JsonValue v1;
		v1.SetArray();
		ASSERT_TRUE(v1.IsArray());
		ASSERT_EQ(Json::JsonType::Array, v1.GetType());
		ASSERT_EQ(0, v1.GetItemCount());
	}

	// <Test> コンストラクタで Array 型で初期化できる。
	// <Test> AddItem() で要素を追加できる。
	// <Test> GetItemCount() で要素数を取得できる。
	// <Test> operator[] で要素を読み書きできる。
	{
		Json::JsonValue v1(Json::JsonType::Array);
		Json::JsonValue v2;
		v2.SetBool(true);  v1.AddItem(v2);
		v2.SetBool(false); v1.AddItem(v2);
		v2.SetBool(false); v1.AddItem(v2);
		v1[2].SetBool(true);
		ASSERT_EQ(3, v1.GetItemCount());
		ASSERT_TRUE(v1[0].GetBool());
		ASSERT_FALSE(v1[1].GetBool());
		ASSERT_TRUE(v1[2].GetBool());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonValue, Object)
{
	// <Test> SetObject() で空のオブジェクトになる。
	{
		Json::JsonValue v1;
		v1.SetObject();
		ASSERT_TRUE(v1.IsObject());
		ASSERT_EQ(Json::JsonType::Object, v1.GetType());
		ASSERT_EQ(0, v1.GetMemberCount());
	}

	// <Test> コンストラクタで Object 型で初期化できる。
	// <Test> AddMember() で要素を追加できる。
	// <Test> GetMemberCount() で要素数を取得できる。
	// <Test> operator[] で要素を読み書きできる。
	{
		//Json::JsonValue v1(Json::JsonType::Object);
		//Json::JsonValue v2;
		//v2.SetBool(true);  v1.AddMember(_T("a1"), v2);
		//v2.SetBool(false); v1.AddMember(_T("a2"), v2);
		//v2.SetBool(false); v1.AddMember(_T("a3"), v2);
		//v1[2].SetBool(true);
		//ASSERT_EQ(3, v1.GetItemCount());
		//ASSERT_TRUE(v1[0].GetBool());
		//ASSERT_FALSE(v1[1].GetBool());
		//ASSERT_TRUE(v1[2].GetBool());
	}

#if 0


	String lnstr1 = _T("123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
	std::wstring stdstr1 = _T("123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");



	// copy
	printf("----\n");
	for (int j = 0; j < 10; j++)
	{
		ElapsedTimer timer;
		timer.Start();

		for (int i = 0; i < 100000; ++i)
		{
			const String str = lnstr1;
		}
		printf("t:%llu\n", timer.GetElapsedTime());
	}
	printf("----\n");
	for (int j = 0; j < 10; j++)
	{
		ElapsedTimer timer;
		timer.Start();

		for (int i = 0; i < 100000; ++i)
		{
			const std::wstring str = stdstr1;
		}
		printf("t:%llu\n", timer.GetElapsedTime());
	}
#endif
}
