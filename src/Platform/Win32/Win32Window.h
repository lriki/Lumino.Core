
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
	/// �������f�[�^
	struct SettingData
	{
		const TCHAR*	        TitleText;		///< �E�B���h�E�^�C�g���̕�����
		int				        Width;			///< �N���C�A���g�̈�̕�
		int				        Height;			///< �N���C�A���g�̈�̍���
		bool			        Fullscreen;		///< �t���X�N���[�����[�h�ŏ���������ꍇ true
		const TCHAR*	        WinClassName;   ///< �E�B���h�E�N���X�̖��O �܂��� NULL ( NULL �̏ꍇ�AL"_LNote_" ���g���� )
		int                     IconResourceID; ///< �^�C�g���o�[�̃A�C�R�����ʎq (IDI_ICON1 ��)
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
	String		mTitleText;			///< �E�B���h�E�^�C�g���̕�����    
	Size		mClientSize;        ///< �N���C�A���g�̈�̑傫��
	int			mOrginalWidth;      ///< initialize() �܂��� setSize() �Őݒ肳�ꂽ�N���C�A���g�̈�̕�
	int			mOrginalHeight;     ///< initialize() �܂��� setSize() �Őݒ肳�ꂽ�N���C�A���g�̈�̍���
	HWND		mWindowHandle;		///< �E�B���h�E�n���h��
	RECT		mWindowRect;		///< �E�B���h�E�̈�
	HACCEL		mAccelerators;      ///< �A�N�Z�����[�^ (Alt+Enter �̌x�����΍�̂��߂Ɏg���Ă�)
	DWORD		mWindowedStyle;		///< �E�B���h�E���[�h���̃X�^�C��
	bool		mFullScreen;        ///< �t���X�N���[�����[�h ( �p�̃X�^�C�� ) �̏ꍇ true

};

} // namespace Platform
} // namespace Lumino
