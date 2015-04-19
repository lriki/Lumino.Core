/**
	@file	AsyncIOTask.h
*/
#pragma once

#include "Common.h"

namespace Lumino
{
class Exception;
class FileManager;

/**
	@brief	
*/
class AsyncIOTask
{
public:
	AsyncIOTask();
	virtual ~AsyncIOTask();

public:

	/**
		@brief		�������e���������܂��B
	*/
	virtual void Execute() = 0;

	/**
		@brief		�����I����A���̃N���X�� delete ���邩�������l��ݒ肵�܂��B
		@details	�����l�� true �ł��B
	*/
	void SetAutoDelete(bool autoDelete);

	/**
		@brief		�񓯊������̏�Ԃ��擾���܂��B
	*/
	AsyncIOState GetAsyncIOState() const;
	
	/**
		@brief		�����I����A��O�����������ꍇ�͂��̗�O��Ԃ��܂��B
		@return		��O���������Ă��Ȃ���� NULL ��Ԃ��܂��B
	*/
	Exception* GetException() const;

private:
	friend class FileManager;
	AsyncIOState	m_state;
	Exception*		m_exception;
	bool			m_autoDelete;
};

} // namespace Lumino
