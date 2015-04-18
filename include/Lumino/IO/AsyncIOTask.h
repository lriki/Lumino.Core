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
		@brief	�������e���������܂��B
	*/
	virtual void Execute() = 0;

	/**
		@brief	�񓯊������̏�Ԃ��擾���܂��B
	*/
	AsyncIOState GetAsyncIOState() const;
	
	/**
		@brief	�����I����A��O�����������ꍇ�͂��̗�O��Ԃ��܂��B
		@return	��O���������Ă��Ȃ���� NULL ��Ԃ��܂��B
	*/
	Exception* GetException() const;

private:
	friend class FileManager;
	AsyncIOState	m_state;
	Exception*		m_exception;
};

} // namespace Lumino
