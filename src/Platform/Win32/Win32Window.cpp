
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

	// �E�B���h�E�A�C�R��
	DWORD dwExStyle = 0;
	HICON icon = NULL;
	if (settingData.IconResourceID) {
		icon = ::LoadIcon(inst, MAKEINTRESOURCE(settingData.IconResourceID));
	}
	else {
		dwExStyle = WS_EX_DLGMODALFRAME;	// �A�C�R���̖����X�^�C��
	}

	// �E�B���h�E�N���X�̐ݒ�
	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),			    // ���̍\���̂̃T�C�Y
		NULL,							    // �E�C���h�E�̃X�^�C��
		StaticWndProc,					    // ���b�Z�[�W�����֐�
		0, 0,							    // �ʏ�͎g��Ȃ��̂ŏ��0
		inst,				                // �C���X�^���X�n���h��
		icon,							    // �A�C�R��
		::LoadCursor(NULL, IDC_ARROW),	// �J�[�\���̌`
		(HBRUSH)(COLOR_WINDOW + 1),	    // �w�i�F
		NULL,							    // ���j���[�Ȃ�
		(settingData.WinClassName) ? settingData.WinClassName : WINDOW_CLASS_NAME,				// �N���X���̎w��
		NULL };							    // ���A�C�R���i�Ȃ��j

	// �E�B���h�E�N���X�̓o�^
	if (!::RegisterClassEx(&wcex))
	{
		LN_THROW(0, Win32Exception, GetLastError());
	}

	// �E�B���h�E�̍쐬
	mWindowHandle = ::CreateWindowEx(
		dwExStyle,
		(settingData.WinClassName) ? settingData.WinClassName : WINDOW_CLASS_NAME,
		mTitleText.c_str(),
		(settingData.Windowed) ? mWindowedStyle : FULLSCREEN_STYLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, inst, NULL);
	LN_THROW(mWindowHandle, Win32Exception, GetLastError());

	// �E�B���h�E�n���h���Ƃ��̃N���X�̃|�C���^���֘A�t����
	BOOL r = ::SetProp(mWindowHandle, PROP_WINPROC, this);
	LN_THROW((r != FALSE), Win32Exception, GetLastError());

	// �A�N�Z�����[�^�̍쐬
	ACCEL accels[1] =
	{
		{ FALT | FVIRTKEY, VK_RETURN, 0 }
	};
	mAccelerators = ::CreateAcceleratorTable(accels, 1);
	LN_THROW(mAccelerators, Win32Exception, GetLastError());

	// WM_PAINT���Ă΂�Ȃ��悤�ɂ���
	::ValidateRect(mWindowHandle, 0);

	// �E�B���h�E�̑傫����ύX����
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
	// �t���X�N���[���ɂ���ꍇ
	if (flag)
	{
		mFullScreen = true;
		::SetWindowLong(mWindowHandle, GWL_STYLE, FULLSCREEN_STYLE);
		::SetWindowLong(mWindowHandle, GWL_EXSTYLE, 0);
		Resize(true);
	}
	// �E�B���h�E���[�h�ɂ���ꍇ
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

	// �t���X�N���[�����[�h�̏ꍇ
	if (fullscreen)
	{
		/*
		// ��u�h��Ԃ�
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

		// ��ʂ����ς�
		mClientSize.Width = ::GetSystemMetrics(SM_CXSCREEN);
		mClientSize.Height = ::GetSystemMetrics(SM_CYSCREEN);

		::SetForegroundWindow(mWindowHandle);

		x = (::GetSystemMetrics(SM_CXSCREEN) - mClientSize.Width) / 2;
		y = (::GetSystemMetrics(SM_CYSCREEN) - mClientSize.Height) / 2;

		//res = ::SetWindowPos( mWindowHandle, HWND_TOP, 0, 0, mWidth, mHeight, SWP_SHOWWINDOW );
		::SetWindowPos(mWindowHandle, NULL, x, y, mClientSize.Width, mClientSize.Height, SWP_SHOWWINDOW);
	}
	// �E�B���h�E���[�h�̏ꍇ�͎��ۂɃE�B���h�E�̈ʒu��ݒ肷��
	else
	{
		mClientSize.Width = mOrginalWidth;
		mClientSize.Height = mOrginalHeight;

		// �V�����N���C�A���g�̈�̋�`���쐬
		mWindowRect.left = 0;
		mWindowRect.top = 0;
		mWindowRect.right = win_w;
		mWindowRect.bottom = win_h;

		// �O�̂���
		::SetWindowLong(mWindowHandle, GWL_STYLE, mWindowedStyle);

		// �g�̑傫�����܂߂��E�B���h�E�T�C�Y���擾
		res = ::AdjustWindowRect(&mWindowRect, mWindowedStyle, FALSE);

		// �K�v�ȃE�B���h�E�̕��ƍ������v�Z
		win_w = mWindowRect.right - mWindowRect.left;
		win_h = mWindowRect.bottom - mWindowRect.top;

		// �f�B�X�v���C�S�̂̃T�C�Y���擾
		sw = ::GetSystemMetrics(SM_CXSCREEN);
		sh = ::GetSystemMetrics(SM_CYSCREEN);

		// �f�B�X�v���C�̒����ɕ\���ł�����W���v�Z
		x = (sw - win_w) / 2;
		y = (sh - win_h) / 2;

		// �ʒu�Ƒ傫����ύX (SWP_SHOWWINDOW ���w�肵�Ȃ��ƁA�t���X�N���[������̕��A���ɉ����`�悳��Ȃ��Ȃ���)
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
			// �E�B���h�E�T�C�Y���E���Ă���
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