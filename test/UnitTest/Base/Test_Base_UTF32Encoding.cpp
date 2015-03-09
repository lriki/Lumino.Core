#include <TestConfig.h>
#include "../../../src/Text/UTF32Encoding.h"
using namespace Text;

class Test_Base_UTF32Encoding : public ::testing::Test
{
protected:
	virtual void SetUp() 
	{
		setlocale(LC_ALL, "");
	}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Base_UTF32Encoding, Basic)
{
	// UTF-32 → UTF-16
	{
		// 'あ' little
		byte_t buf1[] = { 0x42, 0x30, 0x00, 0x00 };

		Text::UTF32Encoding utf32Enc(false, false);
		RefPtr<Decoder> decoder(utf32Enc.CreateDecoder());
		RefPtr<Encoder> encoder(Encoding::GetUTF16Encoding()->CreateEncoder());
		EncodingConversionResult info;
		RefPtr<ByteBuffer> result1(Encoding::Convert(buf1, 4, decoder, encoder, &info));

		byte_t* utf16buf = (byte_t*)result1->GetData();
		ASSERT_EQ(2, info.BytesUsed);
		ASSERT_EQ(1, info.CharsUsed);
		ASSERT_EQ(false, info.UsedDefaultChar);
		ASSERT_EQ(0x42, utf16buf[0]);
		ASSERT_EQ(0x30, utf16buf[1]);
	}
	// UTF-32 → UTF-16 (分割)
	{
		// 'あ' little
		byte_t buf1[] = { 0x42, 0x30, 0x00, 0x00 };

		Text::UTF32Encoding utf32Enc(false, false);
		RefPtr<Decoder> decoder(utf32Enc.CreateDecoder());
		RefPtr<Encoder> encoder(Encoding::GetUTF16Encoding()->CreateEncoder());
		EncodingConversionResult info;

		// 1回目の変換。2byte分。出てくるのは何もない。decoder に状態記憶される。
		RefPtr<ByteBuffer> result1(Encoding::Convert(&buf1[0], 2, decoder, encoder, &info));
		ASSERT_EQ(0, info.BytesUsed);
		ASSERT_EQ(0, info.CharsUsed);
		ASSERT_EQ(false, info.UsedDefaultChar);

		// 2回目の変換。ここで1文字完成して出てくる。
		RefPtr<ByteBuffer> result2(Encoding::Convert(&buf1[2], 2, decoder, encoder, &info));
		byte_t* utf16buf = (byte_t*)result2->GetData();
		ASSERT_EQ(2, info.BytesUsed);
		ASSERT_EQ(1, info.CharsUsed);
		ASSERT_EQ(false, info.UsedDefaultChar);
		ASSERT_EQ(0x42, utf16buf[0]);
		ASSERT_EQ(0x30, utf16buf[1]);
	}

	// UTF-16 → UTF-32
	{
		// 'あ' little
		byte_t uttf16buf[] = { 0x42, 0x30 };

		Text::UTF32Encoding utf32Enc(false, false);
		RefPtr<Decoder> decoder(Encoding::GetUTF16Encoding()->CreateDecoder());
		RefPtr<Encoder> encoder(utf32Enc.CreateEncoder());
		EncodingConversionResult info;
		RefPtr<ByteBuffer> result1(Encoding::Convert(uttf16buf, 2, decoder, encoder, &info));

		byte_t* utf32buf = (byte_t*)result1->GetData();
		ASSERT_EQ(4, info.BytesUsed);
		ASSERT_EQ(1, info.CharsUsed);
		ASSERT_EQ(false, info.UsedDefaultChar);
		ASSERT_EQ(0x42, utf32buf[0]);
		ASSERT_EQ(0x30, utf32buf[1]);
		ASSERT_EQ(0x00, utf32buf[2]);
		ASSERT_EQ(0x00, utf32buf[3]);
	}

	// UTF-16 → UTF-32 (分割)
	{
		// 'ほっけ' little
		byte_t uttf16buf[] = { 0x67, 0xD8, 0x3D, 0xDE };
		
		Text::UTF32Encoding utf32Enc(false, false);
		RefPtr<Decoder> decoder(Encoding::GetUTF16Encoding()->CreateDecoder());
		RefPtr<Encoder> encoder(utf32Enc.CreateEncoder());
		EncodingConversionResult info;

		// 1回目の変換。2byte分。出てくるのは何もない。状態記憶される。
		RefPtr<ByteBuffer> result1(Encoding::Convert(&uttf16buf[0], 2, decoder, encoder, &info));
		ASSERT_EQ(0, info.BytesUsed);
		ASSERT_EQ(0, info.CharsUsed);
		ASSERT_EQ(false, info.UsedDefaultChar);

		// 2回目の変換。ここで1文字完成して出てくる。
		RefPtr<ByteBuffer> result2(Encoding::Convert(&uttf16buf[2], 2, decoder, encoder, &info));

		byte_t* utf32buf = (byte_t*)result2->GetData();
		ASSERT_EQ(4, info.BytesUsed);
		ASSERT_EQ(1, info.CharsUsed);
		ASSERT_EQ(false, info.UsedDefaultChar);
		ASSERT_EQ(0x3D, utf32buf[0]);
		ASSERT_EQ(0x9E, utf32buf[1]);
		ASSERT_EQ(0x02, utf32buf[2]);
		ASSERT_EQ(0x00, utf32buf[3]);
	}

	// UTF-16 → UTF-32 (1byteずつ分割)
	{
		// 'ほっけ' little
		byte_t uttf16buf[] = { 0x67, 0xD8, 0x3D, 0xDE };

		Text::UTF32Encoding utf32Enc(false, false);
		RefPtr<Decoder> decoder(Encoding::GetUTF16Encoding()->CreateDecoder());
		RefPtr<Encoder> encoder(utf32Enc.CreateEncoder());
		EncodingConversionResult info;

		// 1回目の変換。1byte分。出てくるのは何もない。状態記憶される。
		RefPtr<ByteBuffer> result1(Encoding::Convert(&uttf16buf[0], 1, decoder, encoder, &info));
		ASSERT_EQ(0, info.BytesUsed);
		ASSERT_EQ(0, info.CharsUsed);
		ASSERT_EQ(false, info.UsedDefaultChar);

		// 2回目の変換。1byte分。出てくるのは何もない。状態記憶される。
		RefPtr<ByteBuffer> result2(Encoding::Convert(&uttf16buf[1], 1, decoder, encoder, &info));
		ASSERT_EQ(0, info.BytesUsed);
		ASSERT_EQ(0, info.CharsUsed);
		ASSERT_EQ(false, info.UsedDefaultChar);

		// 3回目の変換。1byte分。出てくるのは何もない。状態記憶される。
		RefPtr<ByteBuffer> result3(Encoding::Convert(&uttf16buf[2], 1, decoder, encoder, &info));
		ASSERT_EQ(0, info.BytesUsed);
		ASSERT_EQ(0, info.CharsUsed);
		ASSERT_EQ(false, info.UsedDefaultChar);

		// 4回目の変換。ここで1文字完成して出てくる。
		RefPtr<ByteBuffer> result4(Encoding::Convert(&uttf16buf[3], 1, decoder, encoder, &info));

		byte_t* utf32buf = (byte_t*)result4->GetData();
		ASSERT_EQ(4, info.BytesUsed);
		ASSERT_EQ(1, info.CharsUsed);
		ASSERT_EQ(false, info.UsedDefaultChar);
		ASSERT_EQ(0x3D, utf32buf[0]);
		ASSERT_EQ(0x9E, utf32buf[1]);
		ASSERT_EQ(0x02, utf32buf[2]);
		ASSERT_EQ(0x00, utf32buf[3]);
	}
}
