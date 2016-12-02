#include <TestConfig.h>
#include <Lumino/IO/StringWriter.h>
#include <Lumino/Json/JsonWriter.h>
using namespace ln::tr;

//==============================================================================
// Test_Json_JsonWriter_Integrate
//==============================================================================
class Test_Json_JsonWriter_Integrate : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//------------------------------------------------------------------------------
TEST_F(Test_Json_JsonWriter_Integrate, Example)
{
	StringWriter s;
	JsonWriter writer(&s);

	writer.WriteStartObject();
	writer.WritePropertyName(_T("hello"));
	writer.WriteString(_T("world"));
	writer.WritePropertyName(_T("t"));
	writer.WriteBool(true);
	writer.WritePropertyName(_T("f"));
	writer.WriteBool(false);
	writer.WritePropertyName(_T("n"));
	writer.WriteNull();
	writer.WritePropertyName(_T("i"));
	writer.WriteDouble(123);
	writer.WritePropertyName(_T("pi"));
	writer.WriteDouble(3.1416);
	writer.WritePropertyName(_T("a"));
	writer.WriteStartArray();
	for (unsigned i = 0; i < 4; i++)
		writer.WriteDouble(i);
	writer.WriteEndArray();
	writer.WriteEndObject();

	ASSERT_EQ(
		_T("{\"hello\":\"world\",\"t\":true,\"f\":false,\"n\":null,\"i\":123.000000,\"pi\":3.141600,\"a\":[0.000000,1.000000,2.000000,3.000000]}"),
		s.ToString());
}
