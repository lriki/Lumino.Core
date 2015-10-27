﻿/**
	@file	BinaryWriter.h
*/
#pragma once

#include "../Base/Common.h"
#include "../Base/Environment.h"
#include "Common.h"

LN_NAMESPACE_BEGIN
class Stream;

/**
	@brief	ストリームにバイナリデータを書き込むクラスです。
*/
class BinaryWriter
{
public:
	BinaryWriter(Stream* stream);
	~BinaryWriter();

public:

	/**
		@brief		符号付き 1 バイト整数を書き込み、ストリームの現在位置を 1 バイト進めます。
		@param[in]	value		: 書き込む値
	*/
	void WriteInt8(int8_t value);

	/**
		@brief		符号付き 2 バイト整数を書き込み、ストリームの現在位置を 2 バイト進めます。
		@param[in]	value		: 書き込む値
		@param[in]	byteOrder	: 書き込み先のバイトオーダー
	*/
	void WriteInt16(int16_t value, ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		符号付き 4 バイト整数を書き込み、ストリームの現在位置を 4 バイト進めます。
		@param[in]	value		: 書き込む値
		@param[in]	byteOrder	: 書き込み先のバイトオーダー
	*/
	void WriteInt32(int32_t value, ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		符号付き 8 バイト整数を書き込み、ストリームの現在位置を 8 バイト進めます。
		@param[in]	value		: 書き込む値
		@param[in]	byteOrder	: 書き込み先のバイトオーダー
	*/
	void WriteInt64(int64_t value, ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		符号無し 1 バイト整数を書き込み、ストリームの現在位置を 1 バイト進めます。
		@param[in]	value		: 書き込む値
	*/
	void WriteUInt8(uint8_t value);

	/**
		@brief		符号無し 2 バイト整数を書き込み、ストリームの現在位置を 2 バイト進めます。
		@param[in]	value		: 書き込む値
		@param[in]	byteOrder	: 書き込み先のバイトオーダー
	*/
	void WriteUInt16(uint16_t value, ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		符号無し 4 バイト整数を書き込み、ストリームの現在位置を 4 バイト進めます。
		@param[in]	value		: 書き込む値
		@param[in]	byteOrder	: 書き込み先のバイトオーダー
	*/
	void WriteUInt32(uint32_t value, ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		符号無し 8 バイト整数を書き込み、ストリームの現在位置を 8 バイト進めます。
		@param[in]	value		: 書き込む値
		@param[in]	byteOrder	: 書き込み先のバイトオーダー
	*/
	void WriteUInt64(uint64_t value, ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		4 バイト浮動小数点値を書き込み、ストリームの現在位置を 4 バイト進めます。
		@param[in]	value		: 書き込む値
	*/
	void WriteFloat(float value);

	/**
		@brief		8 バイト浮動小数点値を書き込み、ストリームの現在位置を 8 バイト進めます。
		@param[in]	value		: 書き込む値
	*/
	void WriteDouble(double value);

	/**
		@brief		指定したバイト数のデータを書き込み、ストリームの現在位置を進めます。
	*/
	void Write(const void* buffer, size_t count);

	/**
		@brief		ストリームの現在位置を指定した位置に設定します。
	*/
	void Seek(int offset, SeekOrigin origin);

	/**
		@brief		ストリームの現在位置を取得します。
	*/
	int64_t GetPosition() const;

private:
	Stream*   m_stream;
};

LN_NAMESPACE_END
