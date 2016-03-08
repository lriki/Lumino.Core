#include <TestConfig.h>
#include <Lumino/Reflection/Notify.h>
#include <Lumino/Reflection/ReflectionEventArgs.h>
#include <Lumino/Reflection/ReflectionObject.h>
#include <Lumino/Reflection/Property.h>

class IntegrationTest_Reflection_ReflectionObject : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

class RefTest1 : public tr::ReflectionObject
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:
};
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(RefTest1, tr::ReflectionObject);

class RefTest2 : public RefTest1
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:
};
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(RefTest2, tr::ReflectionObject);

class EventTest1 : public tr::ReflectionObject
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:
	LN_REFLECTION_EVENT(tr::ReflectionEventArgs, ClickedEvent);
public:
	tr::ReflectionEvent<tr::ReflectionEventArgs>	Clicked;
	tr::DelegateEvent<int>							Clicked2;

	void OnClicked() { tr::ReflectionEventArgs e; RaiseReflectionEvent(Clicked, &e); }
	void OnClicked2(int a) { RaiseDelegateEvent(Clicked2, a); }
};
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(EventTest1, tr::ReflectionObject);
LN_REFLECTION_EVENT_IMPLEMENT(EventTest1, tr::ReflectionEventArgs, ClickedEvent, "Clicked", Clicked);

//---------------------------------------------------------------------
TEST_F(IntegrationTest_Reflection_ReflectionObject, Basic)
{
	RefTest1 t1;
	RefTest2 t2;

	auto type1 = tr::TypeInfo::GetTypeInfo(&t1);
	ASSERT_TRUE(type1->GetName().IndexOf(_T("RefTest1")) >= 0);

	auto type2 = tr::TypeInfo::GetTypeInfo(&t2);
	ASSERT_TRUE(type2->GetName().IndexOf(_T("RefTest2")) >= 0);

	tr::TypeInfo::GetTypeInfo(&t1)->SetBindingTypeInfo((void*)1);
	tr::TypeInfo::GetTypeInfo(&t2)->SetBindingTypeInfo((void*)2);

	ASSERT_EQ(1, (intptr_t)tr::TypeInfo::GetBindingTypeInfo(&t1));
	ASSERT_EQ(2, (intptr_t)tr::TypeInfo::GetBindingTypeInfo(&t2));

}

//---------------------------------------------------------------------
TEST_F(IntegrationTest_Reflection_ReflectionObject, Event)
{
	int count = 0;
	EventTest1 t1;
	t1.Clicked += [&count](tr::ReflectionEventArgs* e) { count++; };
	t1.OnClicked();
	t1.OnClicked();
	ASSERT_EQ(2, count);
}

//---------------------------------------------------------------------
TEST_F(IntegrationTest_Reflection_ReflectionObject, DelegateEvent)
{
	int count = 0;
	EventTest1 t1;
	t1.Clicked2 += [&count](int a) { count += a;};
	t1.OnClicked2(1);
	t1.OnClicked2(2);
	ASSERT_EQ(3, count);
}

//---------------------------------------------------------------------
class PropertyTest1 : public tr::ReflectionObject
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:
	LN_TR_PROPERTY(int, V1Property);
	LN_TR_PROPERTY(RefTest1*, V2Property);
	LN_TR_PROPERTY(Point, V3Property);
	LN_TR_PROPERTY(RefTest2*, V4Property);

public:
	void SetV1(int v) { return tr::Property::SetPropertyValueDirect<int>(this, V1Property, v); }
	int GetV1() const { return tr::Property::GetPropertyValueDirect<int>(this, V1Property); }

	void SetV2(RefTest1* v) { return tr::Property::SetPropertyValueDirect<RefTest1*>(this, V2Property, v); }
	RefTest1* GetV2() const { return tr::Property::GetPropertyValueDirect<RefTest1*>(this, V2Property); }

	void SetV3(const Point& v) { return tr::Property::SetPropertyValueDirect<Point>(this, V3Property, v); }
	const Point& GetV3() const { return tr::Property::GetPropertyValueDirect<Point>(this, V3Property); }

	void SetV4(RefTest2* v) { return tr::Property::SetPropertyValueDirect<RefTest2*>(this, V4Property, v); }
	RefTest2* GetV4() const { return tr::Property::GetPropertyValueDirect<RefTest2*>(this, V4Property); }

