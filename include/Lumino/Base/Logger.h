/**
	@file	Logger.h
*/
#pragma once

namespace Lumino
{

/**
	@brief		ログファイルのユーティリティです。
*/
class Logger
{
public:

	/// 通知レベル
	enum Level
	{
		Level_Info = 0,		///< 補足情報
		Level_Warning ,		///< 警告
		Level_Error,		///< エラー
	};

public:

	/**
		@brief		ログ機能を初期化し、ログファイルを新規作成します。
		@param[in]	filePath	: ログファイルのパス
		@return		true=成功 / false=失敗
	*/
	static bool Initialize(const TCHAR* filePath) throw();

	/**
		@brief		通知レベルを指定して書式指定メッセージを書き込みます。
		@details	ログ機能が初期化されていない場合は何もしません。
	*/
	static void WriteLine(Level level, const char* format, ...) throw();
	static void WriteLine(Level level, const wchar_t* format, ...) throw();		///< @overload WriteLine

};

} // namespace Lumino
