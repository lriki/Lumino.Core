
/*
	■[2015/2/6] メッセージループの別スレッド化について
		・別スレッドで動かす場合
			- ウィンドウドラッグ中等もゲーム処理続行可能。
			  システムの都合でメインスレッドを極力中断しなくて良い。
			- イベントハンドラはメインスレッドで実行する。そのため、戻り値を返すことができない。
			  例えば、Closing をコールバック形式で処理できない。
			  普通のGUIアプリだったら OnClosing で handled = true を返してクローズをキャンセルできたりするけど、
			  これは不可能である。
			  ゲーム用途として、ゲーム内GUIでクローズ確認するならいいんだけど。
		・メインスレッドで動かす場合
			- 普通の GUI アプリとして自然な動作。実装もシンプルになる。
			- ゲーム用途としてはあまりメリット無い。ゲーム内GUIでクローズ確認するならどのみち一度 Closing を Post しなければならない。




	■メッセージ処理手順について

		システムから通知されたメッセージは、一度ライブラリのイベントに変換したうえで "Post" する。
		つまり、メインスレッドと同じスレッドで通知されたイベントだとしても一度ライブラリのイベントキューを通す。
		このイベントキューはスレッドセーフにする
		なぜこうするかというと、
		・Win32 環境で別スレッドでメッセージ処理を行うことで、ウィンドウドラッグ中もメインスレッドで処理を行えるようにする。
		・Android 等、既に出来上がっているフレームワークに組み込む場合、メインループを記述することが困難になる場合がある。
		  その場合はゲームスレッドを起動してそこにメインループを記述するが、イベント発生元と処理が別のスレッドになることになる。

		問題はマルチスレッド関係なので、シングルスレッドで動作させるなら "Send" でもOK。
		  
		
*/

#include "../Internal.h"
#ifdef LN_WIN32
	#include "Win32/Win32WindowManager.h"
#endif
#ifdef LN_X11
	#include "X11/X11WindowManager.h"
#endif
#include "../../include/Lumino/Platform/PlatformManager.h"


namespace Lumino
{
namespace Platform
{

//=============================================================================
// WindowCreationSettings
//=============================================================================
WindowCreationSettings::WindowCreationSettings()
	: Title()
	, ClientSize(640, 480)
	, Fullscreen(false)
	, Resizable(true)
{}

PlatformManagerSettings::PlatformManagerSettings()
	: API(WindowSystemAPI_Win32API)
	, UseInternalUIThread(false)
{}

//=============================================================================
// PlatformManager
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PlatformManager::PlatformManager()
	: m_useThread(false)
	, m_windowManager(NULL)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PlatformManager::PlatformManager(const PlatformManagerSettings& settings)
	: m_useThread(false)
	, m_windowManager(NULL)
{
	Initialize(settings);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
PlatformManager::~PlatformManager()
{
	Dispose();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void PlatformManager::Initialize(const PlatformManagerSettings& settings)
{
	m_windowCreationSettings = settings.MainWindowSettings;
	m_useThread = settings.UseInternalUIThread;

#ifdef LN_WIN32
	m_windowManager = LN_NEW Win32WindowManager();
#endif
#ifdef LN_X11
	m_windowManager = LN_NEW X11WindowManager();
#endif

	if (m_useThread) {
		m_mainWindowThreadInitFinished.SetFalse();
		m_mainWindowThreadEndRequested.SetFalse();
		m_mainWindowThread.Start(LN_CreateDelegate(this, &PlatformManager::Thread_MainWindow));
		m_mainWindowThreadInitFinished.Wait();	// 初期化終了まで待機する
	}
	else {
		m_windowManager->CreateMainWindow(m_windowCreationSettings);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool PlatformManager::DoEvents()
{
	// メインスレッドでメッセージ処理する場合は InternalDoEvents
	if (!m_useThread) {
		m_windowManager->DoEvents();
	}

	return !m_windowManager->IsEndRequested();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void PlatformManager::Dispose()
{
	// 別スレッドでメッセ―じょりしていた場合異はスレッド終了待機
	if (m_useThread) {
		m_mainWindowThreadEndRequested.SetTrue();	// 終了要求だして、
		m_mainWindowThread.Wait();					// 待つ
	}
	// メインスレッドでメッセージ処理していた場合はここで Destroy
	else {
		m_windowManager->Finalize();
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void PlatformManager::Thread_MainWindow()
{
	// 初期化
	m_windowManager->CreateMainWindow(m_windowCreationSettings);
	m_mainWindowThreadInitFinished.SetTrue();	// 初期化完了

	// メッセージループ
	while (!m_mainWindowThreadEndRequested.IsTrue())
	{
		m_windowManager->DoEvents();
		Threading::Thread::Sleep(10);
	}

	// 終了処理
	m_windowManager->Finalize();
}

} // namespace Platform
} // namespace Lumino
