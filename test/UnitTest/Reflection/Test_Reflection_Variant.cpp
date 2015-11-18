#include <TestConfig.h>
#include <Lumino/Reflection/ReflectionObject.h>
#include <Lumino/Reflection/Variant.h>
using namespace ln::tr;

class IntegrationTest_Reflection_Variant : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

LN_ENUM(VariantTestEnum1)
{
	Value0,
	Value1,
};
LN_ENUM_DECLARE(VariantTestEnum1);

//---------------------------------------------------------------------
TEST_F(IntegrationTest_Reflection_Variant, Basic)
{
	// <Test> Bool 型
	{
		Variant v1(true);
		Variant v2(false);
		ASSERT_EQ(VariantType::Bool, v1.GetType());
		ASSERT_EQ(VariantType::Bool, v2.GetType());
		ASSERT_EQ(true, Variant::Cast<bool>(v1));
		ASSERT_EQ(false, Variant::Cast<bool>(v2));
	}
	// <Test> Enum 型
	{
		VariantTestEnum1 e1 = VariantTestEnum1::Value1;
		Variant v1(e1);							// Enum クラス派生
		Variant v2(VariantTestEnum1::Value0);	// enum 直値指定
		ASSERT_EQ(VariantType::Enum, v1.GetType());
		ASSERT_EQ(VariantType::Enum, v2.GetType());
		ASSERT_EQ(VariantTestEnum1::Value1, Variant::Cast<VariantTestEnum1>(v1));
		ASSERT_EQ(VariantTestEnum1::Value0, Variant::Cast<VariantTestEnum1>(v2));
	}

	//CPINFO info;
	//Variant ve1(info);
}
