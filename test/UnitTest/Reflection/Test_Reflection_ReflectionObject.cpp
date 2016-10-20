#include <TestConfig.h>
#include <Lumino/Reflection/Notify.h>
#include <Lumino/Reflection/ReflectionEventArgs.h>
#include <Lumino/Reflection/ReflectionObject.h>
#include <Lumino/Reflection/Property.h>

// テスト用
class Point
{
public:
	int x, y;

	Point()
		: x(0), y(0)
	{}

	Point(int x_, int y_)
		: x(x_), y(y_)
	{}

	bool operator != (const Point& rhs) const
	{
		return x != rhs.x || y != rhs.y;
	}
};

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
	String name;

	auto type1 = tr::TypeInfo::GetTypeInfo(&t1);
	name = type1->GetName();
	ASSERT_TRUE(name.IndexOf(_T("RefTest1")) >= 0);

	auto type2 = tr::TypeInfo::GetTypeInfo(&t2);
	name = type2->GetName();
	ASSERT_TRUE(name.IndexOf(_T("RefTest2")) >= 0);

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
	LN_TR_PROPERTY(RefPtr<RefTest2>, V4Property);

public:
	PropertyTest1()
		: m_v1(this, 0)
		, m_v2(this, nullptr)
		, m_v3(this)
		, m_v4(this)
	{
	}

	void SetV1(int v) { m_v1 = v; }
	int GetV1() const { return m_v1; }

	void SetV2(RefTest1* v) { m_v2 = v; }
	RefTest1* GetV2() const { return m_v2; }

	void SetV3(const Point& v) { m_v3 = v; }
	const Point& GetV3() const { return m_v3; }

	void SetV4(RefTest2* v) { m_v4.Set(v); }
	//RefTest2* GetV4() const { return m_v4; }	// TODO: できればコレがやりたいのだが・・・
	RefTest2* GetV4() const { return m_v4.Get(); }
	RefPtr<RefTest2> GetV4_2() const { return m_v4; }

	virtual void OnPropertyChanged(tr::PropertyChangedEventArgs* e) override
	{
		m_lastChangedProp = e->changedProperty;
	}

public:
	tr::Property<int> m_v1;
	tr::Property<RefTest1*> m_v2;
	tr::Property<Point> m_v3;
	tr::Property<RefPtr<RefTest2>> m_v4;
	const tr::PropertyInfo*	m_lastChangedProp;
};
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(PropertyTest1, tr::ReflectionObject);
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, int, V1Property, "V1", m_v1, tr::PropertyMetadata());
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, RefTest1*, V2Property, "V2", m_v2, tr::PropertyMetadata());
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, Point, V3Property, "V3", m_v3, tr::PropertyMetadata());
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, RefPtr<RefTest2>, V4Property, "V4", m_v4, tr::PropertyMetadata());


TEST_F(IntegrationTest_Reflection_ReflectionObject, Property)
{
	PropertyTest1 t1;

	// <Test> 値型
	{
		// set
		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V1Property, 100);
		ASSERT_EQ(100, t1.m_v1);

		// get
		tr::Variant v1 = tr::PropertyInfo::GetPropertyValue(&t1, PropertyTest1::V1Property);
		ASSERT_EQ(100, tr::Variant::Cast<int>(v1));

		// set (Direct)
		tr::PropertyInfo::SetPropertyValueDirect<int>(&t1, PropertyTest1::V1Property, 200);
		ASSERT_EQ(200, t1.m_v1);

		// get (Direct)
		tr::Variant v2 = tr::PropertyInfo::GetPropertyValueDirect<int>(&t1, PropertyTest1::V1Property);
		ASSERT_EQ(200, tr::Variant::Cast<int>(v2));

		// setter/getter
		t1.SetV1(300);
		ASSERT_EQ(300, t1.GetV1());
	}

	// <Test> ポインタ型
	{
		RefTest1 rt1;
		RefTest1 rt2;

		// set
		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V2Property, &rt1);
		ASSERT_EQ(&rt1, t1.m_v2);

		// get
		tr::Variant v1 = tr::PropertyInfo::GetPropertyValue(&t1, PropertyTest1::V2Property);
		ASSERT_EQ(&rt1, tr::Variant::Cast<RefTest1*>(v1));

		// set (Direct)
		tr::PropertyInfo::SetPropertyValueDirect<RefTest1*>(&t1, PropertyTest1::V2Property, &rt2);
		ASSERT_EQ(&rt2, t1.m_v2);

		// get (Direct)
		tr::Variant v2 = tr::PropertyInfo::GetPropertyValueDirect<RefTest1*>(&t1, PropertyTest1::V2Property);
		ASSERT_EQ(&rt2, tr::Variant::Cast<RefTest1*>(v2));

		// setter/getter
		RefTest1 rt3;
		t1.SetV2(&rt3);
		ASSERT_EQ(&rt3, t1.GetV2());

		// setter/getter (nullptr)
		t1.SetV2(nullptr);
		ASSERT_EQ(nullptr, t1.GetV2());
	}

	// <Test> 構造体
	{
		// SetPropertyValue/GetPropertyValue
		Point pt1(1, 2);
		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V3Property, pt1);
		ASSERT_EQ(1, t1.m_v3.Get().x);
		ASSERT_EQ(2, t1.m_v3.Get().y);
		tr::Variant v = tr::PropertyInfo::GetPropertyValue(&t1, PropertyTest1::V3Property);
		ASSERT_EQ(1, tr::Variant::Cast<Point>(v).x);
		ASSERT_EQ(2, tr::Variant::Cast<Point>(v).y);

		// アクセサメンバ関数
		t1.SetV3(Point(3, 4));
		ASSERT_EQ(3, t1.GetV3().x);
		ASSERT_EQ(4, t1.GetV3().y);
	}

	// <Test> RefPtr
	{
		// SetPropertyValue/GetPropertyValue
		RefPtr<RefTest2> p1(LN_NEW RefTest2(), false);
		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V4Property, p1);
		ASSERT_EQ(p1, t1.m_v4);
		tr::Variant v = tr::PropertyInfo::GetPropertyValue(&t1, PropertyTest1::V4Property);
		ASSERT_EQ(p1.Get(), tr::Variant::Cast<RefPtr<RefTest2>>(v));

		// アクセサメンバ関数
		RefPtr<RefTest2> p2(LN_NEW RefTest2(), false);
		t1.SetV4(p2);
		ASSERT_EQ(p2.Get(), t1.GetV4());

		// アクセサメンバ関数
		t1.SetV4(nullptr);
		ASSERT_EQ(nullptr, t1.GetV4());
	}

	// <Test> OnPropertyChanged 通知
	{
		t1.SetV1(1);
		ASSERT_EQ(PropertyTest1::V1Property, t1.m_lastChangedProp);

		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V2Property, nullptr);
		ASSERT_EQ(PropertyTest1::V2Property, t1.m_lastChangedProp);

		tr::PropertyInfo::SetPropertyValueDirect<int>(&t1, PropertyTest1::V1Property, 2);
		ASSERT_EQ(PropertyTest1::V1Property, t1.m_lastChangedProp);
	}

	// <Test> デフォルト値に戻す
	{
		t1.SetV1(1);
		ASSERT_EQ(1, t1.GetV1());

		auto ref = tr::PropertyInfo::GetProperty(&t1, PropertyTest1::V1Property);
		ref.ClearValue();

		ASSERT_EQ(0, t1.GetV1());
	}
}
