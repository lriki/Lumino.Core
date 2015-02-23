
#include "../../Internal.h"
#include "Win32Window.h"
#include "Win32WindowManager.h"

namespace Lumino
{
namespace Platform
{
//=============================================================================
// Application
//=============================================================================
//PlatformManager* PlatformManager::Create()
//{
//	return LN_NEW Win32WindowManager();
//}

//=============================================================================
// Win32WindowManager
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32WindowManager::Win32WindowManager()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32WindowManager::~Win32WindowManager()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void Win32WindowManager::OnClosedWindow(Window* closedWindow)
//{
//	// 循環参照防止のため、Close されたら参照を外す
//	if (closedWindo/*w == mMainWindow) {
//		mMainWindow.*/SafeRelease();
//	}
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void Win32WindowManager::Exit()
//{
//	::PostQuitMessage(0);
//	PlatformManager::Exit();
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Win32WindowManager::CreateMainWindow(const WindowCreationSettings& settings)
{
	Win32Window::SettingData data;
	data.TitleText = settings.Title;
	data.Width = settings.ClientSize.Width;
	data.Height = settings.ClientSize.Height;
	data.Fullscreen = settings.Fullscreen;
	data.WinClassName = NULL;
	data.IconResourceID = NULL;
	data.Resizable = settings.Resizable;
	mMainWindow.Attach(LN_NEW Win32Window(this, data));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Win32WindowManager::DoEvents()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if (::GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_QUIT){
				m_endRequested = true;
			}
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Win32WindowManager::Finalize()
{
	mMainWindow.SafeRelease();
}

} // namespace Platform
} // namespace Lumino