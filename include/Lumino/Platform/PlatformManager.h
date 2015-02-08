
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

/// �E�B���h�E�V�X�e���Ɏg�p����API�E���C�u����
enum WindowSystemAPI
{
	WindowSystemAPI_Win32API = 0,		///< WindowsAPI
	WindowSystemAPI_GLFW,
};

/// �E�B���h�E���쐬���邽�߂̐ݒ�
struct WindowCreationSettings
{
	String	Title;			///< �E�B���h�E�^�C�g�� (default:"")
	Size	ClientSize;		///< �N���C�A���g�̈�̃s�N�Z���T�C�Y (default:640,480)
	bool	Fullscreen;		///< �t���X�N���[�����[�h�ō쐬���邩�ǂ��� (default:false)
	bool	Resizable;		///< �σE�B���h�E�Ƃ��č쐬���邩�ǂ��� (default:true)

	WindowCreationSettings();
};

/// PlatformManager �����������邽�߂̐ݒ�
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
