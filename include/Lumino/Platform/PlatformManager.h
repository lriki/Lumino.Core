
#pragma once

#include "../Base/RefObject.h"
#include "../Base/Size.h"
#include "../Base/String.h"
#include "../Base/Array.h"
#include "../Threading/Thread.h"

namespace Lumino
{
namespace Platform
{
class WindowManagerBase;

/// ウィンドウシステムに使用するAPI・ライブラリ
enum WindowSystemAPI
{
	WindowSystemAPI_Win32API = 0,		///< WindowsAPI
	WindowSystemAPI_GLFW,
};

/// ウィンドウを作成するための設定
struct WindowCreationSettings
{
	String	Title;			///< ウィンドウタイトル (default:"")
	Size	ClientSize;		///< クライアント領域のピクセルサイズ (default:640,480)
	bool	Fullscreen;		///< フルスクリーンモードで作成するかどうか (default:false)
	bool	Resizable;		///< 可変ウィンドウとして作成するかどうか (default:true)

	WindowCreationSettings();
};

/// PlatformManager を初期化するための設定
struct PlatformManagerSettings
{
	WindowSystemAPI			API;
	WindowCreationSettings	MainWindowSettings;
	bool					UseInternalUIThread;	///<  (default:false)

	PlatformManagerSettings();
};

class PlatformManager
	: public RefObject
{
	//public:
	//
	//	static PlatformManager* Create();

public:
	PlatformManager();
	PlatformManager(const PlatformManagerSettings& settings);
	virtual ~PlatformManager();

public:

	void Initialize(const PlatformManagerSettings& settings);
	// override Application
	//virtual void CreateMainWindow(const WindowCreationSettings& settings, bool useThread);
	bool DoEvents();

public:
	void Dispose();
	//void AddWindow(Window* window) { mWindowArray.Add(window); }
	//void RemoveWindow(Window* window) { mWindowArray.Remove(window); }

protected:
	//virtual Window* InternalCreateWindow(const WindowCreationSettings& settings) = 0;
	//virtual void InternalDoEvents() = 0;
	//virtual void InternalDestroyWindow(Window* window) = 0;

private:
	void Thread_MainWindow();

private:
	bool						m_useThread;
	WindowCreationSettings		m_windowCreationSettings;
	WindowManagerBase*			m_windowManager;
	Threading::DelegateThread	m_mainWindowThread;
	Threading::EventFlag		m_mainWindowThreadInitFinished;
	Threading::EventFlag		m_mainWindowThreadEndRequested;
};

} // namespace Platform
} // namespace Lumino
