#include <TestConfig.h>
#include <Lumino/IO/FileSystem.h>
#include <Lumino/IO/StreamReader.h>
#include <Lumino/IO/StringReader.h>
#include <Lumino/Text/Encoding.h>
#include <Lumino/Json/JsonReader.h>
using namespace ln::tr;

class Test_Json_JsonReader : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

// �\�����������ǂ߂Ă��邩���`�F�b�N���邽�߂� Handler
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
	String str = FileSystem::ReadAllText(LN_LOCALFILE("TestData/ReaderTest1.txt"), Encoding::GetSystemMultiByteEncoding());
	TestJsonHandler handler;
	JsonReader reader(&handler);
	reader.Parse(str);
	ASSERT_EQ(handler.Text, _T("<obj><key><null><key><true><key><false><key><double><key><str><key><ary><double><double></ary><key><obj><key><double></obj></obj>"));
}

//�Ō�̗v�f�̌�ɃJ���} (,) ��t���Ă͂Ȃ�܂���B


//---------------------------------------------------------------------
TEST_F(Test_Json_JsonReader, Basic2)
{
	{
		JsonReader2 reader(_T("{}"));
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartObject, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndObject, reader.GetTokenType());
		ASSERT_EQ(false, reader.Read());
	}
	// <Test> JSON �̃��[�g�v�f�͔z����\�B
	// <Test> �擪�̋󔒂͓ǂݔ�΂��B
	{
		JsonReader2 reader(_T(" \t[]"));
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartArray, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndArray, reader.GetTokenType());
		ASSERT_EQ(false, reader.Read());
	}

	// <Test> �I�u�W�F�N�g
	// <Test> �v���p�e�B��
	// <Test> ������l
	{
		JsonReader2 reader(_T("{\"name\":\"str\"}"));
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartObject, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::PropertyName, reader.GetTokenType()); ASSERT_EQ(_T("name"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::String, reader.GetTokenType()); ASSERT_EQ(_T("str"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndObject, reader.GetTokenType());
		ASSERT_EQ(false, reader.Read());
	}
	// <Test> �����̃����o�����I�u�W�F�N�g
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
	// <Test> ���l
	{
		JsonReader2 reader(_T("[10]"));
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartArray, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::Int32, reader.GetTokenType()); ASSERT_EQ(_T("10"), reader.GetValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndArray, reader.GetTokenType());
		ASSERT_EQ(false, reader.Read());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonReader, GetValue)
{
	// <Test> Bool �̒l�̎擾
	{
		JsonReader2 reader(_T("[true,false]"));
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartArray, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::Boolean, reader.GetTokenType()); ASSERT_EQ(true, reader.GetBoolValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::Boolean, reader.GetTokenType()); ASSERT_EQ(false, reader.GetBoolValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndArray, reader.GetTokenType());
		ASSERT_EQ(false, reader.Read());
	}
	// <Test> Int32 �� Int64 �� Double �̋��
	// <Test> Int32 �� Int64 �� Double �̒l�̎擾
	{
		JsonReader2 reader(_T("[-2147483648,2147483647,-2147483649,2147483648,1.0,-1.0]"));
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::StartArray, reader.GetTokenType());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::Int32, reader.GetTokenType()); ASSERT_EQ(-2147483648, reader.GetInt32Value());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::Int32, reader.GetTokenType()); ASSERT_EQ(2147483647, reader.GetInt32Value());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::Int64, reader.GetTokenType()); ASSERT_EQ(-2147483649LL, reader.GetInt64Value());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::Int64, reader.GetTokenType()); ASSERT_EQ(2147483648LL, reader.GetInt64Value());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::Float, reader.GetTokenType()); ASSERT_EQ(1.0, reader.GetFloatValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::Float, reader.GetTokenType()); ASSERT_EQ(-1.0, reader.GetFloatValue());
		ASSERT_EQ(true, reader.Read()); ASSERT_EQ(JsonToken::EndArray, reader.GetTokenType());
		ASSERT_EQ(false, reader.Read());
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonReader, Integrate)
{
	// http://json.org/example.html
	// <Integrate> �I�t�B�V�����y�[�W�̃T���v��1
	{
		StreamReader r(LN_LOCALFILE(_T("TestData/JSONExample1.json")));
		JsonReader2 jr(&r);
		ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::StartObject, jr.GetTokenType());
			ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("glossary"), jr.GetValue());
			ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::StartObject, jr.GetTokenType());
				ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("title"), jr.GetValue());
				ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::String, jr.GetTokenType()); ASSERT_EQ(_T("example glossary"), jr.GetValue());
				ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("GlossDiv"), jr.GetValue());
				ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::StartObject, jr.GetTokenType());
					ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("title"), jr.GetValue());
					ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::String, jr.GetTokenType()); ASSERT_EQ(_T("S"), jr.GetValue());
					ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("GlossList"), jr.GetValue());
					ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::StartObject, jr.GetTokenType());
						ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("GlossEntry"), jr.GetValue());
						ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::StartObject, jr.GetTokenType());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("ID"), jr.GetValue());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::String, jr.GetTokenType()); ASSERT_EQ(_T("SGML"), jr.GetValue());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("SortAs"), jr.GetValue());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::String, jr.GetTokenType()); ASSERT_EQ(_T("SGML"), jr.GetValue());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("GlossTerm"), jr.GetValue());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::String, jr.GetTokenType()); ASSERT_EQ(_T("Standard Generalized Markup Language"), jr.GetValue());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("Acronym"), jr.GetValue());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::String, jr.GetTokenType()); ASSERT_EQ(_T("SGML"), jr.GetValue());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("Abbrev"), jr.GetValue());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::String, jr.GetTokenType()); ASSERT_EQ(_T("ISO 8879:1986"), jr.GetValue());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("GlossDef"), jr.GetValue());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::StartObject, jr.GetTokenType());
								ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("para"), jr.GetValue());
								ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::String, jr.GetTokenType()); ASSERT_EQ(_T("A meta-markup language, used to create markup languages such as DocBook."), jr.GetValue());
								ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("GlossSeeAlso"), jr.GetValue());
								ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::StartArray, jr.GetTokenType());
									ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::String, jr.GetTokenType()); ASSERT_EQ(_T("GML"), jr.GetValue());
									ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::String, jr.GetTokenType()); ASSERT_EQ(_T("XML"), jr.GetValue());
								ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::EndArray, jr.GetTokenType());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::EndObject, jr.GetTokenType());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::PropertyName, jr.GetTokenType()); ASSERT_EQ(_T("GlossSee"), jr.GetValue());
							ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::String, jr.GetTokenType()); ASSERT_EQ(_T("markup"), jr.GetValue());
						ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::EndObject, jr.GetTokenType());
					ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::EndObject, jr.GetTokenType());
				ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::EndObject, jr.GetTokenType());
			ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::EndObject, jr.GetTokenType());
		ASSERT_EQ(true, jr.Read()); ASSERT_EQ(JsonToken::EndObject, jr.GetTokenType());
		ASSERT_EQ(false, jr.Read());
	}
	// <Integrate> �I�t�B�V�����y�[�W�̃T���v��2
	{
		StreamReader r(LN_LOCALFILE(_T("TestData/JSONExample2.json")));
		JsonReader2 jr(&r);
		while (jr.Read());
		ASSERT_EQ(JsonParseError2::NoError, jr.GetError().code);
		ASSERT_EQ(true, r.IsEOF());
	}
	// <Integrate> �I�t�B�V�����y�[�W�̃T���v��3
	{
		StreamReader r(LN_LOCALFILE(_T("TestData/JSONExample3.json")));
		JsonReader2 jr(&r);
		while (jr.Read());
		ASSERT_EQ(JsonParseError2::NoError, jr.GetError().code);
		ASSERT_EQ(true, r.IsEOF());
	}
	// <Integrate> �I�t�B�V�����y�[�W�̃T���v��4
	{
		StreamReader r(LN_LOCALFILE(_T("TestData/JSONExample4.json")));
		JsonReader2 jr(&r);
		while (jr.Read());
		ASSERT_EQ(JsonParseError2::NoError, jr.GetError().code);
		ASSERT_EQ(true, r.IsEOF());
	}
	// <Integrate> �I�t�B�V�����y�[�W�̃T���v��5
	{
		StreamReader r(LN_LOCALFILE(_T("TestData/JSONExample5.json")));
		JsonReader2 jr(&r);
		while (jr.Read());
		ASSERT_EQ(JsonParseError2::NoError, jr.GetError().code);
		ASSERT_EQ(true, r.IsEOF());
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
		ASSERT_EQ(7, reader.GetError().column);	// } �̈ʒu
	}
	// <Test> ArrayInvalidClosing
	{
		JsonReader2 reader(_T("[\"\",]"));
		while (reader.TryRead());
		ASSERT_EQ(JsonParseError2::ArrayInvalidClosing, reader.GetError().code);
		ASSERT_EQ(4, reader.GetError().column);	// ] �̈ʒu
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

//---------------------------------------------------------------------
TEST_F(Test_Json_JsonReader, Issues)
{
	// <Issue> �l�X�g���ꂽ�z��̏I�[�ŉ�͂��I�����Ă��܂��B
	{
		StringReader r(_T(R"([{"tags": ["dolor"]},"end"])"));
		JsonReader2 jr(&r);
		while (jr.Read());
		ASSERT_EQ(JsonParseError2::NoError, jr.GetError().code);
		ASSERT_EQ(true, r.IsEOF());
	}
	// <Issue> �v���p�e�B�̒l���z�񂾂ƁA���̃v���p�e�B�̉�͂ŏI�����Ă��܂��B
	{
		StringReader r(_T(R"({"tags": [],"friends":"10"})"));
		JsonReader2 jr(&r);
		while (jr.Read());
		ASSERT_EQ(JsonParseError2::NoError, jr.GetError().code);
		ASSERT_EQ(true, r.IsEOF());
	}
}

//---------------------------------------------------------------------
//TEST_F(Test_Json_JsonReader, JsonGenerator)
//{
//	StreamReader r(Test_GetTempFilePath(_T("JsonGenerator.txt")));
//	JsonReader2 jr(&r);
//
//	while (jr.Read())
//	{
//		printf("%s ", jr.GetValue().c_str());
//	}
//	ASSERT_EQ(JsonParseError2::NoError, jr.GetError().code);
//	ASSERT_EQ(true, r.IsEOF());
//}
