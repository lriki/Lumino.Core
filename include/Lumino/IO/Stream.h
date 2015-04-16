
#pragma once

#include "../Base/RefObject.h"

namespace Lumino
{

/// ストリーム位置の指定の基準
enum SeekOrigin
{
	SeekOrigin_Begin	= SEEK_SET,		/// ストリームの先頭
	SeekOrigin_Current	= SEEK_CUR,		/// ストリームの現在位置
	SeekOrigin_End		= SEEK_END,		/// ストリームの末尾
};

/**
	@brief		ストリームのベースクラス
*/
class Stream
    : public RefObject
{
public:
	Stream() {}
	virtual ~Stream() {}

public:

	/**
		@brief	読み取りをサポートするかどうか
	*/
	virtual bool CanRead() = 0;

	/**
		@brief	書き込みをサポートするかどうか
	*/
	virtual bool CanWrite() = 0;

	/**
		@brief	ストリームの長さ (バイト単位) を取得します。
	*/
	virtual int64_t GetLength() const = 0;

	/**
		@brief	ストリーム内の現在位置 (ファイルポインタの位置) を取得します。
	*/
	virtual int64_t GetPosition() const = 0;
	
	/**
		@brief		バイト シーケンスを読み取り、読み取ったバイト数の分だけストリームの現在位置を進めます。
		@param		buffer		: 読み取ったデータの格納先アドレス
		@param		byteCount	: 読み取るバイト数
		@return		実際に読み取ったバイト数。EOF に到達している場合は 0 を返す。
	*/
	virtual size_t Read(void* buffer, size_t byteCount) = 0;

	/**
		@brief		現在のストリームにバイト シーケンスを書き込み、書き込んだバイト数の分だけストリームの現在位置を進めます。
		@param		data		: 書き込むデータ
		@param		byteCount	: バイト数
		@details	固定長バッファへの書き込み等で、現在のシーク位置がストリームの末尾に向かって
					count バイトよりも近い位置にあり容量を拡充できない場合は例外 (NotSupportedException) が発生します。
	*/
	virtual void Write(const void* data, size_t byteCount) = 0;


	/**
		@brief		ストリームの現在位置を指定した位置に設定します。
	*/
	virtual void Seek(int64_t offset, SeekOrigin origin) = 0;

	/**
		@brief		ストリームの内部バッファのデータを全てターゲット(ファイル等)に書き込み、内部バッファをクリアする
	*/
	virtual void Flush() = 0;

};

} // namespace Lumino
