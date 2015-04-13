/**
	@file	Environment.h
*/

#pragma once

namespace Lumino
{

/**
	@brief	現在の環境に関する情報にアクセスするためのクラスです。
*/
class Environment
{
public:

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

};

} // namespace Lumino
