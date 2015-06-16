#include <TestConfig.h>

class Test_Base_Enum : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

// C++ operators
// https://en.wikipedia.org/wiki/Operators_in_C_and_C++

class Enum
{
protected:

	template<typename TEnum>
	struct EnumParser
	{
		// LN_ENUM_DECLARE マクロが非常に長くなるのを避けるため、部分的にクラス化した
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
		static String ToString(int value)
		{
			ArrayList<Pair>& members = GetMemberList();
			for (int i = 0; i < members.GetCount(); ++i)
			{
				if (members[i].Value == value) {
					return members[i].Name;
				}
			}
			LN_ASSERT(0);
			return String();
		}
		static TEnum Parse(const TCHAR* str)
		{
			int value;
			if (TryParse(str, &value)) {
				return (TEnum)value;
			}
			LN_THROW(0, ArgumentException);
			return TEnum();
		}
		static bool TryParse(const TCHAR* str, int* outValue)
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
			return false;
		}
	};

	template<typename TEnum>
	struct EnumFlagsParser : public EnumParser<TEnum>
	{
	public:

		static String ToString(int value)
		{
			ArrayList<Pair>& members = GetMemberList();
			for (int i = 0; i < members.GetCount(); ++i)
			{
				if (members[i].Value == value) {
					return members[i].Name;
				}
			}
			LN_ASSERT(0);
			return String();
		}
		static TEnum Parse(const TCHAR* str)
		{
			int value;
			if (TryParse(str, &value)) {
				return (TEnum)value;
			}
			LN_THROW(0, ArgumentException);
			return TEnum();
		}
		static bool TryParse(const TCHAR* str, int* outValue)
		{
			//ArrayList<Pair>& members = GetMemberList();
			//for (int i = 0; i < members.GetCount(); ++i)
			//{
			//	if (members[i].Name == str)
			//	{
			//		*outValue = members[i].Value;
			//		return true;
			//	}
			//}
			//return false;
		}

	};
};


#define LN_ENUM(name) \
	class name : public Enum \
	{ \
	public: \
		enum _##name

#define LN_ENUM_DECLARE(name) \
	private: \
		int	m_value; \
	public: \
		typedef _##name enum_type; \
		name() : m_value(0) {} \
		name(enum_type v) : m_value(v) {} \
		inline operator int() const { return m_value; } \
		inline bool operator==(name right) const { return m_value == right.m_value; } \
		inline bool operator==(enum_type right) const { return m_value == right; } \
		inline bool operator!=(name right) const { return !operator==(right); } \
		inline bool operator!=(enum_type right) const { return !operator==(right); } \
		friend inline bool operator==(name::enum_type left, name right) throw(); \
		friend inline bool operator!=(name::enum_type left, name right) throw(); \
	}; \
	inline bool operator==(name::enum_type left, name right) throw() { return left == right.m_value; } \
	inline bool operator!=(name::enum_type left, name right) throw() { return left != right.m_value; }

