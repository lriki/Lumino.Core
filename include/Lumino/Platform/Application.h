
#pragma once

#include "../Base/RefObject.h"
#include "../Base/Size.h"

namespace Lumino
{
namespace Platform
{
class Window;

class Application
	: public RefObject
{
public:

	static Application* Create();

public:
	
	/**
		@brief		���C���E�B���h�E���쐬���܂��B
		@details	���C���E�B���h�E�� 1 �̃A�v���P�[�V�����ɂ� 1 �����쐬�ł��܂��B
					�쐬�����E�B���h�E�� GetMainWindow() �Ŏ擾�ł��܂��B
	*/
	virtual void CreateMainWindow(const TCHAR* title, Size clienSize, bool windowed = true, bool resizable = true) = 0;

	/**
		@brief		���C���E�B���h�E���擾���܂��B
	*/
	virtual Window* GetMainWindow() = 0;

	/**
		@brief	���b�Z�[�W�L���[�Ɍ��݂��郁�b�Z�[�W�����ׂď������܂��B
	*/
	virtual bool DoEvents() = 0;

	/**
		@brief	���b�Z�[�W�L���[�ɃA�v���P�[�V�����̏I����ʒm���܂��B
	*/
	virtual void Exit() = 0;

protected:
	Application() {}
	virtual ~Application() {}
};

} // namespace Platform
} // namespace Lumino
