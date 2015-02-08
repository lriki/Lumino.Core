#include <TestConfig.h>
using namespace Platform;

class Test_Platform_Application : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//---------------------------------------------------------------------
TEST_F(Test_Platform_Application, Basic)
{
	PlatformManagerSettings s;
	PlatformManager m(s);
#if 0
	while (app->DoEvents())
	{
	}
#endif
}

//---------------------------------------------------------------------
//TEST_F(Test_Platform_Application, DoEvents)
//{
//	PlatformManagerSettings s;
//	PlatformManager m(s);
//
//	int i = 0;
//	while (m.DoEvents())
//	{
//		++i;
//		if (i > 10) {	// �K���ɉ񂵂���I���ʒm�𑗂�
//			app->Exit();
//		}
//	}
//}

//---------------------------------------------------------------------
TEST_F(Test_Platform_Application, OnThread)
{
	PlatformManagerSettings s;
	s.UseInternalUIThread = true;
	PlatformManager m(s);
#if 0
	while (app->DoEvents())
	{
	}
#endif
}