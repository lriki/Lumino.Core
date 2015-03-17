
#pragma once

#include "../../../include/Lumino/Base/String.h"
#include "../../../include/Lumino/Base/Size.h"
#include "Win32WindowBase.h"

namespace Lumino
{
namespace Platform
{

class Win32Window
	: public Win32WindowBase
{
public:
	Win32Window(Win32WindowManager* windowManager, HWND hWnd, DWORD hWindowedStyle, HACCEL hAccel, const String& title);
	virtual ~Win32Window();

public:
	// override NativeWindow
	virtual const Size& GetSize() const { return mClientSize; }
	virtual void SetVisible(bool visible);
	virtual void SetFullScreenEnabled(bool enabled);
	virtual bool IsFullScreenEnabled() const { return mFullScreen; }
	virtual void CaptureMouse();
	virtual void ReleaseMouseCapture();

	// override Win32WindowBase
	virtual HWND GetWindowHandle() { return mWindowHandle; }

private:
	void Resize(bool fullscreen);

private:
	String		mTitleText;			///< ウィンドウタイトルの文字列  
	int			mOrginalWidth;      ///< initialize() または setSize() で設定されたクライアント領域の幅
	int			mOrginalHeight;     ///< initialize() または setSize() で設定されたクライアント領域の高さ
	HWND		mWindowHandle;		///< ウィンドウハンドル
	RECT		mWindowRect;		///< ウィンドウ領域
	HACCEL		mAccelerators;      ///< アクセラレータ (Alt+Enter の警告音対策のために使ってる)
	DWORD		mWindowedStyle;		///< ウィンドウモード時のスタイル
	bool		mFullScreen;        ///< フルスクリーンモード ( 用のスタイル ) の場合 true

};

} // namespace Platform
} // namespace Lumino
