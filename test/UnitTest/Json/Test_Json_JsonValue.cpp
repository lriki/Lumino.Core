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
	// <Test> JsonValue �̃f�t�H���g�l�� Null�B
	// <Test> IsNull() �� Null ���m�F����B
	// <Test> GetType() �� Null ���m�F����B
	{
		JsonValue v;
		ASSERT_TRUE(v.IsNull());
		ASSERT_EQ(JsonType::Null, v.GetType());
	}
	// <Test> SetNull() �� Null ��ݒ�B
	{
		JsonValue v;
		v.SetNull();
		ASSERT_TRUE(v.IsNull());
		ASSERT_EQ(JsonType::Null, v.GetType());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonValue, Bool)
{
	// <Test> SetBool() �� Bool �l��ݒ肷��B
	// <Test> IsBool() �� Bool �l���m�F����B
	// <Test> GetType() �� Bool �l���m�F����B
	{
		JsonValue v;
		v.SetBool(true);
		ASSERT_TRUE(v.IsBool());
		ASSERT_EQ(JsonType::Bool, v.GetType());
	}

	// <Test> SetBool() �� true ��ݒ肷��B
	// <Test> SetBool() �� false ��ݒ肷��B
	{
		JsonValue v;
		v.SetBool(true);
		ASSERT_TRUE(v.GetBool());
		v.SetBool(false);
		ASSERT_FALSE(v.GetBool());
	}

	// <Test> �R���X�g���N�^�� true ��ݒ肷��B
	// <Test> �R���X�g���N�^�� false ��ݒ肷��B
	{
		JsonValue v1(true);
		JsonValue v2(false);
		ASSERT_TRUE(v1.GetBool());
		ASSERT_FALSE(v2.GetBool());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonValue, Double)
{
	// <Test> �R���X�g���N�^�� Double �l��ݒ肷��B
	// <Test> SetDouble() �� Double �l��ݒ肷��B
	// <Test> GetDouble() �� Double �l���擾����B
	// <Test> IsDouble() �� Double �l���m�F����B
	// <Test> GetType() �� Double �l���m�F����B
	{
		JsonValue v1(1.0);
		JsonValue v2;
		v2.SetDouble(2.0);
		ASSERT_TRUE(v1.IsDouble());
		ASSERT_EQ(JsonType::Double, v1.GetType());
		ASSERT_EQ(1.0, v1.GetDouble());
		ASSERT_TRUE(v2.IsDouble());
		ASSERT_EQ(JsonType::Double, v2.GetType());
		ASSERT_EQ(2.0, v2.GetDouble());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonValue, String)
{
	// <Test> �R���X�g���N�^�� String �l��ݒ肷��B
	// <Test> SetString() �� String �l��ݒ肷��B
	// <Test> GetString() �� String �l���擾����B
	// <Test> IsString() �� String �l���m�F����B
	// <Test> GetType() �� String �l���m�F����B
	{
		JsonValue v1(_T("abc"));
		JsonValue v2;
		v2.SetString(String(_T("def")));
		ASSERT_TRUE(v1.IsString());
		ASSERT_EQ(JsonType::String, v1.GetType());
		ASSERT_EQ(_T("abc"), v1.GetString());
		ASSERT_TRUE(v2.IsString());
		ASSERT_EQ(JsonType::String, v2.GetType());
		ASSERT_EQ(_T("def"), v2.GetString());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonValue, Array)
{
	// <Test> SetArray() �ŋ�̔z��ɂȂ�B
	{
		JsonValue v1;
		v1.SetArray();
		ASSERT_TRUE(v1.IsArray());
		ASSERT_EQ(JsonType::Array, v1.GetType());
		ASSERT_EQ(0, v1.GetItemCount());
	}

	// <Test> �R���X�g���N�^�� Array �^�ŏ������ł���B
	// <Test> AddItem() �ŗv�f��ǉ��ł���B
	// <Test> GetItemCount() �ŗv�f�����擾�ł���B
	// <Test> operator[] �ŗv�f��ǂݏ����ł���B
	{
		JsonValue v1(JsonType::Array);
		JsonValue v2;
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
	// <Test> SetObject() �ŋ�̃I�u�W�F�N�g�ɂȂ�B
	{
		JsonValue v1;
		v1.SetObject();
		ASSERT_TRUE(v1.IsObject());
		ASSERT_EQ(JsonType::Object, v1.GetType());
		ASSERT_EQ(0, v1.GetMemberCount());
	}

	// <Test> �R���X�g���N�^�� Object �^�ŏ������ł���B
	// <Test> AddMember() �ŗv�f��ǉ��ł���B
	// <Test> GetMemberCount() �ŗv�f�����擾�ł���B
	// <Test> operator[] �ŗv�f��ǂݏ����ł���B
	{
		//JsonValue v1(JsonType::Object);
		//JsonValue v2;
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
