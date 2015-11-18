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
public:

};

//---------------------------------------------------------------------
TEST_F(IntegrationTest_Reflection_ReflectionObject, Basic)
{
	RefTest1 t1;
}
