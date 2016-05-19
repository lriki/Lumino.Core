﻿#include <TestConfig.h>

class IntegrateTest_Threading_Semaphore : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static int g_value = 0;

//---------------------------------------------------------------------
TEST_F(IntegrateTest_Threading_Semaphore, Basic)
{
	Semaphore sem(2, 2);

	ConditionFlag flag(false);
	Threading::DelegateThread thr1, thr2, thr3, thr4;
	
	g_value = 0;
	auto func = [&sem, &flag](){ sem.Lock(); g_value++; flag.Wait(); sem.Unlock(); };

	thr1.Start(Delegate<void()>(func));
	thr2.Start(Delegate<void()>(func));
	thr3.Start(Delegate<void()>(func));
	thr4.Start(Delegate<void()>(func));

	Threading::Thread::Sleep(100);

	// この時点で2つはLock()で止まっている
	ASSERT_EQ(2, g_value);

	// 先に flag.Wait() で待っているものを動かす→Unlock()にたどり着く
	flag.SetTrue();

	Threading::Thread::Sleep(100);
	ASSERT_EQ(4, g_value);
}
