#include <TestConfig.h>

class Test_Base_Enum : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};


#define LN_ENUM_CLASS(name) \
	class name \
	{ \
	public: \
		enum _##name

#define LN_ENUM_CLASS_DECLARE(name) \
	private: \
		int	m_value; \
	public: \
		typedef _##name enum_type; \
		name() : m_value(0) {} \
		name(enum_type v) : m_value(v) {} \
		bool operator==(name right) const { return m_value == right.m_value; } \
		bool operator==(enum_type right) const { return m_value == right; } \
		bool operator!=(name right) const { return !operator==(right); } \
		bool operator!=(enum_type right) const { return !operator==(right); } \
		friend inline bool operator==(name::enum_type left, name right) throw(); \
	}; \
	inline bool operator==(name::enum_type left, name right) throw() { return left == right.m_value; }

#define LN_ENUM_CLASS_FLAGS(name) \
	class name \
	{ \
	public: \
		enum _##name

#define LN_ENUM_CLASS_FLAGS_DECLARE(name) \
	private: \
		int	m_value; \
	public: \
		typedef _##name enum_type; \
		name() : m_value(0) {} \
		name(enum_type v) : m_value(v) {} \
		LN_CONSTEXPR inline bool TestFlag(enum_type f) const throw() { return (m_value & f) == f && (f != 0 || m_value == f); } \
		bool operator==(name right) const { return m_value == right.m_value; } \
		bool operator==(_##name right) const { return m_value == right; } \
		bool operator!=(name right) const { return !operator==(right); } \
		bool operator!=(_##name right) const { return !operator==(right); } \
		friend inline name operator&(const name& left, enum_type right) throw(); \
		friend inline name operator|(const name& left, enum_type right) throw(); \
	}; \
	inline name operator&(const name& left, name::enum_type right) throw() { name value; value.m_value = left.m_value & right; return value; } \
	inline name operator|(const name& left, name::enum_type right) throw() { name value; value.m_value = left.m_value | right; return value; } \
	inline name operator|(name::enum_type left, name::enum_type right) throw() { return name(left) | right; }



template < typename TEnumClass, typename TEnum >
struct EnumParser
{
	// LN_ENUM_CLASS_DECLARE マクロが非常に長くなるのを避けるため、部分的にクラス化した
public:
	struct Pair { String Name; TEnum Value; };
	static ArrayList<Pair>& GetMemberList()
	{
		static ArrayList<Pair> members; return members;	// ヘッダ include だけで済ますため、static 変数は関数内に閉じ込めておく
	}
	void Init(const TEnum* values, int valuesCount, const char* argNames)
	{
		ArrayList<Pair>& members = GetMemberList();
		String names = argNames;
		ArrayList<String> tokens = names.Split(_T(","));
		for (int i = 0; i < valuesCount; ++i)
		{
			Pair p;
			p.Name = tokens[i].Trim();
			p.Value = values[i];
			members.Add(p);
		}
	}
	static const TCHAR* ToString(int value)
	{
		ArrayList<Pair>& members = GetMemberList();
		for (int i = 0; i < members.GetCount(); ++i)
		{
			if (members[i].Value == value) {
				return members[i].Name.GetCStr();
			}
		}
		LN_ASSERT(0);
		return NULL;
	}
	static TEnumClass Parse(const TCHAR* str)
	{
		TestValues value;
		if (TryParse(str, &value)) {
			return value;
		}
		LN_THROW(0, ArgumentException);
		return TEnumClass();
	}
	static bool TryParse(const TCHAR* str, TEnumClass* outValue)
	{
		ArrayList<Pair>& members = GetMemberList();
		for (int i = 0; i < members.GetCount(); ++i)
		{
			if (members[i].Name == str)
			{
				*outValue = members[i].Value;
				return true;
			}
		}
		LN_THROW(0, ArgumentException);
		return false;
	}
};



LN_ENUM_CLASS(TestValues)
{
	ID1 = 0,
	ID2,
	ID3,
};

private:
	struct LocalEnumParser : public EnumParser <TestValues, _TestValues> { LocalEnumParser() { _TestValues values[] = { ID1, ID2, ID3 };  Init(values, LN_ARRAY_SIZE_OF(values), "ID1, ID2, ID3"); } };
	static LocalEnumParser& GetEnumParser() { static LocalEnumParser parser; return parser; }
public:
	const TCHAR* ToString() const { return GetEnumParser().ToString(m_value); }
	static TestValues Parse(const TCHAR* str) { return GetEnumParser().Parse(str); };
	static bool TryParse(const TCHAR* str, TestValues* outValue) { return GetEnumParser().TryParse(str, outValue); }
LN_ENUM_CLASS_DECLARE(TestValues);

LN_ENUM_CLASS_FLAGS(TestFlags)
{
	Option1 = 0x01,
	Option2 = 0x02,
	Option3 = 0x04,
};
LN_ENUM_CLASS_FLAGS_DECLARE(TestFlags);

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Enum, Basic)
{
	TestValues v1 = TestValues::ID2;
	//TestValues v0 = 10;		// error

	TestFlags flags1 = TestFlags::Option1 | TestFlags::Option2;
	//TestFlags flags0 = 0x01;	// error
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Enum, ToString)
{
	// <Test> enum 値を文字列に変換できること
	{
		TestValues v1 = TestValues::ID2;
		ASSERT_STREQ(_T("ID2"), v1.ToString());

		v1 = TestValues::ID1;
		ASSERT_STREQ(_T("ID1"), v1.ToString());
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Enum, Parse)
{
	// <Test> 文字列を enum 値に変換できること
	{
		TestValues v1 = TestValues::Parse(_T("ID2"));
		ASSERT_EQ(TestValues::ID2, v1);
	}
}

