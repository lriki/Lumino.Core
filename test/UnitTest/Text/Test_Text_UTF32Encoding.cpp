#include <TestConfig.h>
using namespace Text;

class Test_Text_UTF32Encoding : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Text_UTF32Encoding, AsciiCode)
{
	// <Test> ASCII 文字コードの通常文字部分をすべて変換してみる。
	{
		char ascii[95];
		for (int i = 0; i < 95; ++i) { ascii[i] = 0x20 + i; }
		UTF32 utf32[95];
		//for (int i = 0; i < 95; ++i) { utf32[i] = 0x20 + i; }

		//UTF32 utf32Temp[95];
		EncodingConversionResult result;
		Encoding::Convert(ascii, 95, Encoding::GetSystemMultiByteEncoding(), utf32, sizeof(utf32), Encoding::GetUTF32Encoding(), &result);
	
		for (int i = 0; i < 95; ++i) {
			printf("%c : %c\n", ascii[i], (char)utf32[i]);
			ASSERT_EQ(0x20 + i, utf32[i]);
		}
	}


}
