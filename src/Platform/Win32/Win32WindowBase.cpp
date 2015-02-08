
/*
	[2015/01/10] ���b�Z�[�W�����͔񓯊��ɂ���ׂ����H

		����ׂ��ł͂Ȃ��B�E�B���h�E�h���b�O���ɕ`��X�V�Ȃǂ��ł��邪�A
		�ώG�ȃ��b�Z�[�W�Ǘ���R�[���o�b�N���K�v�ŁA�����b�g�����f�����b�g�̕����傫���B

		�f�����b�g
		�E�C�x���g�͕K�� PostEvent �ň�x���C���X���b�h�ɓn������A
		  ���C��������e Window �ɃA�^�b�`����Ă��� Listener �ɑ��M����K�v������B�������G�B
		�EPostEvent ���Ȃ���΂Ȃ�Ȃ��Ƃ������Ƃ́A�L���[�ɓ����C�x���g�f�[�^��
		  new �����I�u�W�F�N�g���T�C�Y�Œ�̍\���̂łȂ���΂Ȃ�Ȃ��B
		  �C�x���g1���Ƃ� new �̓R�X�g���������A�T�C�Y�Œ�\���̂͊g�����������B
		�EClosing �C�x���g�ŃA�v���̏I���m�F���b�Z�[�W�{�b�N�X�Ƃ����o���Ȃ��B
		  �c�[���n�ł̓��C���X���b�h���������܂܂��Ɠs���������B
		  ���Q�[���Ȃ�܂��������ǁB
*/


#include "../../Internal.h"
#include "Win32WindowManager.h"
#include "Win32WindowBase.h"

