
#pragma once

#include "../Base/Array.h"
#include "EventArgs.h"

namespace Lumino
{
namespace Platform
{

/**
	@brief	�v���b�g�t�H�[���ŗL�̃E�B���h�E�V�X�e���̃E�B���h�E�𒊏ۉ����܂��B
*/
class IEventListener
{
public:

	/**
		@brief	���b�Z�[�W�����R�[���o�b�N
		@return true �̏ꍇ�A�Ăяo�����ő��̏������s�킸�ɁA�����ɏ������I�����܂��B
	*/
	virtual bool OnEvent(const EventArgs& e) = 0;

protected:
	virtual ~IEventListener() {}
};

} // namespace Platform
} // namespace Lumino



