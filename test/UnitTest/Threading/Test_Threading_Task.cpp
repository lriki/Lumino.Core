﻿#include <TestConfig.h>
using namespace ln::tr;

class IntegrateTest_Threading_Task : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static volatile int g_value = 0;

//---------------------------------------------------------------------
TEST_F(IntegrateTest_Threading_Task, Basic)
{
	g_value = 0;
	Threading::EventFlag flag(false);
	auto func = [&flag](){ g_value++; flag.Wait(); };

	int level = TaskScheduler::GetDefault()->GetMaxConcurrencyLevel();

	// 同時実行可能な数+1だけ Task を作る
	Array<TaskPtr> tasks;
	for (int i = 0; i < level + 1; ++i)
	{
		tasks.Add(Task::Run(Delegate<void()>(func)));
	}

	// 実行されているのは TaskScheduler::GetDefault()->GetMaxConcurrencyLevel() だけ
	//Threading::Thread::Sleep(200);
	while (g_value != level);
	ASSERT_EQ(level, g_value);
	ASSERT_EQ(TaskStatus::Running, tasks[0]->GetStatus());	// まだ lag.Wait(); で待機している

	flag.SetTrue();
	//Threading::Thread::Sleep(200);
	while (g_value != level + 1);
	ASSERT_EQ(level + 1, g_value);
	Threading::Thread::Sleep(200);
	for (int i = 0; i < level + 1; ++i)
	{
		ASSERT_EQ(true, tasks[i]->IsCompleted());
	}


	
}
