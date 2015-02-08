
#pragma once

#include "../../../include/Lumino/Base/String.h"
#include "../../../include/Lumino/Base/Size.h"
#include "../WindowBase.h"

namespace Lumino
{
namespace Platform
{

class X11Window
	: public WindowBase
{
public:
	/// 初期化データ
	struct SettingData
	{
		const TCHAR*	TitleText;		///< ウィンドウタイトルの文字列
		Size			ClientSize;		///< クライアント領域のサイズ
		bool			Fullscreen;		///< フルスクリーンモードで初期化する場合 true
		bool			Resizable;
	};

public:
	X11Window(X11WindowManager* windowManager, const SettingData& settingData);
	virtual ~X11Window();

public:
	// override Window
	virtual const Size& GetSize() const { return mClientSize; }
	virtual void SetFullScreenEnabled(bool enabled);
	virtual bool IsFullScreenEnabled() const { return mFullScreen; }
	virtual void CaptureMouse();
	virtual void ReleaseMouseCapture();
	
private:
	void SetVisible(bool show);
	X11WindowManager* GetWindowManager() { return static_cast<X11WindowManager*>(m_windowManager); }

private:
	String		m_titleText;		///< ウィンドウタイトルの文字列
	Size		m_clientSize;		///< クライアント領域の大きさ
	
	::Window	m_x11Window;
};

} // namespace Platform
} // namespace Lumino
