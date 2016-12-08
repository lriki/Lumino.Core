#include <TestConfig.h>
#include <Lumino/Base/Serialization.h>
#include <Lumino/Json/JsonDocument.h>


class TestObject1
{
public:

	void lnsl_SerializeImpl(tr::Archive& ar)
	{
		ar & tr::MakeNVP("value1", m_value1);
	}

private:
	int		m_value1 = 100;
};

//==============================================================================
class Test_Serialization : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//------------------------------------------------------------------------------
TEST_F(Test_Serialization, Basic)
{
	//TestObject1 t1;
	//tr::JsonDocument doc;
	//tr::Archive ar(&doc);
}



