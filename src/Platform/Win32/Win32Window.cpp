
#include "../../Internal.h"
#include "Win32Window.h"

namespace Lumino
{
namespace Platform
{

//=============================================================================
// Win32Window
//=============================================================================

const TCHAR*	Win32Window::WINDOW_CLASS_NAME = _T("_LNote_");
const TCHAR*	Win32Window::PROP_WINPROC = _T("_LNoteProp_");
const DWORD		Win32Window::WINDOWED_STYLE = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;// | WS_THICKFRAME;
const DWORD		Win32Window::FULLSCREEN_STYLE = WS_POPUP;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32Window::Win32Window(Win32Application* app, const SettingData& settingData)
	: Win32WindowBase(app)
	, mOrginalWidth(640)
	, mOrginalHeight(480)
	, mWindowHandle(NULL)
	, mAccelerators(NULL)
	, mWindowedStyle(WINDOWED_STYLE)
	, mFullScreen(false)
{
	mTitleText = settingData.TitleText;
	mClientSize.Width = settingData.Width;
	mClientSize.Height = settingData.Height;
	mFullScreen = false;
	mOrginalWidth = mClientSize.Width;
	mOrginalHeight = mClientSize.Height;
	mWindowedStyle = WINDOWED_STYLE | ((settingData.Resizable) ? (WS_THICKFRAME | WS_MAXIMIZEBOX) : 0);

	HINSTANCE inst = (HINSTANCE)::GetModuleHandle(NULL);

	// ウィンドウアイコン
	DWORD dwExStyle = 0;
	HICON icon = NULL;
	if (settingData.IconResourceID) {
		icon = ::LoadIcon(inst, MAKEINTRESOURCE(settingData.IconResourceID));
	}
	else {
		dwExStyle = WS_EX_DLGMODALFRAME;	// アイコンの無いスタイル
	}

	// ウィンドウクラスの設定
	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),			    // この構造体のサイズ
		NULL,							    // ウインドウのスタイル
		StaticWndProc,					    // メッセージ処理関数
		0, 0,							    // 通常は使わないので常に0
		inst,				                // インスタンスハンドル
		icon,							    // アイコン
		::LoadCursor(NULL, IDC_ARROW),	// カーソルの形
		(HBRUSH)(COLOR_WINDOW + 1),	    // 背景色
		NULL,							    // メニューなし
		(settingData.WinClassName) ? settingData.WinClassName : WINDOW_CLASS_NAME,				// クラス名の指定
		NULL };							    // 小アイコン（なし）

	// ウィンドウクラスの登録
	if (!::RegisterClassEx(&wcex))
	{
		LN_THROW(0, Win32Exception, GetLastError());
	}

	// ウィンドウの作成
	mWindowHandle = ::CreateWindowEx(
		dwExStyle,
		(settingData.WinClassName) ? settingData.WinClassName : WINDOW_CLASS_NAME,
		mTitleText.c_str(),
		(settingData.Windowed) ? mWindowedStyle : FULLSCREEN_STYLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, inst, NULL);
	LN_THROW(mWindowHandle, Win32Exception, GetLastError());

	// ウィンドウハンドルとこのクラスのポインタを関連付ける
	BOOL r = ::SetProp(mWindowHandle, PROP_WINPROC, this);
	LN_THROW((r != FALSE), Win32Exception, GetLastError());

	// アクセラレータの作成
	ACCEL accels[1] =
	{
		{ FALT | FVIRTKEY, VK_RETURN, 0 }
	};
	mAccelerators = ::CreateAcceleratorTable(accels, 1);
	LN_THROW(mAccelerators, Win32Exception, GetLastError());

	// WM_PAINTが呼ばれないようにする
	::ValidateRect(mWindowHandle, 0);

	// ウィンドウの大きさを変更する
	Resize(!settingData.Windowed);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32Window::~Win32Window()
{
	if (mWindowHandle)
	{
		::DestroyWindow(mWindowHandle);
		//UnregisterClass( WINDOW_CLASS_NAME, mInstanceHandle );
		mWindowHandle = NULL;
	}
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
		::SetWindowLong(mWindowHandle, GWL_STYLE, FULLSCREEN_STYLE);
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

		// 念のため
		::SetWindowLong(mWindowHandle, GWL_STYLE, mWindowedStyle);

		// 枠の大きさも含めたウィンドウサイズを取得
		res = ::AdjustWindowRect(&mWindowRect, mWindowedStyle, FALSE);

		// 必要なウィンドウの幅と高さを計算
		win_w = mWindowRect.right - mWindowRect.left;
		win_h = mWindowRect.bottom - mWindowRect.top;

		// ディスプレイ全体のサイズを取得
		sw = ::GetSystemMetrics(SM_CXSCREEN);
		sh = ::GetSystemMetrics(SM_CYSCREEN);

		// ディスプレイの中央に表示できる座標を計算
		x = (sw - win_w) / 2;
		y = (sh - win_h) / 2;

		// 位置と大きさを変更 (SWP_SHOWWINDOW を指定しないと、フルスクリーンからの復帰時に何も描画されなくなった)
		res = ::SetWindowPos(mWindowHandle, NULL, x, y, win_w, win_h, SWP_SHOWWINDOW);
	}
}

//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
LRESULT CALLBACK Win32Window::StaticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//static int i = 0;
	//printf( "%d %4x\n", i++, msg_ );
	/*
	0x000C SPI_LANGDRIVER
	0x00AE ?
	0x007F SPI_SETMOUSEDOCKTHRESHOLD
	*/

	Win32Window* window = (Win32Window*)::GetProp(hwnd, PROP_WINPROC);

	if (window) {
		if (msg == WM_SIZE) {
			// ウィンドウサイズを拾っておく
			RECT rc;
			::GetClientRect(window->mWindowHandle, &rc);
			window->mClientSize.Set(rc.right, rc.bottom);
		}
		bool handled = false;
		LRESULT r = window->WndProc(hwnd, msg, wparam, lparam, &handled);
		if (!handled) {
			r = ::DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return r;
	}
	else {
		return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

} // namespace Platform
} // namespace Lumino