#include <TestConfig.h>
#include <Lumino/Reflection/ReflectionObject.h>
#include <Lumino/Reflection/ReflectionArrayObject.h>
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

class RefTest1 : public ReflectionObject
{
};
class ArrayTest1 : public ReflectionArrayObject
{
};


//---------------------------------------------------------------------
TEST_F(IntegrationTest_Reflection_Variant, Basic)
{
	// <Test> Bool 型
	{
		Variant v1(true);
		Variant v2(false);
		Variant v3 = v2;	// copy
		ASSERT_EQ(VariantType::Bool, v1.GetType());
		ASSERT_EQ(VariantType::Bool, v2.GetType());
		ASSERT_EQ(VariantType::Bool, v3.GetType());
		ASSERT_EQ(true, Variant::Cast<bool>(v1));
		ASSERT_EQ(false, Variant::Cast<bool>(v2));
		ASSERT_EQ(false, Variant::Cast<bool>(v3));
	}
	// <Test> Enum 型
	{
		VariantTestEnum1 e1 = VariantTestEnum1::Value1;
		Variant v1(e1);							// Enum クラス派生
		Variant v2(VariantTestEnum1::Value0);	// enum 直値指定
		Variant v3 = v2;	// copy
		ASSERT_EQ(VariantType::Enum, v1.GetType());
		ASSERT_EQ(VariantType::Enum, v2.GetType());
		ASSERT_EQ(VariantType::Enum, v3.GetType());
		ASSERT_EQ(VariantTestEnum1::Value1, Variant::Cast<VariantTestEnum1>(v1));
		ASSERT_EQ(VariantTestEnum1::Value0, Variant::Cast<VariantTestEnum1>(v2));
		ASSERT_EQ(VariantTestEnum1::Value0, Variant::Cast<VariantTestEnum1>(v3));
	}
	// <Test> ReflectionObject 型
	{
		RefTest1* obj1 = new RefTest1();
		Variant v1(obj1);
		Variant v2 = v1;	// copy
		ASSERT_EQ(VariantType::Object, v1.GetType());
		ASSERT_EQ(VariantType::Object, v2.GetType());
		ASSERT_EQ(obj1, Variant::Cast<RefTest1*>(v1));
		ASSERT_EQ(obj1, Variant::Cast<ReflectionObject*>(v1));
		ASSERT_EQ(obj1, Variant::Cast<RefTest1*>(v2));
		obj1->Release();
	}
	// <Test> RefPtr 型
	{
		RefPtr<RefTest1> obj1(new RefTest1(), false);
		Variant v1(obj1);
		Variant v2 = v1;	// copy
		ASSERT_EQ(VariantType::Object, v1.GetType());
		ASSERT_EQ(VariantType::Object, v2.GetType());
		ASSERT_EQ(obj1.GetObjectPtr(), Variant::Cast<RefTest1*>(v1));
		ASSERT_EQ(obj1.GetObjectPtr(), Variant::Cast<ReflectionObject*>(v1));
		ASSERT_EQ(obj1.GetObjectPtr(), Variant::Cast<RefTest1*>(v2));
	}
	// <Test> ReflectionArrayObject 型
	{
		ReflectionArrayObject* obj1 = new ReflectionArrayObject();
		Variant v1(obj1);
		Variant v2 = v1;	// copy
		ASSERT_EQ(VariantType::ArrayObject, v1.GetType());
		ASSERT_EQ(VariantType::ArrayObject, v2.GetType());
		ASSERT_EQ(obj1, Variant::Cast<ArrayTest1*>(v1));
		ASSERT_EQ(obj1, Variant::Cast<ReflectionArrayObject*>(v1));
		ASSERT_EQ(obj1, Variant::Cast<ReflectionObject*>(v1));
		ASSERT_EQ(obj1, Variant::Cast<ArrayTest1*>(v2));
		obj1->Release();
	}
}
