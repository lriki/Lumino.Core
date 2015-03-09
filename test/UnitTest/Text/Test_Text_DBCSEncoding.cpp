#include <TestConfig.h>
using namespace Text;

class Test_Text_DBCSEncoding : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Text_DBCSEncoding, SJIS)
{
	Encoding* sjisEnc = Encoding::GetEncoding(EncodingType_SJIS);
	RefPtr<ByteBuffer> sjisBuf(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/ConvertTable_SJIS_test.txt")));
	RefPtr<ByteBuffer> utf16Buf(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/ConvertTable_SJIS_test_UTF16.txt")));

	// MBCS → UTF16
	EncodingConversionResult result;
	RefPtr<ByteBuffer> utf16Result(
		Encoding::Convert(sjisBuf->GetData(), sjisBuf->GetSize(), sjisEnc, Encoding::GetUTF16Encoding(), &result));

	int cmp = memcmp(utf16Buf->GetData(), utf16Result->GetData(), utf16Buf->GetSize());
	ASSERT_EQ(0, cmp);											// 内容一致
	ASSERT_TRUE(utf16Buf->GetSize() == utf16Result->GetSize());	// バッファサイズ一致

	// UTF16 → MBCS
	RefPtr<ByteBuffer> sjisResult(
		Encoding::Convert(utf16Result->GetData(), utf16Result->GetSize(), Encoding::GetUTF16Encoding(), sjisEnc, &result));

	cmp = memcmp(sjisBuf->GetData(), sjisResult->GetData(), sjisBuf->GetSize());
	ASSERT_EQ(0, cmp);											// 内容一致
	ASSERT_TRUE(sjisBuf->GetSize() == sjisResult->GetSize());	// バッファサイズ一致
}