public:
	int m_v1 = 0;
	RefTest1* m_v2 = nullptr;
	Point m_v3;
	RefPtr<RefTest2> m_v4;
};
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(PropertyTest1, tr::ReflectionObject);
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, int, V1Property, "V1", m_v1, tr::PropertyMetadata());
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, RefTest1*, V2Property, "V2", m_v2, tr::PropertyMetadata());
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, Point, V3Property, "V3", m_v3, tr::PropertyMetadata());
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, RefTest2*, V4Property, "V4", m_v4, tr::PropertyMetadata());


TEST_F(IntegrationTest_Reflection_ReflectionObject, Property)
{
	PropertyTest1 t1;

	// <Test> 値型
	{
		// SetPropertyValue/GetPropertyValue
		tr::Property::SetPropertyValue(&t1, PropertyTest1::V1Property, 100);
		ASSERT_EQ(100, t1.m_v1);
		tr::Variant v = tr::Property::GetPropertyValue(&t1, PropertyTest1::V1Property);
		ASSERT_EQ(100, tr::Variant::Cast<int>(v));

		// アクセサメンバ関数
		t1.SetV1(200);
		ASSERT_EQ(200, t1.GetV1());
	}

	// <Test> ポインタ型
	{
		RefTest1 rt1;
		tr::Property::SetPropertyValue(&t1, PropertyTest1::V2Property, &rt1);
		ASSERT_EQ(&rt1, t1.m_v2);
		tr::Variant v = tr::Property::GetPropertyValue(&t1, PropertyTest1::V2Property);
		ASSERT_EQ(&rt1, tr::Variant::Cast<RefTest1*>(v));

		RefTest1 rt2;
		t1.SetV2(&rt2);
		ASSERT_EQ(&rt2, t1.GetV2());

		t1.SetV2(nullptr);
		ASSERT_EQ(nullptr, t1.GetV2());
	}

	// <Test> 構造体
	{
		// SetPropertyValue/GetPropertyValue
		Point pt1(1, 2);
		tr::Property::SetPropertyValue(&t1, PropertyTest1::V3Property, pt1);
		ASSERT_EQ(1, t1.m_v3.X);
		ASSERT_EQ(2, t1.m_v3.Y);
		tr::Variant v = tr::Property::GetPropertyValue(&t1, PropertyTest1::V3Property);
		ASSERT_EQ(1, tr::Variant::Cast<Point>(v).X);
		ASSERT_EQ(2, tr::Variant::Cast<Point>(v).Y);

		// アクセサメンバ関数
		t1.SetV3(Point(3, 4));
		ASSERT_EQ(3, t1.GetV3().X);
		ASSERT_EQ(4, t1.GetV3().Y);
	}

	// <Test> RefPtr
	{
		// SetPropertyValue/GetPropertyValue
		RefPtr<RefTest2> p1(LN_NEW RefTest2(), false);
		tr::Property::SetPropertyValue(&t1, PropertyTest1::V4Property, p1);
		ASSERT_EQ(p1, t1.m_v4);
		tr::Variant v = tr::Property::GetPropertyValue(&t1, PropertyTest1::V4Property);
		ASSERT_EQ(p1.GetObjectPtr(), tr::Variant::Cast<RefTest2*>(v));

		// アクセサメンバ関数
		RefPtr<RefTest2> p2(LN_NEW RefTest2(), false);
		t1.SetV4(p2);
		ASSERT_EQ(p2.GetObjectPtr(), t1.GetV4());

		// アクセサメンバ関数
		t1.SetV4(nullptr);
		ASSERT_EQ(nullptr, t1.GetV4());
	}
}
