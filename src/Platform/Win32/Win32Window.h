
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
	/// 初期化データ
	struct SettingData
	{
		const TCHAR*	        TitleText;		///< ウィンドウタイトルの文字列
		int				        Width;			///< クライアント領域の幅
		int				        Height;			///< クライアント領域の高さ
		bool			        Fullscreen;		///< フルスクリーンモードで初期化する場合 true
		const TCHAR*	        WinClassName;   ///< ウィンドウクラスの名前 または NULL ( NULL の場合、L"_LNote_" が使われる )
		int                     IconResourceID; ///< タイトルバーのアイコン識別子 (IDI_ICON1 等)
		bool					Resizable;
	};

public:
	static const TCHAR*		WINDOW_CLASS_NAME;
	static const TCHAR*		PROP_WINPROC;
	static const DWORD		WINDOWED_STYLE;
	static const DWORD		FULLSCREEN_STYLE;

public:
	Win32Window(Win32WindowManager* windowManager, const SettingData& settingData);
	virtual ~Win32Window();

public:
	// override Window
	virtual const Size& GetSize() const { return mClientSize; }
	virtual void SetFullScreenEnabled(bool enabled);
	virtual bool IsFullScreenEnabled() const { return mFullScreen; }
	virtual void CaptureMouse();
	virtual void ReleaseMouseCapture();

	// override Win32WindowBase
	virtual HWND GetWindowHandle() { return mWindowHandle; }

private:
	void Resize(bool fullscreen);
	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	String		mTitleText;			///< ウィンドウタイトルの文字列    
	Size		mClientSize;        ///< クライアント領域の大きさ
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
