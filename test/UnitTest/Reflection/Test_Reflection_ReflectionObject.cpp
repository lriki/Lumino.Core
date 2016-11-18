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


//==============================================================================
class Test_Reflection_Property : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

class PropertyTest1
	: public tr::ReflectionObject
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
public:
	LN_TR_PROPERTY2(int, V1);
	LN_TR_PROPERTY2(RefTest1*, V2);
	LN_TR_PROPERTY2(Point, V3);
	LN_TR_PROPERTY2(RefPtr<RefTest2>, V4);
	tr::Property<int> V5;	// メタデータを持たないプロパティ
	LN_TR_PROPERTY2(int, V6);

public:
	PropertyTest1()
		: V1(1)
		, V2(nullptr)
		, V3()
		, V4()
		, V5(5)
		, m_lastChangedProp(nullptr)
		, m_v6ChangedCount(0)
	{
		InitializeProperties();
	}

	void SetV1(int v) { V1 = v; }
	int GetV1() const { return V1; }

	void SetV2(RefTest1* v) { V2 = v; }
	RefTest1* GetV2() const { return V2; }

	void SetV3(const Point& v) { V3 = v; }
	const Point& GetV3() const { return V3; }

	void SetV4(RefTest2* v) { V4.Set(v); }
	//RefTest2* GetV4() const { return V4; }	// TODO: できればコレがやりたいのだが・・・
	RefTest2* GetV4() const { return V4.Get(); }
	RefPtr<RefTest2> GetV4_2() const { return V4; }

	virtual void OnPropertyChanged(tr::PropertyChangedEventArgs* e) override
	{
		m_lastChangedProp = e->changedProperty;
	}

public:
	const tr::PropertyInfo*	m_lastChangedProp;
	int m_v6ChangedCount;

private:
	static void V6Changed(tr::ReflectionObject* obj)
	{
		static_cast<PropertyTest1*>(obj)->m_v6ChangedCount++;
	}
};
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(PropertyTest1, tr::ReflectionObject);
LN_TR_PROPERTY2_IMPLEMENT(PropertyTest1, int, V1, tr::PropertyMetadata());
LN_TR_PROPERTY2_IMPLEMENT(PropertyTest1, RefTest1*, V2, tr::PropertyMetadata());
LN_TR_PROPERTY2_IMPLEMENT(PropertyTest1, Point, V3, tr::PropertyMetadata());
LN_TR_PROPERTY2_IMPLEMENT(PropertyTest1, RefPtr<RefTest2>, V4, tr::PropertyMetadata());
LN_TR_PROPERTY2_IMPLEMENT(PropertyTest1, int, V6, tr::PropertyMetadata(PropertyTest1::V6Changed));

//------------------------------------------------------------------------------
TEST_F(Test_Reflection_Property, GetSet)
{
	PropertyTest1 obj;
	RefTest1 t2;
	auto t3 = RefPtr<RefTest2>::MakeRef();

	// int type
	ASSERT_EQ(1, obj.V1);
	obj.V1 = 2;
	ASSERT_EQ(2, obj.V1);

	// pointer type
	ASSERT_EQ(nullptr, obj.V2);
	obj.V2 = &t2;
	ASSERT_EQ(&t2, obj.V2);

	// struct type
	Point pt1(1, 2);
	obj.V3 = pt1;
	ASSERT_EQ(1, obj.V3.Get().x);
	ASSERT_EQ(2, obj.V3.Get().y);

	// RefPtr<> type
	ASSERT_EQ(nullptr, obj.V4.Get());
	obj.V4 = t3;
	ASSERT_EQ(t3, obj.V4);
}

