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
	uint64_t t = timer.GetElapsedTimeNS();
	ASSERT_EQ(true, t >= 100 * 1000 * 1000);
}
