#include <TestConfig.h>

class Test_Thread : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//int gValue[4096];
//int gValue;

//---------------------------------------------------------------------
TEST_F(Test_Thread, Basic)
{
#if 0
	struct TestThread : public Thread
	{
		int tar;
		//int value;
		//EventFlag inited;
		volatile bool writed;
		volatile bool runnning;

		int gValue;
		volatile int*	gTest;

		TestThread(volatile int* tt/*int t*/)
		{
			//tar = t;
			////value = 0;
			//gValue[4000] = 0;
			////inited.SetFalse();
			//inited = false;
			writed = false;
			runnning = true;
			tar = 0;
			gTest = tt;
		}

		virtual void Execute()
		{
			SetThreadAffinityMask(GetCurrentThread(), 0x00000002); // ���݂̃X���b�h��1�Ԗڂ̃R�A�ɌŒ�

			while (runnning)
			{
				gValue = tar;
				*gTest = tar;
				tar++;
				writed = true;
				while (writed) {}	// �����Ă����܂ő҂�
			}

			//value = tar;
			//gValue[4000] = tar;
			//inited.SetTrue();
			//inited = true;
		}
	};

	SetThreadAffinityMask(GetCurrentThread(), 0x00000001); // ���݂̃X���b�h��0�Ԗڂ̃R�A�ɌŒ�

	volatile int gTest;
	TestThread testThread(&gTest);
	testThread.Start();

	

	for (int i = 0; i < 2000000; i++)
	{
		while (!testThread.writed) {}	// �������܂ő҂�

		//EXPECT_EQ(i, testThread.gValue);
		EXPECT_EQ(i, gTest);

		testThread.writed = false;

		if (i % 1000 == 0) {
			::Sleep(1);
		}
#if 0
		//EXPECT_EQ(0, testThread.value);
		EXPECT_EQ(0, gValue[4000]);
		gValue[4000] = -i;
		//byte_t str1[] = { 0x93, 0xFA, 0x96, 0x7B, 0x8C, 0xEA, 0x00 };
		//StringW str2;
		//str2.ConvertFrom(str1, 6, Encoding::GetSystemEncoding());


		//https://www.jpcert.or.jp/sc-rules/c-pos03-c.html


	

		//while (!testThread.inited.IsTrue()) {}
		while (!testThread.inited) {}
		//testThread.inited.Wait();
		//printf("%d\n", testThread.value);
		//EXPECT_EQ(i, testThread.value);
		EXPECT_EQ(i, gValue[4000]);
		//if (i != gValue)
		//{
		//	printf("i:%d gValue:%d\n", gValue);
		//}
		


#endif
	}

	testThread.writed = false;
	testThread.runnning = false;
	testThread.Wait();
#endif
}
