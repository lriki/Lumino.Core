
#pragma once

namespace Lumino
{
namespace Platform
{
class Window;

/// システムイベントの種類
enum EventType
{
	EventType_Unknown = 0,		///< 不明なイベント

	EventType_Quit,				///< アプリ終了要求
	EventType_Close,			///< ウィンドウが閉じられようとしている
	//EventType_AppActivate,		///< ウィンドウがアクティブになった
	//EventType_AppDeactivate = 4,	///< ウィンドウが非アクティブになった

	EventType_MouseDown,		///< [MouseEventArgs] マウスボタンが押された
	EventType_MouseUp,			///< [MouseEventArgs] マウスボタンが離された
	EventType_MouseMove,		///< [MouseEventArgs] マウスが移動した
	EventType_MouseWheel,		///< [MouseEventArgs] マウスホイールが操作された

	EventType_KeyDown,			///< [MouseEventArgs] キー押下
	EventType_KeyUp,			///< [MouseEventArgs] キー押し上げ
	//EventType_KeyChar,			///< [MouseEventArgs] 文字入力 (CtrlやAltキーが押されず、日本語入力 OFF の状態でキーが押され場合)

	//EventType_TOUCH_DOWN = 13,	///< [ LNEvent::Touch ] 画面がタッチされた
	//EventType_TOUCH_UP = 14,	///< [ LNEvent::Touch ] 画面から離された
	//EventType_TOUCH_MOVE = 15,	///< [ LNEvent::Touch ] タッチ中に移動した

	//EventType_WINDOW_SIZE_CHANGED,		///< ウィンドウサイズが変更された

	//EventType_ANDROID_PAUSED = 101,	///< Android APP_CMD_RESUME
	//EventType_ANDROID_RESUMED = 102,	///< Android APP_CMD_PAUSE
};

/// マウスボタン
enum MouseButton
{
	MouseButton_Left = 0,		///< 左ボタン
	MouseButton_Right,			///< 右ボタン
	MouseButton_Middle,			///< 中央ボタン

	MouseButton_None = 0xff		///< 無効または押されていないことを示す
};

/// キーコード (NumPad はサポートしていない)
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

	Key_Space,	// 0x20 = ' ' (スペース)
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

	Key_Colon,		///< :	[非推奨 GLFWでは;]
	Key_SemiColon,	///< ;	[非推奨 GLFWでは=]
	Key_Comma,		///< ,
	Key_Period,		///< .
	Key_Slash,		///< /
	Key_Minus,		///< -
	Key_BackSlash,	///< \(バックスラッシュ) [非推奨 GLFWではNOWORD2]
	Key_Yen,		///< \(BaskSpaceキーの左の\キー) [非推奨 GLFWではI]
	Key_Caret,		///< ^ [非推奨 GLFWではGLFW_KEY_APOSTROPHE(')]
	Key_LBracket,	///< [
	Key_RBracket,	///< ]

	Key_Android_Plus,		///< Android '+'

	key_Max = 256,
};

/**
	@brief		イベント引数の基底クラス
	@details	イベント固有の引数にアクセスする場合は、イベントの種類に対応する派生クラスにキャストします。
*/
class EventArgs
{
public:
	EventType	Type;		///< イベントの種類
	Window*		Sender;		///< イベントの送信元ウィンドウ

public:
	EventArgs() {}
	virtual ~EventArgs() {}
};

/**
	@brief		マウスイベントの引数情報です。
*/
class MouseEventArgs
	: public EventArgs
{
public:
	MouseButton	Button;		///< ボタン番号
	short		Delta;		///< マウスホイールの回転回数 (windows では 1 回につき 120 が格納されるが、これは正または負の回数で、1単位)
	short		X;			///< マウスイベント生成時のマウスの X 座標 (クライアント領域外は -1)
	short		Y;			///< マウスイベント生成時のマウスの Y 座標 (クライアント領域外は -1)
	short		MoveX;	    ///< X 座標の移動量
	short		MoveY;      ///< Y 座標の移動量

public:
	MouseEventArgs() {}
	virtual ~MouseEventArgs() {}
};

/**
	@brief		キーボードイベントの引数情報です。
*/
class KeyEventArgs
	: public EventArgs
{
public:
	Key			KeyCode;	///< キーコード
	bool		IsAlt;		///< Alt キーが押されている場合 true
	bool		IsShift;	///< Shift キーが押されている場合 true
	bool		IsControl;	///< Ctrl キーが押されている場合 true

public:
	KeyEventArgs() {}
	virtual ~KeyEventArgs() {}
};

} // namespace Platform
} // namespace Lumino

