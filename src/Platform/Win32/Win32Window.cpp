
#include "../../Internal.h"
#include "Win32WindowManager.h"
#include "Win32Window.h"

namespace Lumino
{
namespace Platform
{

//=============================================================================
// Win32Window
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32Window::Win32Window(Win32WindowManager* windowManager, HWND hWnd, DWORD hWindowedStyle, HACCEL hAccel, const String& title)
	: Win32WindowBase(windowManager)
	, mTitleText(title)
	, mOrginalWidth(0)
	, mOrginalHeight(0)
	, mWindowHandle(hWnd)
	, mAccelerators(hAccel)
	, mWindowedStyle(hWindowedStyle)
	, mFullScreen(false)
{
	// 初期化時のサイズ記憶
	RECT rc;
	GetClientRect(mWindowHandle, &rc);
	mClientSize.Width = rc.right - rc.left;
	mClientSize.Height = rc.bottom - rc.top;
	//mClientSize = clientSize;
	mOrginalWidth = mClientSize.Width;
	mOrginalHeight = mClientSize.Height;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32Window::~Win32Window()
{
	if (mWindowHandle)
	{
		::DestroyWindow(mWindowHandle);
		mWindowHandle = NULL;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Win32Window::SetVisible(bool visible)
{
	::ShowWindow(mWindowHandle, (visible) ? SW_SHOW : SW_HIDE);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Win32Window::SetFullScreenEnabled(bool flag)
{
	// フルスクリーンにする場合
	if (flag)
	{
		mFullScreen = true;
		::SetWindowLong(mWindowHandle, GWL_STYLE, Win32WindowManager::FULLSCREEN_STYLE);
		::SetWindowLong(mWindowHandle, GWL_EXSTYLE, 0);
		Resize(true);
	}
	// ウィンドウモードにする場合
	else
	{
		mFullScreen = false;
		Resize(false);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Win32Window::CaptureMouse()
{
	::SetCapture(mWindowHandle);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Win32Window::ReleaseMouseCapture()
{
	::ReleaseCapture();
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
void Win32Window::Resize(bool fullscreen)
{
	BOOL res;
	int x, y, sw, sh;
	int win_w = mOrginalWidth;
	int win_h = mOrginalHeight;

	// フルスクリーンモードの場合
	if (fullscreen)
	{
		/*
		// 一瞬塗りつぶす
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(mWindowHandle , &ps);

		//SelectObject(hdc , CreateSolidBrush(RGB(0xFF , 0 , 0)));
		PatBlt(hdc , 10 , 10 , 200 , 100 , PATCOPY);
		//DeleteObject(
		// SelectObject(
		//  hdc , GetStockObject(WHITE_BRUSH)
		// )
		//);

		EndPaint(mWindowHandle , &ps);
		*/

		// 画面いっぱい
		mClientSize.Width = ::GetSystemMetrics(SM_CXSCREEN);
		mClientSize.Height = ::GetSystemMetrics(SM_CYSCREEN);

		::SetForegroundWindow(mWindowHandle);

		x = (::GetSystemMetrics(SM_CXSCREEN) - mClientSize.Width) / 2;
		y = (::GetSystemMetrics(SM_CYSCREEN) - mClientSize.Height) / 2;

		//res = ::SetWindowPos( mWindowHandle, HWND_TOP, 0, 0, mWidth, mHeight, SWP_SHOWWINDOW );
		::SetWindowPos(mWindowHandle, NULL, x, y, mClientSize.Width, mClientSize.Height, SWP_SHOWWINDOW);
	}
	// ウィンドウモードの場合は実際にウィンドウの位置を設定する
	else
	{
		mClientSize.Width = mOrginalWidth;
		mClientSize.Height = mOrginalHeight;

		// 新しいクライアント領域の矩形を作成
		mWindowRect.left = 0;
		mWindowRect.top = 0;
		mWindowRect.right = win_w;
		mWindowRect.bottom = win_h;

		//// 念のため
		//::SetWindowLong(mWindowHandle, GWL_STYLE, mWindowedStyle);

		//// 枠の大きさも含めたウィンドウサイズを取得
		//res = ::AdjustWindowRect(&mWindowRect, mWindowedStyle, FALSE);

		//// 必要なウィンドウの幅と高さを計算
		//win_w = mWindowRect.right - mWindowRect.left;
		//win_h = mWindowRect.bottom - mWindowRect.top;

		//// ディスプレイ全体のサイズを取得
		//sw = ::GetSystemMetrics(SM_CXSCREEN);
		//sh = ::GetSystemMetrics(SM_CYSCREEN);

		//// ディスプレイの中央に表示できる座標を計算
		//x = (sw - win_w) / 2;
		//y = (sh - win_h) / 2;

		//// 位置と大きさを変更 (SWP_SHOWWINDOW を指定しないと、フルスクリーンからの復帰時に何も描画されなくなった)
		//res = ::SetWindowPos(mWindowHandle, NULL, x, y, win_w, win_h, SWP_SHOWWINDOW);
	}
}

} // namespace Platform
} // namespace Lumino