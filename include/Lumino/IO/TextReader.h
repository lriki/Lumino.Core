
#pragma once
#include "../Base/String.h"
#include "../Base/RefObject.h"

LN_NAMESPACE_BEGIN

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
	virtual int Peek() = 0;

	/**
		@brief		���݈ʒu�̕������擾���A���݈ʒu�����̕����Ɉړ����܂��B
		@return		EOF �ɓ��B���Ă��邩�G���[�����������ꍇ�� -1 ��Ԃ��܂��B
					�߂�l�� -1 �����`�F�b�N������ATCHAR �ɃL���X�g���邱�Ƃŕ����Ƃ��Ďg�p�ł��܂��B
	*/
	virtual int Read() = 0;

	/**
		@brief		���݈ʒu���� 1 �s���̕������ǂݎ��A���݈ʒu���ړ����܂��B
		@param[out]	line	: �ǂݎ������������i�[����ϐ��̃|�C���^ (���s�����͊܂܂Ȃ�)
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
		@brief		���݈ʒu�� EOF �ɓ��B���Ă��邩���m�F���܂��B
	*/
	virtual bool IsEOF() = 0;

};

LN_NAMESPACE_END
