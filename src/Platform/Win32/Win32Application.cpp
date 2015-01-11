
#include "../../Internal.h"
#include "Win32Window.h"
#include "Win32Application.h"

namespace Lumino
{
namespace Platform
{
//=============================================================================
// Application
//=============================================================================
Application* Application::Create()
{
	return LN_NEW Win32Application();
}

//=============================================================================
// Win32Application
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32Application::Win32Application()
	: mEndRequested(false)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32Application::~Win32Application()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void Win32Application::OnClosedWindow(Window* closedWindow)
//{
//	// èzä¬éQè∆ñhé~ÇÃÇΩÇﬂÅAClose Ç≥ÇÍÇΩÇÁéQè∆ÇäOÇ∑
//	if (closedWindo/*w == mMainWindow) {
//		mMainWindow.*/SafeRelease();
//	}
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Win32Application::CreateMainWindow(const TCHAR* title, Size clienSize, bool windowed, bool resizable)
{
	Win32Window::SettingData data;
	data.TitleText = title;
	data.Width = clienSize.Width;
	data.Height = clienSize.Height;
	data.Windowed = windowed;
	data.WinClassName = NULL;
	data.IconResourceID = NULL;
	data.Resizable = resizable;
	mMainWindow.Attach(LN_NEW Win32Window(this, data));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Win32Application::DoEvents()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if (::GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_QUIT){
				mEndRequested = true;
			}
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	return !mEndRequested;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Win32Application::Exit()
{
	::PostQuitMessage(0);
	mEndRequested = true;
}

} // namespace Platform
} // namespace Lumino