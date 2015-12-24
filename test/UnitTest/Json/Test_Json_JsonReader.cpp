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
		JsonReader2 reader(_T(" \t[]"));
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartArray, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndArray, reader.GetTokenType());
		ASSERT_EQ(false, reader.Read());
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
		JsonReader2 reader(_T("{\"name\":\"str\",\"name2\":\"str2\"}"));
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartObject, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::PropertyName, reader.GetTokenType()); ASSERT_EQ(_T("name"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::String, reader.GetTokenType()); ASSERT_EQ(_T("str"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::PropertyName, reader.GetTokenType()); ASSERT_EQ(_T("name2"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::String, reader.GetTokenType()); ASSERT_EQ(_T("str2"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndObject, reader.GetTokenType());
		ASSERT_EQ(false, reader.Read());
	}
	// <Test> null
	// <Test> true
	// <Test> false
	{
		JsonReader2 reader(_T("[null,true,false]"));
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartArray, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::Null, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::Boolean, reader.GetTokenType()); ASSERT_EQ(_T("true"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::Boolean, reader.GetTokenType()); ASSERT_EQ(_T("false"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndArray, reader.GetTokenType());
		ASSERT_EQ(false, reader.Read());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonReader, Error)
{
	// <Test> UnterminatedString
	{
		JsonReader2 reader(_T("{\""));
		while (reader.TryRead());
		ASSERT_EQ(JsonParseError2::UnterminatedString, reader.GetError().code);
	}
	// <Test> InvalidStringChar
	{
		JsonReader2 reader(_T("{\"\a\"}"));
		while (reader.TryRead());
		ASSERT_EQ(JsonParseError2::InvalidStringChar, reader.GetError().code);
	}
	// <Test> InvalidStringEscape
	{
		JsonReader2 reader(_T("{\"\\a\"}"));
		while (reader.TryRead());
		ASSERT_EQ(JsonParseError2::InvalidStringEscape, reader.GetError().code);
	}
	// <Test> InvalidObjectClosing
	{
		JsonReader2 reader(_T("{\"\":\"\",}"));
		while (reader.TryRead());
		ASSERT_EQ(JsonParseError2::InvalidObjectClosing, reader.GetError().code);
		ASSERT_EQ(7, reader.GetError().column);	// } の位置
	}
	// <Test> ArrayInvalidClosing
	{
		JsonReader2 reader(_T("[\"\",]"));
		while (reader.TryRead());
		ASSERT_EQ(JsonParseError2::ArrayInvalidClosing, reader.GetError().code);
		ASSERT_EQ(4, reader.GetError().column);	// ] の位置
	}
	// <Test> ValueInvalid
	{
		JsonReader2 reader(_T("[n]"));
		while (reader.TryRead());
		ASSERT_EQ(JsonParseError2::ValueInvalid, reader.GetError().code);
		JsonReader2 reader2(_T("[t]"));
		while (reader2.TryRead());
		ASSERT_EQ(JsonParseError2::ValueInvalid, reader.GetError().code);
		JsonReader2 reader3(_T("[f]"));
		while (reader3.TryRead());
		ASSERT_EQ(JsonParseError2::ValueInvalid, reader.GetError().code);
	}
}

