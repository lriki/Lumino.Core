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
		@brief		処理内容を実装します。
	*/
	virtual void Execute() = 0;

	/**
		@brief		処理終了後、このクラスを delete するかを示す値を設定します。
		@details	初期値は true です。
	*/
	void SetAutoDelete(bool autoDelete);

	/**
		@brief		非同期処理の状態を取得します。
	*/
	AsyncIOState GetAsyncIOState() const;
	
	/**
		@brief		処理終了後、例外が発生した場合はその例外を返します。
		@return		例外が発生していなければ NULL を返します。
	*/
	Exception* GetException() const;

private:
	friend class FileManager;
	AsyncIOState	m_state;
	Exception*		m_exception;
	bool			m_autoDelete;
};

} // namespace Lumino
