﻿#include <TestConfig.h>
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
	RefPtr<ByteBuffer> buf1(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/UTF8BOM.txt")));
	RefPtr<ByteBuffer> buf2(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/UTF16LittleBOM.txt")));
	RefPtr<ByteBuffer> buf3(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/UTF16BigBOM.txt")));
	RefPtr<ByteBuffer> buf4(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/UTF32LittleBOM.txt")));
	RefPtr<ByteBuffer> buf5(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/UTF32BigBOM.txt")));
	ASSERT_EQ(EncodingType_UTF8, detector.Detect(buf1->GetData(), buf1->GetSize()));
	ASSERT_EQ(EncodingType_UTF16L, detector.Detect(buf2->GetData(), buf2->GetSize()));
	ASSERT_EQ(EncodingType_UTF16B, detector.Detect(buf3->GetData(), buf3->GetSize()));
	ASSERT_EQ(EncodingType_UTF32L, detector.Detect(buf4->GetData(), buf4->GetSize()));
	ASSERT_EQ(EncodingType_UTF32B, detector.Detect(buf5->GetData(), buf5->GetSize()));
}

//---------------------------------------------------------------------
TEST_F(Test_Text_EncodingDetector, SJIS)
{
	EncodingDetector detector;
	RefPtr<ByteBuffer> buf1(FileUtils::ReadAllBytes(LN_TEST_GET_FILE_PATH("TestData/ConvertTable_SJIS_test.txt")));
	detector.Detect(buf1->GetData(), buf1->GetSize());
	ASSERT_EQ(EncodingType_SJIS, detector.GetEncodingType());

}
