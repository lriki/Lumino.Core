﻿
#pragma once

#include <vector>
#include "../Base/Exception.h"
#include "Stream.h"

namespace Lumino
{
/**
	@brief	データの読み書き先としてメモリを使用するストリームです。
*/
class MemoryStream
	: public Stream
{
public:
	/**
		@brief		拡張可能なメモリストリームを作成します。
		@details	このコンストラクタの後、必要に応じて Create() を呼び出して再初期化することができます。
		*/
	MemoryStream();

	~MemoryStream();

public:

	/**
		@brief	キャパシティを指定して、拡張可能なメモリストリームを作成します。
		@param	size	: サイズ(バイト数)
		*/
	void Create(size_t size);

	/**
		@brief		既存のバッファを指定して、サイズ変更できないメモリストリームを作成します。
		@param		buffer	: バッファの先頭アドレス
		@param		size	: サイズ(バイト数)
		@details	buffer に指定したバッファは、このクラスのインスタンスが存在する間は開放してはいけません。
		*/
	void Create(void* buffer, size_t size);

	/**
		@brief		既存のバッファを指定して、サイズ変更できない読み取り専用のメモリストリームを作成します。
		@param		buffer	: バッファの先頭アドレス
		@param		size	: サイズ(バイト数)
		@details	buffer に指定したバッファは、このクラスのインスタンスが存在する間は開放してはいけません。
		*/
	void Create(const void* buffer, size_t size);

	/**
		@brief		データの読み書き先バイト配列の先頭アドレスを取得します。
	*/
	void* GetBuffer(size_t index = 0);

public:
	// override Stream
	virtual bool CanRead() const;
	virtual bool CanWrite() const;
	virtual int64_t GetLength() const;
	virtual int64_t GetPosition() const;
	virtual size_t Read(void* buffer, size_t byteCount);
	virtual void Write(const void* pData, size_t nByteCount);
	virtual void Seek(int64_t offset, SeekOrigin origin);
	virtual void Flush() {}		// Write が直接メモリに書きこむので不要

private:
	std::vector<byte_t>	m_buffer;			///< 可変長の場合はこのバッファを使う
	size_t				m_seekPos;

	void*				m_fixedBuffer;		///< 固定長の場合はこのバッファを使う
	const void*			m_constfixedBuffer;	///< 固定長の場合はこのバッファを使う
	size_t				m_fixedBufferSize;
};

} // namespace Lumino
