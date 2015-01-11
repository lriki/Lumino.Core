
#pragma once

namespace Lumino
{
namespace Platform
{
class Window;

/// �V�X�e���C�x���g�̎��
enum EventType
{
	EventType_Unknown = 0,		///< �s���ȃC�x���g

	EventType_Quit,				///< �A�v���I���v��
	EventType_Close,			///< �E�B���h�E�������悤�Ƃ��Ă���
	//EventType_AppActivate,		///< �E�B���h�E���A�N�e�B�u�ɂȂ���
	//EventType_AppDeactivate = 4,	///< �E�B���h�E����A�N�e�B�u�ɂȂ���

	EventType_MouseDown,		///< [MouseEventArgs] �}�E�X�{�^���������ꂽ
	EventType_MouseUp,			///< [MouseEventArgs] �}�E�X�{�^���������ꂽ
	EventType_MouseMove,		///< [MouseEventArgs] �}�E�X���ړ�����
	EventType_MouseWheel,		///< [MouseEventArgs] �}�E�X�z�C�[�������삳�ꂽ

	EventType_KeyDown,			///< [MouseEventArgs] �L�[����
	EventType_KeyUp,			///< [MouseEventArgs] �L�[�����グ
	//EventType_KeyChar,			///< [MouseEventArgs] �������� (Ctrl��Alt�L�[�������ꂸ�A���{����� OFF �̏�ԂŃL�[��������ꍇ)

	//EventType_TOUCH_DOWN = 13,	///< [ LNEvent::Touch ] ��ʂ��^�b�`���ꂽ
	//EventType_TOUCH_UP = 14,	///< [ LNEvent::Touch ] ��ʂ��痣���ꂽ
	//EventType_TOUCH_MOVE = 15,	///< [ LNEvent::Touch ] �^�b�`���Ɉړ�����

	//EventType_WINDOW_SIZE_CHANGED,		///< �E�B���h�E�T�C�Y���ύX���ꂽ

	//EventType_ANDROID_PAUSED = 101,	///< Android APP_CMD_RESUME
	//EventType_ANDROID_RESUMED = 102,	///< Android APP_CMD_PAUSE
};

/// �}�E�X�{�^��
enum MouseButton
{
	MouseButton_Left = 0,		///< ���{�^��
	MouseButton_Right,			///< �E�{�^��
	MouseButton_Middle,			///< �����{�^��

	MouseButton_None = 0xff		///< �����܂��͉�����Ă��Ȃ����Ƃ�����
};

/// �L�[�R�[�h (NumPad �̓T�|�[�g���Ă��Ȃ�)
enum Key
{
	Key_Unknown = 0,

	Key_A,
	Key_B,
	Key_C,
	Key_D,
	Key_E,
	Key_F,
	Key_G,
	Key_H,
	Key_I,
	Key_J,
	Key_K,
	Key_L,
	Key_M,
	Key_N,
	Key_O,
	Key_P,
	Key_Q,
	Key_R,
	Key_S,
	Key_T,
	Key_U,
	Key_V,
	Key_W,
	Key_X,
	Key_Y,
	Key_Z,

	Key_0,
	Key_1,
	Key_2,
	Key_3,
	Key_4,
	Key_5,
	Key_6,
	Key_7,
	Key_8,
	Key_9,

	Key_F1,
	Key_F2,
	Key_F3,
	Key_F4,
	Key_F5,
	Key_F6,
	Key_F7,
	Key_F8,
	Key_F9,
	Key_F10,
	Key_F11,
	Key_F12,

	Key_Space,	// 0x20 = ' ' (�X�y�[�X)
	Key_Escape,
	Key_Up,
	Key_Down,
	Key_Left,
	Key_Right,
	Key_LShift,
	Key_RShift,
	Key_LCtrl,
	Key_RCtrl,
	Key_LAlt,
	Key_RAlt,
	Key_Tab,
	Key_Enter,
	Key_BackSpace,
	Key_Insert,
	Key_Delete,
	Key_PageUp,
	Key_PageDown,
	Key_Home,
	Key_End,

	Key_Colon,		///< :	[�񐄏� GLFW�ł�;]
	Key_SemiColon,	///< ;	[�񐄏� GLFW�ł�=]
	Key_Comma,		///< ,
	Key_Period,		///< .
	Key_Slash,		///< /
	Key_Minus,		///< -
	Key_BackSlash,	///< \(�o�b�N�X���b�V��) [�񐄏� GLFW�ł�NOWORD2]
	Key_Yen,		///< \(BaskSpace�L�[�̍���\�L�[) [�񐄏� GLFW�ł�I]
	Key_Caret,		///< ^ [�񐄏� GLFW�ł�GLFW_KEY_APOSTROPHE(')]
	Key_LBracket,	///< [
	Key_RBracket,	///< ]

	Key_Android_Plus,		///< Android '+'

	key_Max = 256,
};

/**
	@brief		�C�x���g�����̊��N���X
	@details	�C�x���g�ŗL�̈����ɃA�N�Z�X����ꍇ�́A�C�x���g�̎�ނɑΉ�����h���N���X�ɃL���X�g���܂��B
*/
class EventArgs
{
public:
	EventType	Type;		///< �C�x���g�̎��
	Window*		Sender;		///< �C�x���g�̑��M���E�B���h�E

public:
	EventArgs() {}
	virtual ~EventArgs() {}
};

/**
	@brief		�}�E�X�C�x���g�̈������ł��B
*/
class MouseEventArgs
	: public EventArgs
{
public:
	MouseButton	Button;		///< �{�^���ԍ�
	short		Delta;		///< �}�E�X�z�C�[���̉�]�� (windows �ł� 1 ��ɂ� 120 ���i�[����邪�A����͐��܂��͕��̉񐔂ŁA1�P��)
	short		X;			///< �}�E�X�C�x���g�������̃}�E�X�� X ���W (�N���C�A���g�̈�O�� -1)
	short		Y;			///< �}�E�X�C�x���g�������̃}�E�X�� Y ���W (�N���C�A���g�̈�O�� -1)
	short		MoveX;	    ///< X ���W�̈ړ���
	short		MoveY;      ///< Y ���W�̈ړ���

public:
	MouseEventArgs() {}
	virtual ~MouseEventArgs() {}
};

/**
	@brief		�L�[�{�[�h�C�x���g�̈������ł��B
*/
class KeyEventArgs
	: public EventArgs
{
public:
	Key			KeyCode;	///< �L�[�R�[�h
	bool		IsAlt;		///< Alt �L�[��������Ă���ꍇ true
	bool		IsShift;	///< Shift �L�[��������Ă���ꍇ true
	bool		IsControl;	///< Ctrl �L�[��������Ă���ꍇ true

public:
	KeyEventArgs() {}
	virtual ~KeyEventArgs() {}
};

} // namespace Platform
} // namespace Lumino

