#include <TestConfig.h>
#include <Lumino/Base/ElapsedTimer.h>
#include <Lumino/Threading/Thread.h>

class Test_Base_ElapsedTimer : public ::testing::Test
{
};

//------------------------------------------------------------------------------
TEST_F(Test_Base_ElapsedTimer, Basic)
{
	ElapsedTimer timer;
	timer.Start();
	Thread::Sleep(100);
	uint64_t t = timer.GetElapsed();
	ASSERT_EQ(true, t >= 98 * 1000 * 1000);		// 微妙に早く終わることを考慮する
}
