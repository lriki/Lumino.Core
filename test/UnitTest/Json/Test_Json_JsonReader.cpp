#include <TestConfig.h>
using namespace Text;

class Test_Json_JsonReader : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

// 構造が正しく読めているかをチェックするための Handler
class TestJsonHandler
	: public Json::JsonHandler
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
	String str = FileSystem::ReadAllText(LOCALFILE("TestData/ReaderTest1.txt"), Text::Encoding::GetSystemMultiByteEncoding());
	TestJsonHandler handler;
	Json::JsonReader reader(&handler);
	reader.Parse(str);
	ASSERT_STREQ(handler.Text, _T("<obj><key><null><key><true><key><false><key><double><key><str><key><ary><double><double></ary><key><obj><key><double></obj></obj>"));
}

//最後の要素の後にカンマ (,) を付けてはなりません。



