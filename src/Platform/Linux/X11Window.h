
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
	/// �������f�[�^
	struct SettingData
	{
		const TCHAR*	TitleText;		///< �E�B���h�E�^�C�g���̕�����
		Size			ClientSize;		///< �N���C�A���g�̈�̃T�C�Y
		bool			Fullscreen;		///< �t���X�N���[�����[�h�ŏ���������ꍇ true
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
	String		m_titleText;		///< �E�B���h�E�^�C�g���̕�����
	Size		m_clientSize;		///< �N���C�A���g�̈�̑傫��
	
	::Window	m_x11Window;
};

} // namespace Platform
} // namespace Lumino
