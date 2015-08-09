/**
	@file	Environment.h
*/

#pragma once
#include "EnumExtension.h"

namespace Lumino
{

/// エンディアンを示す値
enum ByteOrder
{
	ByteOrder_Little = 0,	///< リトルエンディアン
	ByteOrder_Big,			///< ビッグエンディアン
};

/// システムの特別なフォルダパスを示す値
LN_ENUM(SpecialFolder)
{
	ApplicationData = 0,
	Temporary,
};
LN_ENUM_DECLARE(SpecialFolder);

/**
	@brief	現在の環境に関する情報にアクセスするためのクラスです。
*/
class Environment
{
public:

	/**
		@brief	現在の環境のエンディアンを確認します。
	*/
	static ByteOrder GetByteOrder();

	/**
		@brief	リトルエンディアン環境であるかを確認します。
	*/
	static bool IsLittleEndian();

	/**
		@brief		システム起動からの時間を ms 単位で取得します。
	*/
	static uint64_t GetTickCount();

	/**
		@brief		システム起動からの時間を ns 単位で取得します。
	*/
	static uint64_t GetTickCountNS();

	/**
		@brief		現在の環境で定義されている改行文字列を取得します。
	*/
	template<typename TChar>
	static const TChar* GetNewLine();

	/**
		@details	
		SpecialFolder   | Windows                   | OS X    | Linux
		----------------|---------------------------|---------|---------
		ApplicationData | <USER>/AppData/Roaming    |         | ~/.local/share
		                |                           |         | /usr/local/share
		                |                           |         | /usr/share/
		Temporary       | <USER>/AppData/Local/Temp |         | /tmp
	*/
	template<typename TChar>
	static void GetSpecialFolderPath(SpecialFolder specialFolder, TChar* outPath);
};

} // namespace Lumino
