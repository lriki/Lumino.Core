#include <TestConfig.h>
#include <Lumino/Json/JsonDocument.h>

class Test_Json_JsonDocument : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonDocument, Basic)
{
	//const char* json = R"( { "hello" : "world", "t" : true , "f" : false, "n": null, "i":123, "pi": 3.1416, "a":[1, 2, 3, 4] } )";
	////String str = FileSystem::ReadAllText(LN_LOCALFILE("TestData/ReaderTest1.txt"), Encoding::GetSystemMultiByteEncoding());
	//tr::JsonDocument2 doc;

	//////ElapsedTimer t;
	//////t.Start();
	//doc.Parse(json);

	////printf("%d", t.GetElapsedTime());

	//printf("");
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonDocument, Save)
{
	tr::JsonDocument2 doc;
	doc.AddMemberBool("b1", true);
	doc.AddMemberBool("b2", false);
	doc.AddMemberInt32("i32", 1);
	doc.AddMemberInt64("i64", 2147483648LL);
	doc.AddMemberFloat("f", 1.0f);
	doc.AddMemberDouble("dbl", 1.0);
	doc.AddMemberString("str", _T("test"));
	auto* obj = doc.AddMemberObject("obj");
	obj->AddMemberInt32("i32_2", 100);
	doc.Save(TEMPFILE("json.txt"));
}


