﻿/**
	@file	FileManager.h
*/
#pragma once

#include "Common.h"
#include "NonCopyable.h"

LN_NAMESPACE_BEGIN

/**
	@brief	DLL または 共有ライブラリをロードし、アクセスするためのクラスです。
*/
class DllLoader
	: public NonCopyable
{
public:

	/**
		@brief		コンストラクタ
		@details	別途 Load() 関数を呼び出して、ライブラリをロードする必要があります。
	*/
	DllLoader();

	/**
		@brief		指定したライブラリをロードし、インスタンスを初期化します。
		@param[in]	filePath	: ライブラリの名前またはパス
	*/
	DllLoader(const TCHAR* filePath);

	/**
		@brief		ロード済みのライブラリを解放します。
	*/
	~DllLoader();

public:

	/**
		@brief		指定したライブラリが存在するかを確認します。
		@return		アクセスできれば true、そうでなければ false
	*/
	static bool Exists(const TCHAR* filePath);

	/**
		@brief		指定したライブラリをロードします。
		@param[in]	filePath	: ライブラリの名前またはパス
	*/
	void Load(const TCHAR* filePath);

	/**
		@brief		ロード済みのライブラリを解放します。
		@details	ロードされていなければ何もしません。デストラクタでも実行されます。
	*/
	void Unload();

	/**
		@brief		ライブラリに含まれる関数のポインタを取得します。
	*/
	void* GetProcAddress(const char* procName);

private:
	void*	m_module;
};

LN_NAMESPACE_END
