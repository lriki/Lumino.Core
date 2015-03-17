﻿
#pragma once

#include "../WindowManagerBase.h"
#include "Win32WindowBase.h"

namespace Lumino
{
namespace Platform
{
class Win32Window;

class Win32WindowManager
	: public WindowManagerBase
{
public:
	/// CreateNativeWindow の引数
	struct NativeWindowCreationData
	{
		const TCHAR*	        TitleText;		///< ウィンドウタイトルの文字列
		int				        Width;			///< クライアント領域の幅
		int				        Height;			///< クライアント領域の高さ
		bool			        Fullscreen;		///< フルスクリーンモードで初期化する場合 true
		bool					Resizable;
	};

	static const TCHAR*		PROP_WINPROC;		///< SetProp() で使用するキー文字列
	static const DWORD		FULLSCREEN_STYLE;	///< フルスクリーンモードのときのウィンドウスタイル

public:
	Win32WindowManager(int IconResourceID);
	virtual ~Win32WindowManager();

public:
	HINSTANCE GetInstanceHandle() const { return m_hInst; }
	const String& GetWindowClassName() const { return m_windowClassName; }

	Win32Window* CreateNativeWindow(const NativeWindowCreationData& data);

	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// クライアント領域のサイズが指定した大きさになるようにウィンドウサイズを調整する
	static void SetWindowClientSize(HWND hWnd, const Size& clientSize);

	/// ウィンドウをスクリーンの中央に移動する
	static void AbjustLocationCentering(HWND hWnd);

public:
	// override Application
	virtual void CreateMainWindow(const WindowCreationSettings& settings);
	virtual Window* GetMainWindow() { return mMainWindow; }
	virtual Window* CreateSubWindow(const WindowCreationSettings& settings);
	virtual void DoEvents();
	virtual void Finalize();

private:
	RefPtr<Win32WindowBase>	mMainWindow;
	HINSTANCE	m_hInst;
	HICON		m_hIcon;
	String		m_windowClassName;
};

} // namespace Platform
} // namespace Lumino
