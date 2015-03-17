
#pragma once

#include "../Base/RefObject.h"
#include "../Base/Size.h"
#include "EventListener.h"

namespace Lumino
{
namespace Platform
{

/**
	@brief	�E�B���h�E�V�X�e���̃E�B���h�E��\���N���X�ł��B
*/
class Window
	: public RefObject
{
public:
	/**
		@brief		�E�B���h�E���쐬���܂��B
		@param[in]	title		: �E�B���h�E�^�C�g��
		@param[in]	size		: �N���C�A���g�̈�̃s�N�Z���T�C�Y
		@param[in]	resizable	: �σE�B���h�E�Ƃ��č쐬���邩�ǂ���
	*/
	Window(const String& title, const Size& clientSize, bool resizable);
	virtual ~Window();

public:

	/**
		@brief		�N���C�A���g�̈�̃T�C�Y���擾���܂��B
	*/
	const Size& GetSize() const;
	
	/**
		@brief		�t���X�N���[���̗L���t���O��ݒ肵�܂��B
		@details	enabled �� true ��ݒ肷��ƃt���X�N���[���Afalse ��ݒ肷��ƃE�B���h�E���[�h�ɂȂ�܂��B
					���̊֐��̓E�B���h�E�X�^�C����ύX���čő剻���邾���ł��B
					���[�U�[��`�E�B���h�E�̏ꍇ�͂Ȃɂ����܂���B
	*/
	void SetFullScreenEnabled(bool enabled);

	/**
		@brief		�t���X�N���[����Ԃ��𔻒肵�܂��B
	*/
	bool IsFullScreenEnabled() const;

	/**
		@brief		�E�B���h�E���A�N�e�B�u���ǂ����������l���擾���܂��B
	*/
	bool IsActive() const;

	/**
		@brief		���̃E�B���h�E�Ƀ}�E�X�L���v�`����ݒ肵�܂��B
	*/
	void CaptureMouse();

	/**
		@brief		���̃E�B���h�E����}�E�X�L���v�`�����폜���܂��B
	*/
	void ReleaseMouseCapture();

	/**
		@brief		���̃E�B���h�E�ɃC�x���g���X�i�[���A�^�b�`���܂��B(priority ���傫�����̂���ɏ��������B�K���f�^�b�`���邱��)
	*/
	void AttachEventListener(IEventListener* listener, int priority);

	/**
		@brief		���̃E�B���h�E����C�x���g���X�i�[���f�^�b�`���܂��B
	*/
	void DetachEventListener(IEventListener* listener);

private:
	friend class Application;
	Window(NativeWindow* nativeWindow);
	NativeWindow*	m_nativeWindow;
};

} // namespace Platform
} // namespace Lumino



