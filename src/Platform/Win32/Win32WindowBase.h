
#pragma once

#include <Platform/Window.h>
#include "../WindowBase.h"

namespace Lumino
{
namespace Platform
{
class Win32Application;

/**
	@brief	
*/
class Win32WindowBase
	: public WindowBase
{
public:
	Win32WindowBase(Win32Application* app);
	virtual ~Win32WindowBase();

public:
	// override Window
	virtual bool IsActive() const { return mIsActive; }

public:
	/// メッセージ処理のベース
	///		この処理はメインウィンドウの他、ユーザーウィンドウのホストクラスでも使用する。
	///		ホストの場合、ライブラリのユーザーが直接この関数を呼び出し、windows メッセージをライブラリに知らせる必要がある。
	///		ホストの場合、メッセージループおよび DefWndProc はユーザー側で呼ぶことになるのでこの中では行わない。代わりに handled でメッセージを処理したことを伝える。
	LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, bool* handled);

	/// ウィンドウハンドルの取得
	virtual HWND GetWindowHandle() = 0;

public:
	static Key ConvertVirtualKeyCode(DWORD winVK);

protected:
	int		mLastMouseX;
	int		mLastMouseY;
	bool	mIsActive;
};

} // namespace Platform
} // namespace Lumino
