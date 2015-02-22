
/*
	��[2015/2/6] ���b�Z�[�W���[�v�̕ʃX���b�h���ɂ���
		�E�ʃX���b�h�œ������ꍇ
			- �E�B���h�E�h���b�O�������Q�[���������s�\�B
			  �V�X�e���̓s���Ń��C���X���b�h���ɗ͒��f���Ȃ��ėǂ��B
			- �C�x���g�n���h���̓��C���X���b�h�Ŏ��s����B���̂��߁A�߂�l��Ԃ����Ƃ��ł��Ȃ��B
			  �Ⴆ�΁AClosing ���R�[���o�b�N�`���ŏ����ł��Ȃ��B
			  ���ʂ�GUI�A�v���������� OnClosing �� handled = true ��Ԃ��ăN���[�Y���L�����Z���ł����肷�邯�ǁA
			  ����͕s�\�ł���B
			  �Q�[���p�r�Ƃ��āA�Q�[����GUI�ŃN���[�Y�m�F����Ȃ炢���񂾂��ǁB
		�E���C���X���b�h�œ������ꍇ
			- ���ʂ� GUI �A�v���Ƃ��Ď��R�ȓ���B�������V���v���ɂȂ�B
			- �Q�[���p�r�Ƃ��Ă͂��܂胁���b�g�����B�Q�[����GUI�ŃN���[�Y�m�F����Ȃ�ǂ݂̂���x Closing �� Post ���Ȃ���΂Ȃ�Ȃ��B




	�����b�Z�[�W�����菇�ɂ���

		�V�X�e������ʒm���ꂽ���b�Z�[�W�́A��x���C�u�����̃C�x���g�ɕϊ����������� "Post" ����B
		�܂�A���C���X���b�h�Ɠ����X���b�h�Œʒm���ꂽ�C�x���g���Ƃ��Ă���x���C�u�����̃C�x���g�L���[��ʂ��B
		���̃C�x���g�L���[�̓X���b�h�Z�[�t�ɂ���
		�Ȃ��������邩�Ƃ����ƁA
		�EWin32 ���ŕʃX���b�h�Ń��b�Z�[�W�������s�����ƂŁA�E�B���h�E�h���b�O�������C���X���b�h�ŏ������s����悤�ɂ���B
		�EAndroid ���A���ɏo���オ���Ă���t���[�����[�N�ɑg�ݍ��ޏꍇ�A���C�����[�v���L�q���邱�Ƃ�����ɂȂ�ꍇ������B
		  ���̏ꍇ�̓Q�[���X���b�h���N�����Ă����Ƀ��C�����[�v���L�q���邪�A�C�x���g�������Ə������ʂ̃X���b�h�ɂȂ邱�ƂɂȂ�B

		���̓}���`�X���b�h�֌W�Ȃ̂ŁA�V���O���X���b�h�œ��삳����Ȃ� "Send" �ł�OK�B
		  
		
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
		m_mainWindowThreadInitFinished.Wait();	// �������I���܂őҋ@����
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
	// ���C���X���b�h�Ń��b�Z�[�W��������ꍇ�� InternalDoEvents
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
	// �ʃX���b�h�Ń��b�Z�\����肵�Ă����ꍇ�ق̓X���b�h�I���ҋ@
	if (m_useThread) {
		m_mainWindowThreadEndRequested.SetTrue();	// �I���v�������āA
		m_mainWindowThread.Wait();					// �҂�
	}
	// ���C���X���b�h�Ń��b�Z�[�W�������Ă����ꍇ�͂����� Destroy
	else {
		m_windowManager->Finalize();
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void PlatformManager::Thread_MainWindow()
{
	// ������
	m_windowManager->CreateMainWindow(m_windowCreationSettings);
	m_mainWindowThreadInitFinished.SetTrue();	// ����������

	// ���b�Z�[�W���[�v
	while (!m_mainWindowThreadEndRequested.IsTrue())
	{
		m_windowManager->DoEvents();
		Threading::Thread::Sleep(10);
	}

	// �I������
	m_windowManager->Finalize();
}

} // namespace Platform
} // namespace Lumino
