
#pragma once

#include "../Base/RefObject.h"
#include "../Base/String.h"
#include "PathName.h"
#include "Stream.h"

namespace Lumino
{


// �t���O�ɂ܂Ƃ߂�ׂ��H
enum FileOpenMode
{
	FileOpenMode_NotOpen	= 0x0000,

	FileOpenMode_ReadOnly	= 0x0001,
	FileOpenMode_WriteOnly	= 0x0002,
	FileOpenMode_ReadWrite	= FileOpenMode_ReadOnly | FileOpenMode_WriteOnly,
	
	FileOpenMode_Open		= 0x0004,
	FileOpenMode_Create		= 0x0008,
	FileOpenMode_Append		= 0x0010,

	//FileOpenMode_Unbuffered = 0x0020,
};

/**
	@brief		�t�@�C��������s���N���X�ł��B
*/
class File
	: public Stream
{
public:

	/**
		@brief		�t�@�C�������w�肵�ăC���X�^���X�����������܂��B
		@details	���̃R���X�g���N�^�́A���ۂɃt�@�C���X�g���[�����J���܂���B
					�J���Ă��Ȃ���Ԃł� Stream �N���X�̋@�\���g�p���邱�Ƃ͂ł��܂���B
					�J���ɂ́AOpen() ���g�p���܂��B
	*/
	File(const String& filePath);
	
	/**
		@brief		�t�@�C�������w�肵�ăC���X�^���X�����������܂��B
		@details	���̃R���X�g���N�^�́A���ۂɃt�@�C���X�g���[�����J���܂���B
					�J���Ă��Ȃ���Ԃł� Stream �N���X�̋@�\���g�p���邱�Ƃ͂ł��܂���B
					�J���ɂ́AOpen() ���g�p���܂��B
	*/
	File(const PathName& filePath);

	virtual ~File() {}

public:



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
