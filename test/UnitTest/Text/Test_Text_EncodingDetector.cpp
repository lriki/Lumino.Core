#include <TestConfig.h>
using namespace Text;

class Test_Text_EncodingDetector : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Text_EncodingDetector, UTF8BOM)
{
	EncodingDetector detector;
	RefPtr<RefBuffer> buf1(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/UTF8BOM.txt")));
	RefPtr<RefBuffer> buf2(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/UTF16LittleBOM.txt")));
	RefPtr<RefBuffer> buf3(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/UTF16BigBOM.txt")));
	RefPtr<RefBuffer> buf4(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/UTF32LittleBOM.txt")));
	RefPtr<RefBuffer> buf5(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/UTF32BigBOM.txt")));
	ASSERT_EQ(EncodingType_UTF8, detector.Detect(buf1->GetPointer(), buf1->GetSize()));
	ASSERT_EQ(EncodingType_UTF16L, detector.Detect(buf2->GetPointer(), buf2->GetSize()));
	ASSERT_EQ(EncodingType_UTF16B, detector.Detect(buf3->GetPointer(), buf3->GetSize()));
	ASSERT_EQ(EncodingType_UTF32L, detector.Detect(buf4->GetPointer(), buf4->GetSize()));
	ASSERT_EQ(EncodingType_UTF32B, detector.Detect(buf5->GetPointer(), buf5->GetSize()));
}

//---------------------------------------------------------------------
TEST_F(Test_Text_EncodingDetector, SJIS)
{
	EncodingDetector detector;
	RefPtr<RefBuffer> buf1(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/ConvertTable_SJIS_test.txt")));
	detector.Detect(buf1->GetPointer(), buf1->GetSize());
	ASSERT_EQ(EncodingType_SJIS, detector.GetEncodingType());

}
