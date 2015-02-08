
#include "../../Internal.h"
#include "X11Window.h"

namespace Lumino
{
namespace Platform
{

//=============================================================================
// X11Window
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
X11Window::X11Window(X11WindowManager* windowManager, const SettingData& settingData)
	: WindowBase(windowManager)
	, m_x11Window(NULL)
{
	m_titleText = settingData.TitleText;
	m_clientSize = ClientSize;
	
	Display*	x11Display			= GetWindowManager()->GetX11Display();
	Screen*		x11DefaultScreen	= GetWindowManager()->GetX11DefaultScreen();
	::Window	x11RootWindow		= GetWindowManager()->GetX11RootWindow();
	
	//---------------------------------------------------------
	// XVisualInfo ���擾����
	//		�����ł� GLX �� API ���g�p���AOpenGL �Ŏg�������T�[�t�F�C�X�t�H�[�}�b�g����
	//		XVisualInfo ���擾���Ă���B
	//		OpenGL �g��Ȃ��ꍇ�� XMatchVisualInfo() ���A���ɂ��擾�����i�͂���B
	//
	//		���Ȃ݂ɁAglfw �̓��[�U�[�̎w���A�����g���@�\���T�|�[�g���Ă��邩���`�F�b�N���A
	//		�����R�[�h�������ʂ݂����Ɋe Visual ���ɃX�R�A��U���āA
	//		�x�X�g�� Visual �����߂悤�Ƃ��Ă����B
	//		�����܂ł�邩�͍���̗v�]�������ŁB�Ƃ肠������ԃx�[�V�b�N�� A8R8G8B�AD24S8 �Ŏ擾����B
	//		��glfx �ł� glXChooseVisual �͎g���Ă��Ȃ��BglXGetFBConfigs ���A�g�����g����Ƃ��� glxChooseFBConfigSGIX�B
	
	// http://earth.uni-muenster.de/~joergs/opengl/glXChooseVisual.html
	int32_t attributes[] =
	{
		//GLX_USE_GL,		// ���t�@�����X�ɂ� "���������" �Ƃ���B
		//GLX_LEVEL, 0,
		GLX_RGBA,			1,		// TrueColor �g�p
		GLX_DOUBLEBUFFER,	1,		// �_�u���o�b�t�@�����O�L��
		GLX_RED_SIZE,		8,		// R 8bit
		GLX_GREEN_SIZE,		8,		// G 8bit
		GLX_BLUE_SIZE,		8,		// B 8bit
		GLX_ALPHA_SIZE,		8,		// A 8bit
		GLX_DEPTH_SIZE,		24,		// �[�x�l 24bit
		GLX_STENCIL_SIZE,	8,		// �X�e���V�� 8bit
		//GLX_SAMPLES,        MultiSamples,	// �}���`�T���v�����O (�A���`�G�C���A�X)
		None
	};
	
	// http://manpages.ubuntu.com/manpages/gutsy/ja/man3/glXChooseVisual.3x.html
	// http://www.wakhok.ac.jp/~tatsuo/sen96/4shuu/section1.html
	XVisualInfo* visual = glXChooseVisual(x11Display, x11DefaultScreen, attributes);
	
	//---------------------------------------------------------
	// �J���[�}�b�v��I������B
	// 		�J���[�}�b�v�Ƃ́A�s�N�Z���t�H�[�}�b�g�̂悤�Ȃ��̂ŁA256�F �Ƃ� 65535�F���l�X�ȃp���b�g�����݂��Ă���B
	// 		����̎��Ԃ̓T�[�o���ɂ���AXCreateColormap() �͎w�肳�ꂽ visual ����œK�ȃJ���[�}�b�v�� ID ��Ԃ��Ă���B����ȃC���[�W�B
	// 		���̃J���[�}�b�v�� XCreateWindow() �Ɏw�肷�邱�ƂŁA�E�B���h�E�̃s�N�Z���t�H�[�}�b�g�����肷��B
	Colormap colorMap = XCreateColormap(x11Display, x11RootWindow, visual->visual, AllocNone);
	
	//---------------------------------------------------------
	// �E�B���h�E�̑��� (Win32 �̃E�B���h�E�X�^�C���݂����Ȃ���)
	//		http://homepage3.nifty.com/rio_i/lab/xlib/015attribute.htm
	XSetWindowAttributes winAttr;
	winAttr.colormap = colorMap;
	winAttr.border_pixel = 0;
	winAttr.override_redirect = False;
	
	// X�T�[�o����ǂ̃C�x���g���󂯎�肽���̂����w�肷��
	winAttr.event_mask = ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask;
	
	//---------------------------------------------------------
	// �E�B���h�E�����
	m_x11Window = XCreateWindow(
		x11Display,
		x11RootWindow,
		0, 0,
		m_clientSize.Width,
		m_clientSize.Height,
		0,                  // Border width
		visual->depth,      // Color depth
		InputOutput,
		visual->visual,
		CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,	// XSetWindowAttributes �̂ǂ̕������l�����邩
		&mWindowAttributes );
	
	
	XFree( visual );
	
	SetVisible(true);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
X11Window::~X11Window()
{
	if (m_x11Window)
	{
		XDestroyWindow(GetWindowManager()->GetX11Display(), m_x11Window);
		m_x11Window = 0;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void X11Window::SetFullScreenEnabled(bool flag)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void X11Window::CaptureMouse()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void X11Window::ReleaseMouseCapture()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void X11Window::SetVisible(bool show)
{
	// http://homepage3.nifty.com/rio_i/lab/xlib/001window.htm
	if (show)
	{
		XMapWindow( mXDisplay, mXWindow );
		XFlush( mXDisplay );
	}
	else
	{
		XUnmapWindow( mXDisplay, mXWindow );
		XFlush( mXDisplay );
	}
}

} // namespace Platform
} // namespace Lumino