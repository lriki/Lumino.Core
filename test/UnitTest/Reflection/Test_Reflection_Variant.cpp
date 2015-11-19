﻿#include <TestConfig.h>
#include <Lumino/Reflection/ReflectionObject.h>
#include <Lumino/Reflection/ReflectionArrayObject.h>
#include <Lumino/Reflection/ReflectionStruct.h>
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
struct StructTest1// : public ReflectionStruct
{
	int a, b;
};


struct TestPoint
{
	//TestPoint(int ix = 0, int iy = 0) : x(ix), y(iy) {}
	int x, y;
};
//---------------------------------------------------------------------
TEST_F(IntegrationTest_Reflection_Variant, Basic)
{
#if 0	// 速度テスト
	for (int ffi = 0; ffi < 10; ++ffi)
	{
		ln::ElapsedTimer timer;
		timer.Start();
		int f = 0;
		for (int i = 0; i < 1000000; ++i)
		{
			TestPoint st;
			st.x = 10;
			st.y = 20;
			Variant a(st);
			f += Variant::Cast<TestPoint>(a).x;
		}
		printf("%d : %lld\n", f, timer.GetElapsedTimeNS());
	}
#endif
#if 0
	for (int ffi = 0; ffi < 10; ++ffi)
	{
		ln::ElapsedTimer timer;
		timer.Start();
		int f = 0;
		for (int i = 0; i < 1000000; ++i)
		{
			Point st;
			st.x = 10;
			st.y = 20;
			boost::any a(st);
			f += boost::any_cast<Point>(a).x;
		}
		printf("%d : %lld\n", f, timer.GetElapsedTimeNS());
	}
#endif


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
	// <Test> TCHAR* 型
	{
		Variant v1(_T("str"));
		Variant v2 = v1;	// copy
		ASSERT_EQ(VariantType::String, v1.GetType());
		ASSERT_EQ(VariantType::String, v2.GetType());
		ASSERT_EQ(_T("str"), Variant::Cast<String>(v1));
		ASSERT_EQ(_T("str"), Variant::Cast<String>(v2));
	}
	// <Test> String 型
	{
		Variant v1(String(_T("str")));
		Variant v2 = v1;	// copy
		ASSERT_EQ(VariantType::String, v1.GetType());
		ASSERT_EQ(VariantType::String, v2.GetType());
		ASSERT_EQ(_T("str"), Variant::Cast<String>(v1));
		ASSERT_EQ(_T("str"), Variant::Cast<String>(v2));
		//ASSERT_EQ(_T("str"), Variant::Cast<const String&>(v2));	// 参照文字列型はコンパイルエラーにする (static_assert)
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
	// <Test> 構造体 (POD)
	{
		StructTest1 st;
		st.a = 10; st.b = 20;
		Variant v1(st);
		Variant v2 = v1;	// copy
		ASSERT_EQ(VariantType::Struct, v1.GetType());
		ASSERT_EQ(VariantType::Struct, v2.GetType());
		ASSERT_EQ(10, Variant::Cast<StructTest1>(v1).a);
		ASSERT_EQ(20, Variant::Cast<StructTest1>(v2).b);
		ASSERT_EQ(10, Variant::Cast<const StructTest1&>(v2).a);	// 参照で取り出せる
	}
	// <Test> 構造体 (非POD)
	{
		Point st(10, 20);
		Variant v1(st);
		Variant v2 = v1;	// copy
		Variant v3(Point(30, 40));
		ASSERT_EQ(VariantType::Struct, v1.GetType());
		ASSERT_EQ(VariantType::Struct, v2.GetType());
		ASSERT_EQ(VariantType::Struct, v3.GetType());
		ASSERT_EQ(10, Variant::Cast<Point>(v1).X);
		ASSERT_EQ(20, Variant::Cast<Point>(v2).Y);
		ASSERT_EQ(10, Variant::Cast<const Point&>(v2).X);	// 参照で取り出せる
		ASSERT_EQ(30, Variant::Cast<Point>(v3).X);
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