//------------------------------------------------------------------------------
TEST_F(Test_Reflection_Property, Notification)
{
	PropertyTest1 obj;
	// <Test> OwnerObject の確認
	{
		ASSERT_EQ(&obj, obj.V1.GetOwnerObject());
		ASSERT_EQ(&obj, obj.V2.GetOwnerObject());
		ASSERT_EQ(&obj, obj.V3.GetOwnerObject());
		ASSERT_EQ(&obj, obj.V4.GetOwnerObject());
	}

	// <Test> OnPropertyChanged 通知
	{
		// 初期値と同じだが source が Default → ユーザー値に変わるので通知が来る
		obj.SetV1(1);
		ASSERT_EQ(PropertyTest1::V1Id, obj.m_lastChangedProp);

		tr::PropertyInfo::SetPropertyValue(&obj, PropertyTest1::V2Id, nullptr);
		ASSERT_EQ(PropertyTest1::V2Id, obj.m_lastChangedProp);

		tr::PropertyInfo::SetPropertyValueDirect<int>(&obj, PropertyTest1::V1Id, 2);
		ASSERT_EQ(PropertyTest1::V1Id, obj.m_lastChangedProp);

		// 値のクリア。現在値と初期値が同じだが、ユーザー値なので通知が来る
		obj.V2.ClearValue();
		ASSERT_EQ(PropertyTest1::V2Id, obj.m_lastChangedProp);
	}

	// <Test> デフォルト値に戻す
	{
		obj.SetV1(10);
		ASSERT_EQ(10, obj.GetV1());

		auto ref = tr::PropertyInfo::GetProperty(&obj, PropertyTest1::V1Id);	// TODO: id のとり方は隠蔽したい
		ref.ClearValue();

		ASSERT_EQ(1, obj.GetV1());
	}

	// <Test> static PropertyChanged 通知
	{
		obj.V6 = 100;
		ASSERT_EQ(1, obj.m_v6ChangedCount);
	}
}

//------------------------------------------------------------------------------
TEST_F(IntegrationTest_Reflection_ReflectionObject, GetSetHelper)
{
	PropertyTest1 t1;

	// <Test> 値型
	{
		// set
		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V1Id, 100);
		ASSERT_EQ(100, t1.V1);

		// get
		tr::Variant v1 = tr::PropertyInfo::GetPropertyValue(&t1, PropertyTest1::V1Id);
		ASSERT_EQ(100, tr::Variant::Cast<int>(v1));

		// set (Direct)
		tr::PropertyInfo::SetPropertyValueDirect<int>(&t1, PropertyTest1::V1Id, 200);
		ASSERT_EQ(200, t1.V1);

		// get (Direct)
		tr::Variant v2 = tr::PropertyInfo::GetPropertyValueDirect<int>(&t1, PropertyTest1::V1Id);
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
		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V2Id, &rt1);
		ASSERT_EQ(&rt1, t1.V2);

		// get
		tr::Variant v1 = tr::PropertyInfo::GetPropertyValue(&t1, PropertyTest1::V2Id);
		ASSERT_EQ(&rt1, tr::Variant::Cast<RefTest1*>(v1));

		// set (Direct)
		tr::PropertyInfo::SetPropertyValueDirect<RefTest1*>(&t1, PropertyTest1::V2Id, &rt2);
		ASSERT_EQ(&rt2, t1.V2);

		// get (Direct)
		tr::Variant v2 = tr::PropertyInfo::GetPropertyValueDirect<RefTest1*>(&t1, PropertyTest1::V2Id);
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
		// set
		Point pt1(1, 2);
		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V3Id, pt1);
		ASSERT_EQ(1, t1.V3.Get().x);
		ASSERT_EQ(2, t1.V3.Get().y);

		// get
		tr::Variant v = tr::PropertyInfo::GetPropertyValue(&t1, PropertyTest1::V3Id);
		ASSERT_EQ(1, tr::Variant::Cast<Point>(v).x);
		ASSERT_EQ(2, tr::Variant::Cast<Point>(v).y);

		// set (Direct)
		Point pt2(10, 20);
		tr::PropertyInfo::SetPropertyValueDirect<Point>(&t1, PropertyTest1::V3Id, pt2);
		ASSERT_EQ(10, t1.V3.Get().x);
		ASSERT_EQ(20, t1.V3.Get().y);

		// get (Direct)
		tr::Variant v2 = tr::PropertyInfo::GetPropertyValueDirect<Point>(&t1, PropertyTest1::V3Id);
		ASSERT_EQ(10, tr::Variant::Cast<Point>(v2).x);
		ASSERT_EQ(20, tr::Variant::Cast<Point>(v2).y);

		// setter/getter
		t1.SetV3(Point(3, 4));
		ASSERT_EQ(3, t1.GetV3().x);
		ASSERT_EQ(4, t1.GetV3().y);
	}

	// <Test> RefPtr
	{
		// set
		RefPtr<RefTest2> p1(LN_NEW RefTest2(), false);
		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V4Id, p1);
		ASSERT_EQ(p1, t1.V4);

		// get
		tr::Variant v = tr::PropertyInfo::GetPropertyValue(&t1, PropertyTest1::V4Id);
		ASSERT_EQ(p1.Get(), tr::Variant::Cast<RefPtr<RefTest2>>(v));

		// set (Direct)
		RefPtr<RefTest2> p12(LN_NEW RefTest2(), false);
		tr::PropertyInfo::SetPropertyValueDirect<RefPtr<RefTest2>>(&t1, PropertyTest1::V4Id, p12);
		ASSERT_EQ(p12, t1.V4);

		// get (Direct)
		tr::Variant v2 = tr::PropertyInfo::GetPropertyValueDirect<RefPtr<RefTest2>>(&t1, PropertyTest1::V4Id);
		ASSERT_EQ(p12, tr::Variant::Cast<RefPtr<RefTest2>>(v2));

		// setter/getter
		RefPtr<RefTest2> p2(LN_NEW RefTest2(), false);
		t1.SetV4(p2);
		ASSERT_EQ(p2.Get(), t1.GetV4());

		// setter/getter (nullptr)
		t1.SetV4(nullptr);
		ASSERT_EQ(nullptr, t1.GetV4());
	}
}

