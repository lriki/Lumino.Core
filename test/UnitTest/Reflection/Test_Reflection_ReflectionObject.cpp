#include <TestConfig.h>
#include <Lumino/Reflection/ReflectionObject.h>

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
