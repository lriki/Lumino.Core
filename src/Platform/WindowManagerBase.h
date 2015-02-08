
#pragma once

#include "../../include/Lumino/Platform/PlatformManager.h"

namespace Lumino
{
namespace Platform
{
class Window;

/**
	@note	[2015/2/8]
			�Ȃ�₩���� PlatformManager �� WindowManager �𕪂�������ɂ����B
			
			PlatformManager �ɑS���܂Ƃ߂�Ƃ���ƁA������p������ Win32Manager �Ƃ���
			��邱�ƂɂȂ�A�E�B���h�E�쐬�⃁�b�Z�[�W���[�v�̓T�u�N���X�Œ�`����B
			�v���b�g�t�H�[���̈Ⴂ���|�����[�t�B�Y���ŕ\������Ȃ炱�ꂪ���R�B

			�����A���̕��@���Ƃ���ɏ�̃��x���Ńv���b�g�t�H�[�����l������
			new �����Ȃ���΂Ȃ�Ȃ��B(����̓t�@�N�g���֐��g���΂܂����Ƃ��Ȃ邪)
			����ɁA�I���������f�X�g���N�^�ōs�����Ƃ��ł��Ȃ��B
			�udelete ����O�ɂ͕K�� Finalize() �̂悤�ȏI���������Ă�ł��������ˁv�ɂȂ�B

			���ɁA�X���b�h�֐����牼�z�֐����Ăяo���Ă��鎞�A�f�X�g���N�^�ŃX���b�h�I���҂��Ȃ�Ă��Ƃ�����ƁA
			pre call virtual function �Ƃ��ꌩ�킯�킩��Ȃ��G���[����������B

			LightNote �� Lumino �Ŋe���W���[�����ו������Č��J���邱�Ƃɂ����ȏ�A
			PlatformManager �͒P�̂Ŏg�����Ƃ�����B(���ɁA�����_�Ŏd���Ŏg�����Ă�����)
			�O���Ɍ��J����ȏ�A�\�Ȍ���V���v���ł���ׂ��B
*/
class WindowManagerBase
	: public RefObject
{
protected:
	WindowManagerBase() {}
	virtual ~WindowManagerBase() {}

public:
	virtual void CreateMainWindow(const WindowCreationSettings& settings) = 0;
	virtual Window* GetMainWindow() = 0;
	virtual void DoEvents() = 0;
	virtual void Finalize() = 0;

public:
	void AddWindow(Window* window) { m_windowArray.Add(window); }
	void RemoveWindow(Window* window) { m_windowArray.Remove(window); }
	bool IsEndRequested() const { return m_endRequested; }
	void Exit() { m_endRequested = true; }

protected:
	Array<Window*>			m_windowArray;
	bool					m_endRequested;
};

} // namespace Platform
} // namespace Lumino
