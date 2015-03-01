
#pragma once

#include "../Base/Common.h"

namespace Lumino
{
namespace Platform
{
class Window;

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
	static HWND GetWindowHandle(Window* window);

	/**
		@brief		ウィンドウのデバイスコンテキストハンドルを取得します。
	*/
	static HDC GetWindowDC(Window* window);
#endif
};

} // namespace Platform
} // namespace Lumino
