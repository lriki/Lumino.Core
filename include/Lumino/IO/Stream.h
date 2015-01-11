
#pragma once

#include "../Base/RefObject.h"

namespace Lumino
{

/**
	@brief		�X�g���[���̃x�[�X�N���X
*/
class Stream
    : public RefObject
{
public:
	Stream() {}
	virtual ~Stream() {}

public:

	/**
		@brief	�ǂݎ����T�|�[�g���邩�ǂ���
	*/
	virtual bool CanRead() = 0;

	/**
		@brief	�������݂��T�|�[�g���邩�ǂ���
	*/
	virtual bool CanWrite() = 0;

	/**
		@brief	�X�g���[���� (�o�C�g�P��) �̎擾
	*/
	virtual size_t GetSize() = 0;
	
	/**
		@brief		�o�C�g �V�[�P���X��ǂݎ��A�ǂݎ�����o�C�g���̕������X�g���[���̌��݈ʒu��i�߂܂��B
		@param		duffer		: �ǂݎ�����f�[�^�̊i�[��A�h���X
		@param		bufferSize	: duffer �̃T�C�Y (�o�C�g���P��)
		@param		byteCount	: �ǂݎ��o�C�g��
		@return		���ۂɓǂݎ�����o�C�g���BEOF �ɓ��B���Ă���ꍇ�� 0 ��Ԃ��B
	*/
	virtual size_t Read(void* duffer, size_t bufferSize, size_t byteCount) = 0;

	/**
		@brief		���݂̃X�g���[���Ƀo�C�g �V�[�P���X���������݁A�������񂾃o�C�g���̕������X�g���[���̌��݈ʒu��i�߂܂��B
		@param		data		: �������ރf�[�^
		@param		byteCount	: �o�C�g��
		@details	�Œ蒷�o�b�t�@�ւ̏������ݓ��ŁA���݂̃V�[�N�ʒu���X�g���[���̖����Ɍ�������
					count �o�C�g�����߂��ʒu�ɂ���e�ʂ��g�[�ł��Ȃ��ꍇ�͗�O (NotSupportedException) ���������܂��B
	*/
	virtual void Write(const void* data, size_t byteCount) = 0;

	/**
		@brief	�X�g���[���̓����o�b�t�@�̃f�[�^��S�ă^�[�Q�b�g(�t�@�C����)�ɏ������݁A�����o�b�t�@���N���A����
	*/
	virtual void Flush() = 0;

};

} // namespace Lumino
