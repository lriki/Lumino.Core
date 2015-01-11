
#include "../Internal.h"
#include <Platform/Application.h>
#include <Platform/Window.h>

namespace Lumino
{
namespace Platform
{
	
//=============================================================================
// Window
//=============================================================================
//
////-----------------------------------------------------------------------------
////
////-----------------------------------------------------------------------------
//Window::Window(Application* app)
//{
//	mApplication = app;
//	mApplication->AddWindow(this);
//}
//
////-----------------------------------------------------------------------------
////
////-----------------------------------------------------------------------------
//Window::~Window()
//{
//	mApplication->RemoveWindow(this);
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Window::SendEventToAllListener(const EventArgs& e)
{
	LN_FOREACH(IEventListener* listener, mListenerEntryArray)
	{
		if (listener->OnEvent(e)) {
			return true;
		}
	}
	return false;
}

} // namespace Platform
} // namespace Lumino
