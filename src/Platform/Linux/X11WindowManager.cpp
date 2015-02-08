
#include "../../Internal.h"
#include "Win32Window.h"
#include "X11WindowManager.h"

// http://homepage3.nifty.com/rio_i/lab/xlib/013wm.htm

namespace Lumino
{
namespace Platform
{

//=============================================================================
// X11WindowManager
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
X11WindowManager::X11WindowManager()
	: m_x11Display(NULL)
	, m_x11DefaultScreen(NULL)
	, m_x11RootWindow(NULL)
{
	// X11 �� API ���X���b�h�Ή��ɂ���B���� API �Ăяo���O�ɌĂ�ł����K�v������B
	XInitThreads();

	m_x11Display = XOpenDisplay(NULL);
	LN_THROW(m_x11Display != NULL, InvalidOperationException);
	
	m_x11Screen = DefaultScreen(m_x11Display);
	m_x11Root = RootWindow(m_x11Display, m_x11Screen);
	m_x11Context = XUniqueContext();
	
	
	// ��X11 �ɂ�����u�A�g���v�Ƃ́A�^�C�g�����̃E�B���h�E�v���p�e�B��\�����j�[�NID
	
	
	
	// create window ----
	
	
	
	
	// http://homepage3.nifty.com/rio_i/lab/xlib/015attribute.htm#15_1_7
	
	
	//XSaveContext
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
X11WindowManager::~X11WindowManager()
{
	if (m_x11Display) {
		XCloseDisplay(m_x11Display);
		m_x11Display = NULL;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void X11WindowManager::OnClosedWindow(Window* closedWindow)
//{
//	// �z�Q�Ɩh�~�̂��߁AClose ���ꂽ��Q�Ƃ��O��
//	if (closedWindo/*w == mMainWindow) {
//		mMainWindow.*/SafeRelease();
//	}
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void X11WindowManager::Exit()
//{
//	::PostQuitMessage(0);
//	PlatformManager::Exit();
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void X11WindowManager::CreateMainWindow(const WindowCreationSettings& settings)
{
	X11Window::SettingData data;
	data.TitleText = settings.Title;
	data.ClientSize = settings.ClientSize;
	data.Fullscreen = settings.Fullscreen;
	data.Resizable = settings.Resizable;
	m_mainWindow.Attach(LN_NEW X11Window(this, data));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void X11WindowManager::DoEvents()
{
	while (XPending (m_x11Display) > 0)
	{
		XEvent xev;
		XNextEvent(m_x11Display, &xev);

		// �����̂ւ�� Manager::WndProc �ɉ񂹂�Ƃ������������B
/*
		switch ( xev.type )
		{
			case ClientMessage:
			{
				// �N���[�Y�{�^���������ꂽ�ꍇ
				if ( mX11MainWindow->checkCloseButtonOfThis(
				xev.xclient.message_type,
				xev.xclient.data.l[ 0 ] ) )
				{
				LNEvent lnev;
				lnev.Type = LNEV_CLOSE;
				this->postMessage( lnev );
				}
				continue;
			}
		}

		// ���C�u�����ɕK�v�ȃ��b�Z�[�W�̏ꍇ
		if ( X11::checkNecessaryMessage( xev ) )
		{
		LNEvent lnev;
		bool handled;
		X11::translateMessageToLN( &lnev, xev, &handled );
		this->postMessage( lnev );
		}
		*/
	}
	
	/*
	while( 1 ) {
   63			XNextEvent( dpy, &evt );
   64	
   65			switch ( evt.type ) {
   66				case KeyPress:
   67					/ ���\�[�X�̉�� /
   68					XDestroyWindow( dpy, win );
   69					XCloseDisplay( dpy );
   70					return 0;
   71			}
   72		}
	*/
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void X11WindowManager::Finalize()
{
	mMainWindow.SafeRelease();
}

} // namespace Platform
} // namespace Lumino