//------------------------------------------------------------------------------
TEST_F(Test_Reflection_Property, NonMetadataProperty)
{
	PropertyTest1 t1;

	// get set
	ASSERT_EQ(5, t1.V5);
	t1.V5 = 10;
	ASSERT_EQ(10, t1.V5);

	// 通知は来ない
	ASSERT_EQ(nullptr, t1.m_lastChangedProp);

	// Clear
	t1.V5.ClearValue();
	ASSERT_EQ(5, t1.V5);
}




#if 0


//
//class Initier
//{
//public:
//	Initier()
//	{
//		printf("Initier\n");
//	}
//};
//


//---------------------------------------------------------------------
class PropertyTest1
	: public tr::ReflectionObject
	, public tr::IPropertyChangedListener
{
	LN_TR_REFLECTION_TYPEINFO_DECLARE();
	//Initier	_ini;
public:
	LN_TR_PROPERTY2(int, V1Property);
	LN_TR_PROPERTY2(RefTest1*, V2Property);
	LN_TR_PROPERTY2(Point, V3Property);
	LN_TR_PROPERTY2(RefPtr<RefTest2>, V4Property);
	LN_TR_PROPERTY2(int, V5Property);
	LN_TR_PROPERTY2(int, V6Property);

public:
	PropertyTest1()
		: m_v1(0)
		, m_v2(nullptr)
		, m_v3()
		, m_v4()
		, m_v5()
		, m_v6()
	{
		//printf("PropertyTest1\n");
		//InitializeProperties();
		m_v1.AddListener(this);
		m_v2.AddListener(this);
		m_v3.AddListener(this);
		m_v4.AddListener(this);
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

protected:
	tr::Property<int> m_v5;	// proptected の確認
private:
	tr::Property<int> m_v6;	// private の確認
};
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(PropertyTest1, tr::ReflectionObject);
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, int, V1Property, "V1", m_v1, tr::PropertyMetadata());
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, RefTest1*, V2Property, "V2", m_v2, tr::PropertyMetadata());
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, Point, V3Property, "V3", m_v3, tr::PropertyMetadata());
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, RefPtr<RefTest2>, V4Property, "V4", m_v4, tr::PropertyMetadata());
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, int, V5Property, "V5", m_v5, tr::PropertyMetadata());
LN_TR_PROPERTY_IMPLEMENT(PropertyTest1, int, V6Property, "V6", m_v6, tr::PropertyMetadata());


