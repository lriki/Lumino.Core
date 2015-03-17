
#pragma once

#include "../Base/Common.h"

namespace Lumino
{
namespace Platform
{
class NativeWindow;

/**
	@brief	プラットフォーム固有の機能にアクセスします。
*/
class PlatformSupport
{
public:
#ifdef LN_WIN32
	/**
		@brief		ウィンドウのウィンドウハンドルを取得します。
	*/
	static HWND GetWindowHandle(NativeWindow* window);
#endif
#ifdef LN_X11
	/**
		@brief		ウィンドウのウィンドウIDを取得します。
	*/
	static ::NativeWindow GetX11WindowID(NativeWindow* window);

	/**
		@brief		ウィンドウのビジュアルを取得します。
	*/
	static XVisualInfo* GetX11VisualInfo(NativeWindow* window);

	/**
		@brief		ウィンドウが接続されているXサーバを取得します。
	*/
	static Display* GetX11Display(NativeWindow* window);
#endif
};

} // namespace Platform
} // namespace Lumino
