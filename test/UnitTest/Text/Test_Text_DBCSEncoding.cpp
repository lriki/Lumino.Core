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
	RefPtr<RefBuffer> sjisBuf(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/ConvertTable_SJIS_test.txt")));
	RefPtr<RefBuffer> utf16Buf(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/ConvertTable_SJIS_test_UTF16.txt")));

	// MBCS → UTF16
	EncodingConversionResult result;
	RefPtr<RefBuffer> utf16Result(
		Encoding::Convert(sjisBuf->GetPointer(), sjisBuf->GetSize(), sjisEnc, Encoding::GetUTF16Encoding(), &result));

	int cmp = memcmp(utf16Buf->GetPointer(), utf16Result->GetPointer(), utf16Buf->GetSize());
	ASSERT_EQ(0, cmp);											// 内容一致
	ASSERT_TRUE(utf16Buf->GetSize() == utf16Result->GetSize());	// バッファサイズ一致

	// UTF16 → MBCS
	RefPtr<RefBuffer> sjisResult(
		Encoding::Convert(utf16Result->GetPointer(), utf16Result->GetSize(), Encoding::GetUTF16Encoding(), sjisEnc, &result));

	cmp = memcmp(sjisBuf->GetPointer(), sjisResult->GetPointer(), sjisBuf->GetSize());
	ASSERT_EQ(0, cmp);											// 内容一致
	ASSERT_TRUE(sjisBuf->GetSize() == sjisResult->GetSize());	// バッファサイズ一致
}
