
#pragma once

#include <vector>
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
		@brief		バイト配列の先頭アドレスを取得します。
	*/
	void* GetBuffer();

public:
	// override CHLDStream
	virtual bool CanRead() { return true; }
	virtual bool CanWrite() { return true; }
	virtual size_t GetSize() { return m_buffer.size(); }
	virtual size_t Read(void* buffer, size_t bufferSize, size_t byteCount) { LN_THROW(0, NotImplementedException); return 0; }
	virtual void Write(const void* pData, size_t nByteCount);
	virtual void Flush() {}		// Write が直接メモリに書きこむので不要

private:
	std::vector<byte_t>	m_buffer;			///< 可変長の場合はこのバッファを使う
	size_t				m_seekPos;
	
	void*				m_fixedBuffer;		///< 固定長の場合はこのバッファを使う
	size_t				m_fixedBufferSize;
};

} // namespace Lumino
