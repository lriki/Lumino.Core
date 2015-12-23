#include <TestConfig.h>

class Test_Json_JsonReader : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

// 構造が正しく読めているかをチェックするための Handler
class TestJsonHandler
	: public JsonHandler
{
public:
	String Text;
	virtual bool OnNull()							{ Text += _T("<null>"); return true; }
	virtual bool OnBool(bool value)					{ if (value) Text += _T("<true>"); else Text += _T("<false>"); return true; }
	virtual bool OnDouble(double value)				{ Text += _T("<double>"); return true; }
	virtual bool OnString(const TCHAR* str, int len){ Text += _T("<str>"); return true; }
	virtual bool OnStartArray()						{ Text += _T("<ary>"); return true; }
	virtual bool OnEndArray(int elementCount)		{ Text += _T("</ary>"); return true; }
	virtual bool OnStartObject()					{ Text += _T("<obj>"); return true; }
	virtual bool OnKey(const TCHAR* str, int len)	{ Text += _T("<key>"); return true; }
	virtual bool OnEndObject(int memberCount)		{ Text += _T("</obj>"); return true; }
};

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonReader, Basic)
{
	String str = FileSystem::ReadAllText(LOCALFILE("TestData/ReaderTest1.txt"), Encoding::GetSystemMultiByteEncoding());
	TestJsonHandler handler;
	JsonReader reader(&handler);
	reader.Parse(str);
	ASSERT_EQ(handler.Text, _T("<obj><key><null><key><true><key><false><key><double><key><str><key><ary><double><double></ary><key><obj><key><double></obj></obj>"));
}

//最後の要素の後にカンマ (,) を付けてはなりません。


//---------------------------------------------------------------------
TEST_F(Test_Json_JsonReader, Basic2)
{
	{
		JsonReader2 reader(_T("{}"));
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartObject, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndObject, reader.GetTokenType());
		ASSERT_EQ(false, reader.Read());
	}
	// <Test> JSON のルート要素は配列も可能。
	// <Test> 先頭の空白は読み飛ばす。
	{
		//JsonReader2 reader(_T(" \t[]"));
		//ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartArray, reader.GetTokenType());
		//ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndArray, reader.GetTokenType());
		//ASSERT_EQ(false, reader.Read());
	}

	// <Test> オブジェクト
	// <Test> プロパティ名
	// <Test> 文字列値
	{
		JsonReader2 reader(_T("{\"name\":\"str\"}"));
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartObject, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::PropertyName, reader.GetTokenType()); ASSERT_EQ(_T("name"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::String, reader.GetTokenType()); ASSERT_EQ(_T("str"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndObject, reader.GetTokenType());
		ASSERT_EQ(false, reader.Read());
	}
	// <Test> 複数のメンバを持つオブジェクト
	{
		JsonReader2 reader(_T("{\"name\":\"str\",\"name2\":\"str2\",}"));
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartObject, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::PropertyName, reader.GetTokenType()); ASSERT_EQ(_T("name"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::String, reader.GetTokenType()); ASSERT_EQ(_T("str"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::PropertyName, reader.GetTokenType()); ASSERT_EQ(_T("name2"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::String, reader.GetTokenType()); ASSERT_EQ(_T("str2"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndObject, reader.GetTokenType());
		ASSERT_EQ(false, reader.Read());
	}
}