namespace Lumino
{
namespace Platform
{

//=============================================================================
// Win32WindowBase
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32WindowBase::Win32WindowBase(Win32WindowManager* app)
	: WindowBase(app)
	, mLastMouseX(-1)
	, mLastMouseY(-1)
	, mIsActive(false)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Win32WindowBase::~Win32WindowBase()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
LRESULT Win32WindowBase::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, bool* handled)
{
	*handled = false;

	// WM_SYSKEYDOWN �֌W���ނ�݂� postMessage() ����Ƌ����I������̂ŕK�v�Ȃ��̂����t�B���^�����O
	//if ( msg != WM_SYSKEYDOWN || (msg == WM_SYSKEYDOWN && wparam == VK_RETURN) )
	{
		// �}�E�X�L���v�`���̏���
		switch (msg)
		{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			::SetCapture(hwnd);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			::ReleaseCapture();
			break;
		}

		switch (msg)
		{
			/////////////////// �E�B���h�E�������悤�Ƃ��Ă���
			case WM_CLOSE:
			{
				/*
				������ DefWindowProc() ���Ă΂Ȃ��ŏI������B
				�������Ȃ��ƁA�~�{�^���������ꂽ�u�ԂɃE�B���h�E���j������āA
				�I�����O�̍Ō�̕`�掞�� DirectX �� EndScene() ��
				�u�`���̃E�B���h�E������܂���v�Ƃ������ƂŃG���[���o��B

				����ɏI���t���O�����𗧂ĂāA��������C�����[�v�Ō��o�A
				���̌�� finalize() �Ăяo���� DestroyWindow() ���Ăяo���B
				*/

				EventArgs e(EventType_Close, this);
				if (NortifyEvent(e)) {
					*handled = true;
					return 0;
				}

				// TODO
				if (this == m_windowManager->GetMainWindow()) {
					m_windowManager->Exit();
				}

				*handled = true;
				return 0;
			}

			/////////////////// �E�B���h�E���A�N�e�B�u�E��A�N�e�B�u�ɂȂ����ꍇ
			case WM_ACTIVATE:
			{
				bool active = ((wparam & 0xffff) != 0);
				if (active != mIsActive)
				{
					mIsActive = active;

					//EventArgs e;
					//e.Type = (mIsActive) ? LN_EVENT_APP_ACTIVATE : LN_EVENT_APP_DEACTIVATE;
					//e.Sender = this;
					//SendEventToAllListener(e);		// ���������̏ꍇ�͂��̏�Œʒm
					//mApplication->PostEvent(&e);	// �񓯊������̏ꍇ�͈�x�L���[�ɓ����
				}
				*handled = true;
				return 0;
			}
			/////////////////////////////////////////////// �E�B���h�E�T�C�Y���ύX���ꂽ
			case WM_SIZE:
			{
				//EventArgs e;
				//e.Type = LN_EVENT_WINDOW_SIZE_CHANGED;
				//e.Sender = this;
				//SendEventToAllListener(e);		// ���������̏ꍇ�͂��̏�Œʒm
				//mApplication->PostEvent(&e);	// �񓯊������̏ꍇ�͈�x�L���[�ɓ����

				//*handled = true;
				return 0;
			}
			/////////////////////////////////////////////// Alt + Enter �m�F
			//case WM_SYSKEYDOWN:
			//{
			//	if ( wparam == VK_RETURN )
			//	{
			//		EventArgs e;
			//		e.Type			= LN_EVENT_ALTENTER;
			//		e.Sender		= this;
			//		e.StructSize	= sizeof(EventArgs);
			//		mWindowManager->getManager()->postEventFromWindowThread( &e );

			//		return MAKELONG( -1, MNC_EXECUTE );     // �������̓��j���[�̑����B����̓��j���[�g���ĂȂ��̂łƂ肠�������̂܂�
			//	}
			//	// �����͈ȍ~�� DefWindowProc() ���ĂԁB
			//	// �������Ȃ��� Alt + F4 �Ƃ��������ɂȂ��Ă���ŏI���ł��Ȃ��Ȃ�B
			//	return 0;
			//}
			/////////////////////////////////////////////// Alt + Enter �̌x�����J�b�g http://www26.atwiki.jp/gunsyu3/pages/11.html
			case WM_SYSCHAR:
			{
				*handled = true;
				return 0;
			}
			/////////////////////////////////////////////// �}�E�X�{�^������
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			{
				EventArgs e;
				e.Sender = this;

				switch (msg)
				{
				case WM_LBUTTONDOWN:
					e.Type = EventType_MouseDown;
					e.Mouse.Button = MouseButton_Left;
					break;
				case WM_LBUTTONUP:
					e.Type = EventType_MouseUp;
					e.Mouse.Button = MouseButton_Left;
					break;
				case WM_RBUTTONDOWN:
					e.Type = EventType_MouseDown;
					e.Mouse.Button = MouseButton_Right;
					break;
				case WM_RBUTTONUP:
					e.Type = EventType_MouseUp;
					e.Mouse.Button = MouseButton_Right;
					break;
				case WM_MBUTTONDOWN:
					e.Type = EventType_MouseDown;
					e.Mouse.Button = MouseButton_Middle;
					break;
				case WM_MBUTTONUP:
					e.Type = EventType_MouseUp;
					e.Mouse.Button = MouseButton_Middle;
					break;
				}

				e.Mouse.X = LOWORD(lparam);
				e.Mouse.Y = HIWORD(lparam);
				e.Mouse.Delta = 0;
				e.Mouse.MoveX = (mLastMouseX >= 0) ? e.Mouse.X - mLastMouseX : 0;
				e.Mouse.MoveY = (mLastMouseY >= 0) ? e.Mouse.Y - mLastMouseY : 0;
				NortifyEvent(e);

				mLastMouseX = e.Mouse.X;
				mLastMouseY = e.Mouse.Y;

				*handled = true;
				return 0;
			}
			/////////////////////////////////////////////// �}�E�X�ړ�
			case WM_MOUSEMOVE:
			{
				EventArgs e;
				e.Type = EventType_MouseMove;
				e.Sender = this;
				e.Mouse.Button = MouseButton_None;
				e.Mouse.Delta = 0;
				e.Mouse.X = static_cast< short >(LOWORD(lparam));     // ��x short �ɃL���X�g���Ȃ��ƁA
				e.Mouse.Y = static_cast< short >(HIWORD(lparam));     // �}�C�i�X�l�ɂȂ����Ƃ� 65535 �Ƃ��l������
				e.Mouse.MoveX = (mLastMouseX >= 0) ? e.Mouse.X - mLastMouseX : 0;
				e.Mouse.MoveY = (mLastMouseY >= 0) ? e.Mouse.Y - mLastMouseY : 0;
				NortifyEvent(e);

				mLastMouseX = e.Mouse.X;
				mLastMouseY = e.Mouse.Y;

				*handled = true;
				return 0;
			}
			/////////////////////////////////////////////// �N���C�A���g�̈�O�Ń}�E�X���ړ�����
			case WM_NCMOUSEMOVE:
			{
				// �O�̂��߃z���g�ɃN���C�A���g�̈�O���`�F�b�N
				if (wparam != HTCLIENT)
				{
					EventArgs e;
					e.Type = EventType_MouseMove;
					e.Sender = this;
					e.Mouse.Button = MouseButton_None;
					e.Mouse.Delta = 0;
					e.Mouse.X = -1;
					e.Mouse.Y = -1;
					e.Mouse.MoveX = (mLastMouseX >= 0) ? e.Mouse.X - mLastMouseX : 0;
					e.Mouse.MoveY = (mLastMouseY >= 0) ? e.Mouse.Y - mLastMouseY : 0;
					NortifyEvent(e);

					mLastMouseX = e.Mouse.X;
					mLastMouseY = e.Mouse.Y;

					*handled = true;
					return 0;
				}
			}
			///////////////////////////////////////////// �}�E�X�z�C�[�������삳�ꂽ
			case WM_MOUSEWHEEL:
			{
				EventArgs e;
				e.Type = EventType_MouseWheel;
				e.Sender = this;
				e.Mouse.Button = MouseButton_None;
				e.Mouse.Delta = GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA;
				e.Mouse.X = static_cast< short >(LOWORD(lparam));
				e.Mouse.Y = static_cast< short >(HIWORD(lparam));
				e.Mouse.MoveX = (mLastMouseX >= 0) ? e.Mouse.X - mLastMouseX : 0;
				e.Mouse.MoveY = (mLastMouseY >= 0) ? e.Mouse.Y - mLastMouseY : 0;
				NortifyEvent(e);

				mLastMouseX = e.Mouse.X;
				mLastMouseY = e.Mouse.Y;

				*handled = true;
				return 0;
			}
			///////////////////////////////////////////// �L�[��
			case WM_KEYDOWN:
			{
				EventArgs e;
				e.Type = EventType_KeyDown;
				e.Sender = this;
				e.Key.KeyCode = ConvertVirtualKeyCode(wparam);	// ���z�L�[�R�[�h
				e.Key.IsAlt = ::GetKeyState(VK_MENU) < 0;
				e.Key.IsShift = ::GetKeyState(VK_SHIFT) < 0;
				e.Key.IsControl = ::GetKeyState(VK_CONTROL) < 0;
				NortifyEvent(e);

				*handled = true;
				return 0;
			}
			///////////////////////////////////////////// �L�[��
			case WM_KEYUP:
			{
				EventArgs e;
				e.Type = EventType_KeyUp;
				e.Sender = this;
				e.Key.KeyCode = ConvertVirtualKeyCode(wparam);	// ���z�L�[�R�[�h
				e.Key.IsAlt = ::GetKeyState(VK_MENU) < 0;
				e.Key.IsShift = ::GetKeyState(VK_SHIFT) < 0;
				e.Key.IsControl = ::GetKeyState(VK_CONTROL) < 0;
				NortifyEvent(e);

				*handled = true;
				return 0;
			}
			///////////////////////////////////////////// Alt + KeyDown
			case WM_SYSKEYDOWN:
			{
				EventArgs e;
				e.Type = EventType_KeyDown;
				e.Sender = this;
				e.Key.KeyCode = ConvertVirtualKeyCode(wparam);	// ���z�L�[�R�[�h
				e.Key.IsAlt = true;								// Alt on
				e.Key.IsShift = ::GetKeyState(VK_SHIFT) < 0;
				e.Key.IsControl = ::GetKeyState(VK_CONTROL) < 0;
				NortifyEvent(e);

				*handled = true;
				return 0;
			}
			///////////////////////////////////////////// Alt + KeyUp
			case WM_SYSKEYUP:
			{
				EventArgs e;
				e.Type = EventType_KeyUp;
				e.Sender = this;
				e.Key.KeyCode = ConvertVirtualKeyCode(wparam);	// ���z�L�[�R�[�h
				e.Key.IsAlt = true;								// Alt on
				e.Key.IsShift = ::GetKeyState(VK_SHIFT) < 0;
				e.Key.IsControl = ::GetKeyState(VK_CONTROL) < 0;
				NortifyEvent(e);
				break;	// WM_SYSKEYUP��߂܂����ꍇ�A�K��DefWindowProc�ɍs���悤�ɂ���
			}
			///////////////////////////////////////////// ��������
			//case WM_CHAR:
			//{
			//	// �����̂ݑ���
			//	if (0x20 <= wparam && wparam <= 0x7E)
			//	{
			//		KeyEventArgs e;
			//		e.Type = EventType_KeyChar;
			//		e.Sender = this;
			//		e.KeyCode = (Key)wparam;
			//		e.IsAlt = ::GetKeyState(VK_MENU) < 0;
			//		e.IsShift = ::GetKeyState(VK_SHIFT) < 0;
			//		e.IsControl = ::GetKeyState(VK_CONTROL) < 0;
			//		mWindowManager->getManager()->postEventFromWindowThread((EventArgs*)&e);

			//		return 0;
			//	}
			//}
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Win32WindowBase::NortifyEvent(const EventArgs& e)
{
	return SendEventToAllListener(e);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Key Win32WindowBase::ConvertVirtualKeyCode(DWORD winVK)
{
	if ('A' <= winVK && winVK <= 'Z') return (Key)(Key_A + (winVK - 'A'));
	if ('0' <= winVK && winVK <= '9') return (Key)(Key_0 + (winVK - '0'));
	if (VK_F1 <= winVK && winVK <= VK_F12) return (Key)(Key_F1 + (winVK - VK_F1));

	switch (winVK)
	{
	case VK_SPACE:
		return Key_Space;
	case VK_ESCAPE:
		return Key_Escape;
	case VK_UP:
		return Key_Up;
	case VK_DOWN:
		return Key_Down;
	case VK_LEFT:
		return Key_Left;
	case VK_RIGHT:
		return Key_Right;
	case VK_LSHIFT:
		return Key_LShift;
	case VK_RSHIFT:
		return Key_RShift;
	case VK_LCONTROL:
		return Key_LCtrl;
	case VK_RCONTROL:
		return Key_RCtrl;
	case VK_LMENU:
		return Key_LAlt;
	case VK_RMENU:
		return Key_RAlt;
	case VK_TAB:
		return Key_Tab;
	case VK_RETURN:
		return Key_Enter;
	case VK_BACK:
		return Key_BackSpace;
	case VK_INSERT:
		return Key_Insert;
	case VK_DELETE:
		return Key_Delete;
	case VK_PRIOR:
		return Key_PageUp;
	case VK_NEXT:
		return Key_PageDown;
	case VK_HOME:
		return Key_Home;
	case VK_END:
		return Key_End;

	case VK_OEM_1:
		return Key_Colon;
	case VK_OEM_PLUS:
		return Key_SemiColon;
	case VK_OEM_COMMA:
		return Key_Comma;
	case VK_OEM_PERIOD:
		return Key_Period;
	case VK_OEM_2:
		return Key_Slash;
	case VK_OEM_MINUS:
		return Key_Minus;
	case VK_OEM_102:
		return Key_BackSlash;
	case VK_OEM_5:
		return Key_Yen;
	case VK_OEM_7:
		return Key_Caret;
	case VK_OEM_4:
		return Key_LBracket;
	case VK_OEM_6:
		return Key_RBracket;
		/*
		case 'A': return Key_A;
		case 'B': return Key_B;
		case 'C': return Key_C;
		case 'D': return Key_D;
		case 'E': return Key_E;
		case 'F': return Key_F;
		case 'G': return Key_G;
		case 'H': return Key_H;
		case 'I': return Key_I;
		case 'J': return Key_J;
		case 'K': return Key_K;
		case 'L': return Key_L;
		case 'M': return Key_M;
		case 'N': return Key_N;
		case 'O': return Key_O;
		case 'P': return Key_P;
		case 'Q': return Key_Q;
		case 'R': return Key_R;
		case 'S': return Key_S;
		case 'T': return Key_T;
		case 'U': return Key_U;
		case 'V': return Key_V;
		case 'W': return Key_W;
		case 'X': return Key_X;
		case 'Y': return Key_Y;
		case 'Z': return Key_Z;

		Key_0                    = '0',
		Key_1                    = '1',
		Key_2                    = '2',
		Key_3                    = '3',
		Key_4                    = '4',
		Key_5                    = '5',
		Key_6                    = '6',
		Key_7                    = '7',
		Key_8                    = '8',
		Key_9                    = '9',

		Key_F1					= VK_F1,
		Key_F2					= VK_F2,
		Key_F3					= VK_F3,
		Key_F4					= VK_F4,
		Key_F5					= VK_F5,
		Key_F6					= VK_F6,
		Key_F7					= VK_F7,
		Key_F8					= VK_F8,
		Key_F9					= VK_F9,
		Key_F10					= VK_F10,
		Key_F11					= VK_F11,
		Key_F12					= VK_F12,
		*/
	}
	return Key_Unknown;
}


} // namespace Platform
} // namespace Lumino