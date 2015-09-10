#include <TestConfig.h>

class Test_Json_JsonWriter : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonWriter, Example)
{
	StringWriter s;
	Json::JsonWriter writer(&s);

	writer.StartObject();
	writer.WriteKey(_T("hello"));
	writer.WriteString(_T("world"));
	writer.WriteKey(_T("t"));
	writer.WriteBool(true);
	writer.WriteKey(_T("f"));
	writer.WriteBool(false);
	writer.WriteKey(_T("n"));
	writer.WriteNull();
	writer.WriteKey(_T("i"));
	writer.WriteDouble(123);
	writer.WriteKey(_T("pi"));
	writer.WriteDouble(3.1416);
	writer.WriteKey(_T("a"));
	writer.StartArray();
	for (unsigned i = 0; i < 4; i++)
		writer.WriteDouble(i);
	writer.EndArray();
	writer.EndObject();

	ASSERT_STREQ(
		_T("{\"hello\":\"world\",\"t\":true,\"f\":false,\"n\":null,\"i\":123.000000,\"pi\":3.141600,\"a\":[0.000000,1.000000,2.000000,3.000000]}"),
		s.ToString());
}
