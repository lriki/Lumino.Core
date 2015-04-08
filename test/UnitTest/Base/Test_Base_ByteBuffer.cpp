#include <TestConfig.h>

class Test_Base_ByteBuffer : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};


//-----------------------------------------------------------------------------
TEST_F(Test_Base_ByteBuffer, Constructor)
{
	// デフォルト
	ByteBuffer buf1;
	ASSERT_EQ(NULL, buf1.GetData());
	ASSERT_EQ(0, buf1.GetSize());

	// サイズ指定 + 0 クリア
	ByteBuffer buf2(10);
	ASSERT_EQ(0, *((byte_t*)buf2.GetData()));
	ASSERT_EQ(10, buf2.GetSize());

	// サイズ指定
	ByteBuffer buf3(1, false);
	ASSERT_EQ(1, buf3.GetSize());

	// データコピー
	const char* data1 = "test";
	ByteBuffer buf4(data1, 4);
	ASSERT_EQ(4, buf4.GetSize());
	ASSERT_EQ('t', buf4[0]);
	ASSERT_EQ('e', buf4[1]);
	ASSERT_EQ('s', buf4[2]);
	ASSERT_EQ('t', buf4[3]);

	// メモリバッファ参照
	char data2[2];
	ByteBuffer buf5(data2, 2, true);
	buf5.Copy("ab", 2);
	ASSERT_EQ(2, buf5.GetSize());
	ASSERT_TRUE(TestUtils::CheckArrays(data2, buf5.GetData(), 2));
	ASSERT_EQ('a', buf5[0]);
	ASSERT_EQ('b', buf5[1]);
	ASSERT_EQ('a', data2[0]);	// 元バッファにも書き込まれている
	ASSERT_EQ('b', data2[1]);	// 元バッファにも書き込まれている

	// データコピー
	char data6[5] = "test";
	ByteBuffer buf6(data6, 4, false);
	ASSERT_EQ(4, buf6.GetSize());
	ASSERT_EQ('t', buf6[0]);
	ASSERT_EQ('e', buf6[1]);
	ASSERT_EQ('s', buf6[2]);
	ASSERT_EQ('t', buf6[3]);
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_ByteBuffer, Alloc)
{
	// Alloc + 0クリア
	ByteBuffer buf1;
	buf1.Alloc(5);
	ASSERT_EQ(0, buf1[0]);
	ASSERT_EQ(5, buf1.GetSize());

	// 再確保
	buf1.Alloc(10, false);
	ASSERT_EQ(10, buf1.GetSize());

	// データコピー
	const char* data1 = "ab";
	buf1.Alloc(data1, 2);
	ASSERT_EQ(2, buf1.GetSize());
	ASSERT_EQ('a', buf1[0]);
	ASSERT_EQ('b', buf1[1]);
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_ByteBuffer, Resize)
{
	// 縮退
	char data1[] = "ab";
	ByteBuffer buf1(data1, 2);
	buf1.Resize(1);
	ASSERT_EQ(1, buf1.GetSize());
	ASSERT_EQ('a', buf1[0]);
	ASSERT_EQ('b', ((byte_t*)buf1.GetData())[1]);	// 再確保しないので元のが残っているはず

	// 元のサイズに戻す
	buf1.Resize(2);
	ASSERT_EQ(2, buf1.GetSize());
	ASSERT_EQ('a', buf1[0]);
	ASSERT_EQ('b', buf1[1]);	// 再確保しないので元のが残っているはず

	// 伸長
	buf1.Resize(3);
	ASSERT_EQ(3, buf1.GetSize());
	ASSERT_EQ('a', buf1[0]);
	ASSERT_EQ('b', buf1[1]);	// 元のがコピーされている
	ASSERT_EQ(0, buf1[2]);		// 末尾は 0 クリアされている

	// 縮退 → 伸長
	buf1.Resize(1);
	buf1.Resize(4);
	ASSERT_EQ('a', buf1[0]);	// 最初のは残っている
	ASSERT_EQ(0, buf1[1]);		// 末尾は 0 クリアされている
	ASSERT_EQ(0, buf1[2]);		// 末尾は 0 クリアされている
	ASSERT_EQ(0, buf1[3]);		// 末尾は 0 クリアされている
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_ByteBuffer, SetReferenceBuffer)
{
	char data2[2];
	ByteBuffer buf1;
	buf1.SetReferenceBuffer(data2, 2);
	buf1.Copy("ab", 2);
	ASSERT_EQ(2, buf1.GetSize());
	ASSERT_TRUE(TestUtils::CheckArrays(data2, buf1.GetData(), 2));
	ASSERT_EQ('a', buf1[0]);
	ASSERT_EQ('b', buf1[1]);
	ASSERT_EQ('a', data2[0]);	// 元バッファにも書き込まれている
	ASSERT_EQ('b', data2[1]);	// 元バッファにも書き込まれている
}

//-----------------------------------------------------------------------------
TEST_F(Test_Base_ByteBuffer, Copy_Clear)
{
	// 通常
	ByteBuffer buf1(2);
	buf1.Copy("ab", 2);
	ASSERT_EQ('a', buf1[0]);
	ASSERT_EQ('b', buf1[1]);

	// オフセット指定
	buf1.Copy(1, "c", 1);
	ASSERT_EQ('a', buf1[0]);
	ASSERT_EQ('c', buf1[1]);

	// 0 クリア
	buf1.Clear();
	ASSERT_EQ(0, buf1[0]);
	ASSERT_EQ(0, buf1[1]);
}
