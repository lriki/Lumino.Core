
#pragma once

#include <Platform/Window.h>
#include "../WindowBase.h"

namespace Lumino
{
namespace Platform
{
class Win32Application;

/**
	@brief	
*/
class Win32WindowBase
	: public WindowBase
{
public:
	Win32WindowBase(Win32Application* app);
	virtual ~Win32WindowBase();

public:
	// override Window
	virtual bool IsActive() const { return mIsActive; }

public:
	/// ���b�Z�[�W�����̃x�[�X
	///		���̏����̓��C���E�B���h�E�̑��A���[�U�[�E�B���h�E�̃z�X�g�N���X�ł��g�p����B
	///		�z�X�g�̏ꍇ�A���C�u�����̃��[�U�[�����ڂ��̊֐����Ăяo���Awindows ���b�Z�[�W�����C�u�����ɒm�点��K�v������B
	///		�z�X�g�̏ꍇ�A���b�Z�[�W���[�v����� DefWndProc �̓��[�U�[���ŌĂԂ��ƂɂȂ�̂ł��̒��ł͍s��Ȃ��B����� handled �Ń��b�Z�[�W�������������Ƃ�`����B
	LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, bool* handled);

	/// �E�B���h�E�n���h���̎擾
	virtual HWND GetWindowHandle() = 0;

public:
	static Key ConvertVirtualKeyCode(DWORD winVK);

protected:
	int		mLastMouseX;
	int		mLastMouseY;
	bool	mIsActive;
};

} // namespace Platform
} // namespace Lumino
