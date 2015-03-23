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
	ApplicationSettings s;
	Application app(s);

	RefPtr<Window> win1(Window::Create(_T("test"), Size(200, 200), true));

#if 0
	while (app.DoEvents())
	{
	}
#endif
}

//---------------------------------------------------------------------
//TEST_F(Test_Platform_Application, DoEvents)
//{
//	ApplicationSettings s;
//	Application m(s);
//
//	int i = 0;
//	while (m.DoEvents())
//	{
//		++i;
//		if (i > 10) {	// 適当に回したら終了通知を送る
//			app->Exit();
//		}
//	}
//}

//---------------------------------------------------------------------
TEST_F(Test_Platform_Application, OnThread)
{
	ApplicationSettings s;
	s.UseInternalUIThread = true;
	Application app(s);
#if 0
	while (app.DoEvents())
	{
	}
#endif
}