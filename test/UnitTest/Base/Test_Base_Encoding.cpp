﻿#include <TestConfig.h>
#include "../../../src/Text/UTF16Encoding.h"
#include "../../../src/Text/UTF32Encoding.h"

class Test_Base_Encoding : public ::testing::Test
{
protected:
	virtual void SetUp() 
	{
		setlocale(LC_ALL, "");
	}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Base_Encoding, Basic)
{
	// TCHAR エンコーディング
	{
#ifdef LN_UNICODE
#ifdef _WIN32
		ASSERT_TRUE(dynamic_cast<UTF16Encoding*>(Encoding::GetTCharEncoding()) != NULL);
#else
		ASSERT_TRUE(dynamic_cast<UTF32Encoding*>(Encoding::GetTCharEncoding()) != NULL);
#endif
#else
		// TODO
		//ASSERT_TRUE(dynamic_cast<SystemMultiByteEncoding*>(Encoding::GetTCharEncoding()) != NULL);
#endif
	}
	// UTF16Encoding
	{
		ASSERT_NE((UTF16Encoding*)NULL, dynamic_cast<UTF16Encoding*>(Encoding::GetUTF16Encoding()));
	}
}

//---------------------------------------------------------------------
TEST_F(Test_Base_Encoding, Convert)
{
	EncodingConversionOptions options;
	options.NullTerminated = false;

	const char* buf1 = "123";
	std::unique_ptr<Decoder> decoder(Encoding::GetSystemMultiByteEncoding()->CreateDecoder());
	std::unique_ptr<Encoder> encoder(Encoding::GetWideCharEncoding()->CreateEncoder());

	// src が無い
	ASSERT_THROW(
		Encoding::Convert(NULL, 0, (Decoder*)NULL, NULL, options, NULL),
		ArgumentException);
	// decoder が無い
	ASSERT_THROW(
		Encoding::Convert(buf1, 0, (Decoder*)NULL, NULL, options, NULL),
		ArgumentException);
	// encoder が無い
	ASSERT_THROW(
		Encoding::Convert(buf1, 0, decoder.get(), NULL, options, NULL),
		ArgumentException);

	// ↓ストリーミング用状態記憶対応のため、許可する
	// ×最低でも必要なバイト数が無い
	//ASSERT_THROW(
	//	Encoding::Convert(buf1, 0, decoder, encoder, NULL),
	//	ArgumentException);
}

//---------------------------------------------------------------------
TEST_F(Test_Base_Encoding, SystemEncodingTest)
{
	// "日本語"
#ifdef _WIN32
	byte_t str1[] = { 0x93, 0xFA, 0x96, 0x7B, 0x8C, 0xEA, 0x00 };	// sjis
#else
	byte_t str1[] = { 0xE6, 0x97, 0xA5, 0xE6, 0x9C, 0xAC, 0xE8, 0xAA, 0x9E, 0x00 };	// utf8
#endif

	// std::wstring resize テスト


	//std::wstring s;// = L"あいうえお";
	////s.reserve(15);
	//s.resize(15);
	////s = L"あいうえおあいうえおあいうえお";
	//wchar_t* p = &(s[0]);
	//memcpy(p, L"あいうえおあいうえおあいうえお", 15 * sizeof(wchar_t));
	//printf("%d\n", s.size());
	//printf("%d\n", s.capacity());
	//wprintf(L"%s\n", s.c_str());

	// 最小バイト数
	{
		Encoding* e = Encoding::GetSystemMultiByteEncoding();
		ASSERT_EQ(1, e->GetMinByteCount());
	}

	// 最大バイト数
	{
		//setlocale(LC_ALL, "en-US");
		//char* a = setlocale(LC_ALL, ".65001");
		//char* a = setlocale(LC_ALL, "ko-KR.949");
		//char* a = setlocale(LC_ALL, ".949");
		//printf(a);
		//printf("%d\n", _getmbcp());
		Encoding* e = Encoding::GetSystemMultiByteEncoding();
		ASSERT_TRUE(1 <= e->GetMaxByteCount() && e->GetMaxByteCount() <= 6);
	}

	// 同一エンコーディング
	{
		StringA str2;
		str2.ConvertFrom(str1, 6, Encoding::GetSystemMultiByteEncoding());

		ASSERT_EQ(6, str2.GetLength());
	}

	// Multi → Wide
	{
		StringW str2;
#ifdef _WIN32
		str2.ConvertFrom(str1, 6, Encoding::GetSystemMultiByteEncoding());
#else
		str2.ConvertFrom(str1, 9, Encoding::GetSystemMultiByteEncoding());
#endif

		ASSERT_EQ(3, str2.GetLength());
		ASSERT_EQ(0x65E5, str2[0]);	// L'日'
		ASSERT_EQ(0x672C, str2[1]);	// L'本'
		ASSERT_EQ(0x8A9e, str2[2]);	// L'語'

		// 1文字だけ
		StringW str3;
		str3.AssignCStr("A");
		ASSERT_EQ(1, str3.GetLength());
		ASSERT_EQ('A', str3[0]);
	}

	// Wide → Multi 
	{
		// "日本語"
		wchar_t wstr1[] = { 0x65E5, 0x672C, 0x8A9e, 0x0000 };

		StringA str2;
		str2.ConvertFrom(wstr1, 3 * sizeof(wchar_t), Encoding::GetWideCharEncoding());

		ASSERT_EQ(strlen((char*)str1), str2.GetLength());
		//ASSERT_EQ(0x93, (byte_t)str2.at(0));	// '日'	※ unsingned char で比較しないと一致が取れない
		//ASSERT_EQ(0xFA, (byte_t)str2.at(1));	// '日'
		//ASSERT_EQ(0x96, (byte_t)str2.at(2));	// '本'
		//ASSERT_EQ(0x7B, (byte_t)str2.at(3));	// '本'
		//ASSERT_EQ(0x8C, (byte_t)str2.at(4));	// '語'
		//ASSERT_EQ(0xEA, (byte_t)str2.at(5));	// '語'
		ASSERT_TRUE(TestHelper::CheckArrays(str2.c_str(), str1, strlen((char*)str1)));

		// 1文字だけ
		StringA str3;
		str3.AssignCStr(L"A");
		ASSERT_EQ(1, str3.GetLength());
		ASSERT_EQ('A', str3[0]);
	}

#if 0
	// Multi に変換できない文字があった
	{
		//wchar_t str1[] = L"";
		wchar_t str1[] = { 0xD867, 0xDE3D, 0x0000 };	// ほっけ
		StringA str2;
		ASSERT_THROW(
			str2.ConvertFrom(str1, wcslen(str1) * sizeof(wchar_t), Encoding::GetWideCharEncoding()),
			EncodingFallbackException);
	}

	// Wide に変換できない文字があった
	{
		byte_t str1[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0x00 };
		StringW str2;
		ASSERT_THROW(
			str2.ConvertFrom(str1, strlen((const char*)str1), Encoding::GetSystemMultiByteEncoding()),
			EncodingFallbackException);
	}
#endif
}

//---------------------------------------------------------------------
TEST_F(Test_Base_Encoding, UTF16Encoding)
{
	//ASSERT_EQ(0xFF, Encoding::GetUTF16Encoding()->GetPreamble()[0]);
	//ASSERT_EQ(0xFE, Encoding::GetUTF16Encoding()->GetPreamble()[1]);
	//ASSERT_EQ(0x00, Encoding::GetUTF16Encoding()->GetPreamble()[2]);
	
}
