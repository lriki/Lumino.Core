
#pragma once

#include "../Base/RefObject.h"
#include "../Base/Size.h"
#include "EventListener.h"

namespace Lumino
{
namespace Platform
{

/**
	@brief	�v���b�g�t�H�[���ŗL�̃E�B���h�E�V�X�e���̃E�B���h�E�𒊏ۉ����܂��B
*/
class Window
	: public RefObject
{
public:

	/**
		@brief		�N���C�A���g�̈�̃T�C�Y���擾���܂��B
	*/
	virtual const Size& GetSize() const = 0;
	
	/**
		@brief		�t���X�N���[���̗L���t���O��ݒ肵�܂��B
		@details	enabled �� true ��ݒ肷��ƃt���X�N���[���Afalse ��ݒ肷��ƃE�B���h�E���[�h�ɂȂ�܂��B
					���̊֐��̓E�B���h�E�X�^�C����ύX���čő剻���邾���ł��B
					���[�U�[��`�E�B���h�E�̏ꍇ�͂Ȃɂ����܂���B
	*/
	virtual void SetFullScreenEnabled(bool enabled) = 0;

	/**
		@brief		�t���X�N���[����Ԃ��𔻒肵�܂��B
	*/
	virtual bool IsFullScreenEnabled() const = 0;

	/**
		@brief		�E�B���h�E���A�N�e�B�u���ǂ����������l���擾���܂��B
	*/
	virtual bool IsActive() const = 0;

	/**
		@brief		���̃E�B���h�E�Ƀ}�E�X�L���v�`����ݒ肵�܂��B
	*/
	virtual void CaptureMouse() = 0;

	/**
		@brief		���̃E�B���h�E����}�E�X�L���v�`�����폜���܂��B
	*/
	virtual void ReleaseMouseCapture() = 0;

	/**
		@brief		���̃E�B���h�E�ɃC�x���g���X�i�[���A�^�b�`���܂��B(priority ���傫�����̂���ɏ��������B�K���f�^�b�`���邱��)
	*/
	virtual void AttachEventListener(IEventListener* listener, int priority) = 0;

	/**
		@brief		���̃E�B���h�E����C�x���g���X�i�[���f�^�b�`���܂��B
	*/
	virtual void DetachEventListener(IEventListener* listener) = 0;

protected:
	Window() {}
	virtual ~Window() {}
};

} // namespace Platform
} // namespace Lumino



