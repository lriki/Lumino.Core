
#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>
#include <X11/keysym.h>
#include "../../../include/Lumino/Base/String.h"
#include "../../../include/Lumino/Base/Size.h"
#include "../WindowBase.h"

namespace Lumino
{
namespace Platform
{
class X11WindowManager;

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
	virtual bool IsActive() const { return true; }
	virtual const Size& GetSize() const { return m_clientSize; }
	virtual void SetFullScreenEnabled(bool enabled);
	virtual bool IsFullScreenEnabled() const { return false; }
	virtual void CaptureMouse();
	virtual void ReleaseMouseCapture();
	
private:
	void SetVisible(bool show);
	X11WindowManager* GetWindowManager() { return (X11WindowManager*)(m_windowManager); }

private:
	String		m_titleText;		///< �E�B���h�E�^�C�g���̕�����
	Size		m_clientSize;		///< �N���C�A���g�̈�̑傫��
	
	::Window	m_x11Window;
};

} // namespace Platform
} // namespace Lumino
