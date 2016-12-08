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

	auto* ary = doc.AddMemberArray("ary");
	ary->AddBool(true);
	ary->AddInt32(200);

	auto* obj = doc.AddMemberObject("obj");
	obj->AddMemberInt32("i32_2", 100);

	auto* aryary = doc.AddMemberArray("aryary");
	aryary->AddArray()->AddInt32(111);
	aryary->AddArray()->AddInt32(222);

	auto* objary = doc.AddMemberArray("objary");
	objary->AddObject()->AddMemberInt32("int32", 300);
	objary->AddObject()->AddMemberString("str1", "test2");

	doc.Save(TEMPFILE("json.txt"));

	ASSERT_TRUE(TestHelper::EqualFiles(TEMPFILE("json.txt"), LN_LOCALFILE("TestData/Test_Json_JsonDocument_Save1.json")));

}


