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
	RefPtr<Application> app(Application::Create());
	app->CreateMainWindow(_T("test"), Size(640, 480));
#if 0
	while (app->DoEvents())
	{
	}
#endif
}

//---------------------------------------------------------------------
TEST_F(Test_Platform_Application, DoEvents)
{
	RefPtr<Application> app(Application::Create());

	int i = 0;
	while (app->DoEvents())
	{
		++i;
		if (i > 10) {	// “K“–‚É‰ñ‚µ‚½‚çI—¹’Ê’m‚ð‘—‚é
			app->Exit();
		}
	}
}
