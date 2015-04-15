/**
	@file	BinaryReader.h
*/
#pragma once

#include "../Base/Common.h"

namespace Lumino
{
class Stream;

/// エンディアンを示す値
enum ByteOrder
{
	ByteOrder_Little = 0,	///< リトルエンディアン
	ByteOrder_Big,			///< ビッグエンディアン
};

/**
	@brief	ストリームからバイナリデータを読み取るクラスです。
*/
class BinaryReader
{
public:
	BinaryReader(Stream* stream);
	~BinaryReader();

public:

	/**
		@brief		符号付き 1 バイト整数を読み取り、ストリームの現在位置を 1 バイト進めます。
		@param[in]	byteOrder	: 読み取るデータのバイトオーダー
	*/
	int8_t ReadInt8(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		符号付き 2 バイト整数を読み取り、ストリームの現在位置を 2 バイト進めます。
		@param[in]	byteOrder	: 読み取るデータのバイトオーダー
	*/
	int16_t ReadInt16(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		符号付き 4 バイト整数を読み取り、ストリームの現在位置を 4 バイト進めます。
		@param[in]	byteOrder	: 読み取るデータのバイトオーダー
	*/
	int32_t ReadInt32(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		符号付き 8 バイト整数を読み取り、ストリームの現在位置を 8 バイト進めます。
		@param[in]	byteOrder	: 読み取るデータのバイトオーダー
	*/
	int64_t ReadUInt64(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		符号無し 1 バイト整数を読み取り、ストリームの現在位置を 1 バイト進めます。
		@param[in]	byteOrder	: 読み取るデータのバイトオーダー
	*/
	uint8_t ReadIUInt8(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		符号無し 2 バイト整数を読み取り、ストリームの現在位置を 2 バイト進めます。
		@param[in]	byteOrder	: 読み取るデータのバイトオーダー
	*/
	uint16_t ReadIUInt16(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		符号無し 4 バイト整数を読み取り、ストリームの現在位置を 4 バイト進めます。
		@param[in]	byteOrder	: 読み取るデータのバイトオーダー
	*/
	uint32_t ReadIUInt32(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		符号無し 8 バイト整数を読み取り、ストリームの現在位置を 8 バイト進めます。
		@param[in]	byteOrder	: 読み取るデータのバイトオーダー
	*/
	uint64_t ReadIUInt64(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		指定したバイト数の符号付き整数を読み取り、ストリームの現在位置を進めます。
		@param[in]	byteCount	: 読み取るバイト数 (1, 2, 4, 8のいずれか)
		@param[in]	byteOrder	: 読み取るデータのバイトオーダー
	*/
	int64_t ReadInt(int byteCount, ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		指定したバイト数の符号無し整数を読み取り、ストリームの現在位置を進めます。
		@param[in]	byteCount	: 読み取るバイト数 (1, 2, 4, 8のいずれか)
		@param[in]	byteOrder	: 読み取るデータのバイトオーダー
	*/
	uint64_t ReadUInt(int byteCount, ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		指定したバイト数のデータを読み取り、ストリームの現在位置を進めます。
	*/
	size_t Read(void* buffer, size_t count);

	/**
		@brief		ストリームの現在位置を、指定したオフセットだけ進めます。
	*/
	void Seek(int offset);

	/**
		@brief		ストリームの現在位置を取得します。
	*/
	int64_t GetPosition();

	/**
		@brief		ファイル終端に到達しているかを確認します。
	*/
	bool IsEOF() const;

private:
	Stream*   m_stream;
};

} // namespace Lumino
