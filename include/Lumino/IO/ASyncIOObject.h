/**
	@file	ASyncIOObject.h
*/
#pragma once

#include "../Base/RefObject.h"
#include "Common.h"

namespace Lumino
{
class Exception;
class FileManager;

/**
	@brief	
*/
class ASyncIOObject
	: public RefObject
{
public:
	ASyncIOObject();
	virtual ~ASyncIOObject();

public:

	/**
		@brief		�񓯊������̏�Ԃ��擾���܂��B
	*/
	ASyncIOState GetASyncIOState() const;
	
	/**
		@brief		�����I����A��O�����������ꍇ�͂��̗�O��Ԃ��܂��B
		@return		��O���������Ă��Ȃ���� NULL ��Ԃ��܂��B
	*/
	Exception* GetASyncIOException() const;

protected:

	/**
		@brief		�������e���������܂��B
		@details	���̊֐��͒��ڌĂяo���Ȃ��ł��������B
					RequestASyncIOProc() ���Ăяo�����ƂŁA�K�v�ȃ^�C�~���O�Ŏ��s����܂��B
	*/
	virtual void OnASyncIOProc() = 0;

	/**
		@brief		���������s���܂��B
	*/
	void InvokeIOProc(bool isASync, FileManager* fileManager = NULL);

private:
	friend class FileManager;
	ASyncIOState	m_ayncIOState;
	Exception*		m_ayncIOException;
};

} // namespace Lumino
