
#pragma once

#include "../Base/RefObject.h"

namespace Lumino
{

/**
	@brief	��A�̕�����ǂݎ�邱�Ƃ��ł��郊�[�_�[��\���܂��B
*/
class TextReader
	: public RefObject
{
public:
	virtual ~TextReader();
	
public:

	/**
		@brief		���݈ʒu�̕������擾���܂��B
		@return		EOF �ɓ��B���Ă��邩�G���[�����������ꍇ�� -1 ��Ԃ��܂��B
					�߂�l�� -1 �����`�F�b�N������ATCHAR �ɃL���X�g���邱�Ƃŕ����Ƃ��Ďg�p�ł��܂��B
	*/
	virtual int Peek() const = 0;

	/**
		@brief		���݈ʒu�̕������擾���A���݈ʒu�����̕����Ɉړ����܂��B
		@return		EOF �ɓ��B���Ă��邩�G���[�����������ꍇ�� -1 ��Ԃ��܂��B
					�߂�l�� -1 �����`�F�b�N������ATCHAR �ɃL���X�g���邱�Ƃŕ����Ƃ��Ďg�p�ł��܂��B
	*/
	virtual int Read() = 0;

	/**
		@brief		���݈ʒu���� 1 �s���̕������ǂݎ��A���݈ʒu���ړ����܂��B
		@param[out]	line	: �ǂݎ������������i�[����ϐ��̃A�h���X (���s�����͊܂܂Ȃ�)
		@return		���� EOF �ɓ��B���Ă���ꍇ�� false ��Ԃ��܂��B
		@details	line �� NULL �̏ꍇ�͌��݈ʒu�� 1 �s�����߂邾���ŁA�������Ԃ��܂���B
	*/
	virtual bool ReadLine(String* line) = 0;
	
	/**
		@brief		���݈ʒu����S�Ă̕������ǂݎ��܂��B
		@return		�ǂݎ����������
	*/
	virtual String ReadToEnd() = 0;

	/**
		@brief		Rader �̌��݈ʒu���擾���܂��B
		@details	���݈ʒu�͕����P�� (TCHAR) �ł��B�X�g���[���̃V�[�N�ʒu�Ƃ͈قȂ�_�ɒ��ӂ��Ă��������B
	*/
	virtual int GetPosition() const = 0;

	/**
		@brief		���݈ʒu�� EOF �ɓ��B���Ă��邩���m�F���܂��B
	*/
	virtual bool IsEOF() const = 0;

};

} // namespace Lumino
