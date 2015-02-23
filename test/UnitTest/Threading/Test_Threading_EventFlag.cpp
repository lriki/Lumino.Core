#include <TestConfig.h>
using namespace Threading;

class Test_Threading_EventFlag : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

int gValue;

//---------------------------------------------------------------------
TEST_F(Test_Threading_EventFlag, Basic)
{
	struct TestThread : public Thread
	{
	public:
		TestThread()
			: mInited(false)
			, mRunning(false)
		{
		}

		virtual void Execute()
		{
			gValue++;
			mInited.SetTrue();

			do
			{
				gValue++;
				Thread::Sleep(10);
			} while (mRunning.IsTrue());
		}

		EventFlag mInited;
		EventFlag mRunning;
	};

	gValue = 0;

	TestThread t;
	t.Execute();

	t.mInited.Wait();	// 初期化完了待ち
	ASSERT_TRUE(gValue >= 2);


	t.mRunning.SetFalse();	// 終了要求
	t.Wait();
	ASSERT_TRUE(gValue >= 2);
}

//---------------------------------------------------------------------
TEST_F(Test_Threading_EventFlag, Basic2)
{
	EventFlag f1;
	EventFlag f2(true);
	EventFlag f3(false);
	ASSERT_FALSE(f1.IsTrue());
	ASSERT_TRUE(f2.IsTrue());
	ASSERT_FALSE(f3.IsTrue());

	f2.Wait();	// true なので止まらずに進む
}