TEST_F(IntegrationTest_Reflection_ReflectionObject, Property)
{
	PropertyTest1 t1;

	// <Test> 値型
	{
		// set
		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V1PropertyId, 100);
		ASSERT_EQ(100, t1.m_v1);

		// get
		tr::Variant v1 = tr::PropertyInfo::GetPropertyValue(&t1, PropertyTest1::V1PropertyId);
		ASSERT_EQ(100, tr::Variant::Cast<int>(v1));

		// set (Direct)
		tr::PropertyInfo::SetPropertyValueDirect<int>(&t1, PropertyTest1::V1PropertyId, 200);
		ASSERT_EQ(200, t1.m_v1);

		// get (Direct)
		tr::Variant v2 = tr::PropertyInfo::GetPropertyValueDirect<int>(&t1, PropertyTest1::V1PropertyId);
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
		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V2PropertyId, &rt1);
		ASSERT_EQ(&rt1, t1.m_v2);

		// get
		tr::Variant v1 = tr::PropertyInfo::GetPropertyValue(&t1, PropertyTest1::V2PropertyId);
		ASSERT_EQ(&rt1, tr::Variant::Cast<RefTest1*>(v1));

		// set (Direct)
		tr::PropertyInfo::SetPropertyValueDirect<RefTest1*>(&t1, PropertyTest1::V2PropertyId, &rt2);
		ASSERT_EQ(&rt2, t1.m_v2);

		// get (Direct)
		tr::Variant v2 = tr::PropertyInfo::GetPropertyValueDirect<RefTest1*>(&t1, PropertyTest1::V2PropertyId);
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
		// set
		Point pt1(1, 2);
		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V3PropertyId, pt1);
		ASSERT_EQ(1, t1.m_v3.Get().x);
		ASSERT_EQ(2, t1.m_v3.Get().y);

		// get
		tr::Variant v = tr::PropertyInfo::GetPropertyValue(&t1, PropertyTest1::V3PropertyId);
		ASSERT_EQ(1, tr::Variant::Cast<Point>(v).x);
		ASSERT_EQ(2, tr::Variant::Cast<Point>(v).y);

		// set (Direct)
		Point pt2(10, 20);
		tr::PropertyInfo::SetPropertyValueDirect<Point>(&t1, PropertyTest1::V3PropertyId, pt2);
		ASSERT_EQ(10, t1.m_v3.Get().x);
		ASSERT_EQ(20, t1.m_v3.Get().y);

		// get (Direct)
		tr::Variant v2 = tr::PropertyInfo::GetPropertyValueDirect<Point>(&t1, PropertyTest1::V3PropertyId);
		ASSERT_EQ(10, tr::Variant::Cast<Point>(v2).x);
		ASSERT_EQ(20, tr::Variant::Cast<Point>(v2).y);

		// setter/getter
		t1.SetV3(Point(3, 4));
		ASSERT_EQ(3, t1.GetV3().x);
		ASSERT_EQ(4, t1.GetV3().y);
	}

	// <Test> RefPtr
	{
		// set
		RefPtr<RefTest2> p1(LN_NEW RefTest2(), false);
		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V4PropertyId, p1);
		ASSERT_EQ(p1, t1.m_v4);

		// get
		tr::Variant v = tr::PropertyInfo::GetPropertyValue(&t1, PropertyTest1::V4PropertyId);
		ASSERT_EQ(p1.Get(), tr::Variant::Cast<RefPtr<RefTest2>>(v));

		// set (Direct)
		RefPtr<RefTest2> p12(LN_NEW RefTest2(), false);
		tr::PropertyInfo::SetPropertyValueDirect<RefPtr<RefTest2>>(&t1, PropertyTest1::V4PropertyId, p12);
		ASSERT_EQ(p12, t1.m_v4);

		// get (Direct)
		tr::Variant v2 = tr::PropertyInfo::GetPropertyValueDirect<RefPtr<RefTest2>>(&t1, PropertyTest1::V4PropertyId);
		ASSERT_EQ(p12, tr::Variant::Cast<RefPtr<RefTest2>>(v2));

		// setter/getter
		RefPtr<RefTest2> p2(LN_NEW RefTest2(), false);
		t1.SetV4(p2);
		ASSERT_EQ(p2.Get(), t1.GetV4());

		// setter/getter (nullptr)
		t1.SetV4(nullptr);
		ASSERT_EQ(nullptr, t1.GetV4());
	}

	// <Test> OnPropertyChanged 通知
	{
		t1.SetV1(1);
		ASSERT_EQ(PropertyTest1::V1PropertyId, t1.m_lastChangedProp);

		tr::PropertyInfo::SetPropertyValue(&t1, PropertyTest1::V2PropertyId, nullptr);
		ASSERT_EQ(PropertyTest1::V2PropertyId, t1.m_lastChangedProp);

		tr::PropertyInfo::SetPropertyValueDirect<int>(&t1, PropertyTest1::V1PropertyId, 2);
		ASSERT_EQ(PropertyTest1::V1PropertyId, t1.m_lastChangedProp);
	}

	// <Test> デフォルト値に戻す
	{
		t1.SetV1(1);
		ASSERT_EQ(1, t1.GetV1());

		auto ref = tr::PropertyInfo::GetProperty(&t1, PropertyTest1::V1PropertyId);
		ref.ClearValue();

		ASSERT_EQ(0, t1.GetV1());
	}
}

#endif