#define LN_ENUM_REFLECTION(enumName, ...) \
private: \
	typedef _##enumName enum_type; \
	struct LocalEnumParser : public EnumParser <enum_type> { LocalEnumParser() { enumName::enum_type values[] = { __VA_ARGS__ };  Init(values, LN_ARRAY_SIZE_OF(values), #__VA_ARGS__); } }; \
	static LocalEnumParser& GetEnumParser() { static LocalEnumParser parser; return parser; } \
public: \
	String ToString() const { return GetEnumParser().ToString(m_value); } \
	static enumName Parse(const TCHAR* str) { return GetEnumParser().Parse(str); }; \
	static bool TryParse(const TCHAR* str, enumName* outValue) { return GetEnumParser().TryParse(str, (outValue) ? &outValue->m_value : NULL); }


#define LN_ENUM_FLAGS(name) \
	class name : public Enum \
	{ \
	public: \
		enum _##name

#define LN_ENUM_FLAGS_DECLARE(name) \
	private: \
		int	m_value; \
	public: \
		typedef _##name enum_type; \
		name() : m_value(0) {} \
		name(enum_type v) : m_value(v) {} \
		inline bool TestFlag(enum_type f) const throw() { return (m_value & f) == f && (f != 0 || m_value == f); } \
		inline operator int() const { return m_value; } \
		inline bool operator==(name right) const { return m_value == right.m_value; } \
		inline bool operator==(_##name right) const { return m_value == right; } \
		inline bool operator!=(name right) const { return !operator==(right); } \
		inline bool operator!=(_##name right) const { return !operator==(right); } \
		inline name& operator &= (const name& v) { m_value &= v.m_value; return *this; } \
		inline name& operator &= (enum_type v) { m_value &= v; return *this; } \
		inline name& operator |= (const name& v) { m_value |= v.m_value; return *this; } \
		inline name& operator |= (enum_type v) { m_value |= v; return *this; } \
		inline name operator~() const throw() { name value; value.m_value = ~m_value; return value; } \
		friend inline bool operator==(name::enum_type left, name right) throw(); \
		friend inline bool operator!=(name::enum_type left, name right) throw(); \
		friend inline name operator&(const name& left, const name& right) throw(); \
		friend inline name operator&(const name& left, enum_type right) throw(); \
		friend inline name operator&(enum_type left, const name& right) throw(); \
		friend inline name operator&(enum_type left, enum_type right) throw(); \
		friend inline name operator|(const name& left, const name& right) throw(); \
		friend inline name operator|(const name& left, enum_type right) throw(); \
		friend inline name operator|(enum_type left, const name& right) throw(); \
		friend inline name operator|(enum_type left, enum_type right) throw(); \
		friend inline name operator~(name::enum_type v) throw(); \
	}; \
	inline bool operator==(name::enum_type left, name right) throw() { return left == right.m_value; } \
	inline bool operator!=(name::enum_type left, name right) throw() { return left != right.m_value; } \
	inline name operator&(const name& left, const name& right) throw()		{ name value; value.m_value = left.m_value & right.m_value; return value; } \
	inline name operator&(const name& left, name::enum_type right) throw()	{ name value; value.m_value = left.m_value & right; return value; } \
	inline name operator&(name::enum_type left, const name& right) throw()	{ name value; value.m_value = right.m_value & left;  return value; } \
	inline name operator&(name::enum_type left, name::enum_type right) throw() { return name(left) & right; } \
	inline name operator|(const name& left, const name& right) throw()		{ name value; value.m_value = left.m_value | right.m_value; return value; } \
	inline name operator|(const name& left, name::enum_type right) throw()	{ name value; value.m_value = left.m_value | right; return value; } \
	inline name operator|(name::enum_type left, const name& right) throw()	{ name value; value.m_value = right.m_value | left;  return value; } \
	inline name operator|(name::enum_type left, name::enum_type right) throw() { return name(left) | right; } \
	inline name operator~(name::enum_type v) throw() { name value; value.m_value = (~((int)v)); return value; }

/*
name& operator &= (int v) { m_value &= v; return *this; } \
name& operator &= (uint32_t v) { m_value &= v; return *this; } \
*/

LN_ENUM(TestValues0)
{
	Value1 = 100,
	Value2,
	Value3,
};
LN_ENUM_DECLARE(TestValues0);

LN_ENUM(TestValues)
{
	ID1 = 0,
	ID2,
	ID3 = TestValues0::Value3,
};
LN_ENUM_REFLECTION(TestValues, ID1, ID2, ID3);
LN_ENUM_DECLARE(TestValues);

LN_ENUM_FLAGS(TestFlags)
{
	Option1 = 0x01,
	Option2 = 0x02,
	Option3 = 0x04,
};
LN_ENUM_FLAGS_DECLARE(TestFlags);

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Enum, Basic)
{
	TestValues v1 = TestValues::ID2;
	//TestValues v0 = 10;		// error

	TestFlags flags1 = TestFlags::Option1 | TestFlags::Option2;
	//TestFlags flags0 = 0x01;	// error
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Enum, ValuesOperators)
{
	// <Test> 初期化できる。
	{
		TestValues v1 = TestValues::ID1;
		TestValues v2(TestValues::ID1);
		ASSERT_EQ(TestValues::ID1, v1);
		ASSERT_EQ(TestValues::ID1, v2);
	}
	// <Test> operator =
	{
		TestValues v1;
		v1 = TestValues::ID1;
		ASSERT_EQ(TestValues::ID1, v1);
	}
	// <Test> operator ==
	{
		TestValues v1 = TestValues::ID1;
		TestValues v2 = TestValues::ID2;
		ASSERT_TRUE(v1 == TestValues::ID1);
		ASSERT_FALSE(v1 == TestValues::ID2);
		ASSERT_TRUE(TestValues::ID1 == v1);
		ASSERT_FALSE(TestValues::ID2 == v1);
		ASSERT_TRUE(v1 == v1);
		ASSERT_FALSE(v2 == v1);
		ASSERT_TRUE(v1 == 0);	// int と比較できる
	}
	// <Test> operator !=
	{
		TestValues v1 = TestValues::ID1;
		TestValues v2 = TestValues::ID2;
		ASSERT_FALSE(v1 != TestValues::ID1);
		ASSERT_TRUE(v1 != TestValues::ID2);
		ASSERT_FALSE(TestValues::ID1 != v1);
		ASSERT_TRUE(TestValues::ID2 != v1);
		ASSERT_FALSE(v1 != v1);
		ASSERT_TRUE(v2 != v1);
		ASSERT_FALSE(v1 != 0);	// int と比較できる
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Enum, ToString)
{
	// <Test> enum 値を文字列に変換できる。
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
	// <Test> 文字列を enum 値に変換できる。
	{
		TestValues v1 = TestValues::Parse(_T("ID2"));
		ASSERT_EQ(TestValues::ID2, v1);
	}
	// <Test> 別の enum の値でメンバの値を設定できる。
	{
		TestValues v1 = TestValues::Parse(_T("ID3"));
		ASSERT_EQ(102, v1);
	}
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_Enum, FlagsOperators)
{
	// <Test> 初期化できる。
	{
		TestFlags v1 = TestFlags::Option1;
		TestFlags v2(TestFlags::Option2);
		ASSERT_EQ(TestFlags::Option1, v1);
		ASSERT_EQ(TestFlags::Option2, v2);
	}
	// <Test> operator =
	{
		TestFlags v1;
		v1 = TestFlags::Option1;
		ASSERT_EQ(TestFlags::Option1, v1);
	}
	// <Test> operator ==
	{
		TestFlags v1 = TestFlags::Option1;
		TestFlags v2 = TestFlags::Option2;
		ASSERT_TRUE(v1 == TestFlags::Option1);
		ASSERT_FALSE(v1 == TestFlags::Option2);
		ASSERT_TRUE(TestFlags::Option1 == v1);
		ASSERT_FALSE(TestFlags::Option2 == v1);
		ASSERT_TRUE(v1 == v1);
		ASSERT_FALSE(v2 == v1);
	}
	// <Test> operator !=
	{
		TestFlags v1 = TestFlags::Option1;
		TestFlags v2 = TestFlags::Option2;
		ASSERT_FALSE(v1 != TestFlags::Option1);
		ASSERT_TRUE(v1 != TestFlags::Option2);
		ASSERT_FALSE(TestFlags::Option1 != v1);
		ASSERT_TRUE(TestFlags::Option2 != v1);
		ASSERT_FALSE(v1 != v1);
		ASSERT_TRUE(v2 != v1);
	}
	// <Test> operator |
	{
		TestFlags v1;
		TestFlags v2 = TestFlags::Option2;
		v1 = TestFlags::Option1 | TestFlags::Option2;
		ASSERT_EQ(TestFlags::Option1 | TestFlags::Option2, v1);

		v1 = TestFlags::Option1;
		v1 = v1 | v2;
		ASSERT_EQ(TestFlags::Option1 | TestFlags::Option2, v1);

		// どちらの辺でもOK
		v1 = TestFlags::Option1;
		v1 = v1 | TestFlags::Option2;
		v1 = TestFlags::Option3 | v1;
		ASSERT_EQ(TestFlags::Option1 | TestFlags::Option2 | TestFlags::Option3, v1);
	}
	// <Test> operator &
	{
		TestFlags v1;
		v1 = TestFlags::Option1 & TestFlags::Option2;
		ASSERT_EQ(0, v1);

		TestFlags v2 = TestFlags::Option1;
		v1 = TestFlags::Option1;
		v1 = v1 & v2;
		ASSERT_EQ(TestFlags::Option1, v1);

		// どちらの辺でもOK
		v1 = TestFlags::Option1 | TestFlags::Option2 | TestFlags::Option3;
		v1 = v1 & TestFlags::Option2;
		v1 = TestFlags::Option3 & v1;
		ASSERT_EQ(0, v1);
	}
	// <Test> operator &=
	{
		TestFlags v1 = TestFlags::Option1 | TestFlags::Option2;
		v1 &= TestFlags::Option1;
		ASSERT_EQ(TestFlags::Option1, v1);

		TestFlags v2 = TestFlags::Option2;
		v1 &= v2;
		ASSERT_EQ(0, v1);
	}
	// <Test> operator |=
	{
		TestFlags v1;
		v1 |= TestFlags::Option1;
		ASSERT_EQ(TestFlags::Option1, v1);

		TestFlags v2 = TestFlags::Option2;
		v1 |= v2;
		ASSERT_EQ(TestFlags::Option1 | TestFlags::Option2, v1);
	}
	// <Test> &= ~ でフラグを落とす。
	{
		TestFlags v1 = TestFlags::Option1 | TestFlags::Option2;

		v1 &= ~TestFlags::Option3;
		ASSERT_EQ(TestFlags::Option1 | TestFlags::Option2, v1);	// 変化無し

		v1 &= ~TestFlags::Option2;
		ASSERT_EQ(TestFlags::Option1, v1);	// Option2 が消えている

		v1 &= ~v1;							// 自分自身で落としてみる
		ASSERT_EQ(0, v1);					// 全部消えている
	}
}
