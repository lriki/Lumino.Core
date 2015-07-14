#include <TestConfig.h>
using namespace Text;

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

	auto a = s.ToString();
	printf("");
}
