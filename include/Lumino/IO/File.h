
#pragma once

#include "../Base/RefObject.h"
#include "../Base/String.h"
#include "PathName.h"
#include "Stream.h"

namespace Lumino
{


// フラグにまとめるべき？
enum FileOpenMode
{
	FileOpenMode_NotOpen	= 0x0000,

	FileOpenMode_ReadOnly	= 0x0001,
	FileOpenMode_WriteOnly	= 0x0002,
	FileOpenMode_ReadWrite	= FileOpenMode_ReadOnly | FileOpenMode_WriteOnly,
	
	FileOpenMode_Open		= 0x0004,
	FileOpenMode_Create		= 0x0008,
	FileOpenMode_Append		= 0x0010,

	//FileOpenMode_Unbuffered = 0x0020,
};

/**
	@brief		ファイル操作を行うクラスです。
*/
class File
	: public Stream
{
public:

	/**
		@brief		ファイル名を指定してインスタンスを初期化します。
		@details	このコンストラクタは、実際にファイルストリームを開きません。
					開いていない状態では Stream クラスの機能を使用することはできません。
					開くには、Open() を使用します。
	*/
	File(const String& filePath);
	
	/**
		@brief		ファイル名を指定してインスタンスを初期化します。
		@details	このコンストラクタは、実際にファイルストリームを開きません。
					開いていない状態では Stream クラスの機能を使用することはできません。
					開くには、Open() を使用します。
	*/
	File(const PathName& filePath);

	virtual ~File() {}

public:



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
		@brief	ストリーム長 (バイト単位) の取得
	*/
	virtual size_t GetSize() = 0;
	
	/**
		@brief		バイト シーケンスを読み取り、読み取ったバイト数の分だけストリームの現在位置を進めます。
		@param		duffer		: 読み取ったデータの格納先アドレス
		@param		bufferSize	: duffer のサイズ (バイト数単位)
		@param		byteCount	: 読み取るバイト数
		@return		実際に読み取ったバイト数。EOF に到達している場合は 0 を返す。
	*/
	virtual size_t Read(void* duffer, size_t bufferSize, size_t byteCount) = 0;

	/**
		@brief		現在のストリームにバイト シーケンスを書き込み、書き込んだバイト数の分だけストリームの現在位置を進めます。
		@param		data		: 書き込むデータ
		@param		byteCount	: バイト数
		@details	固定長バッファへの書き込み等で、現在のシーク位置がストリームの末尾に向かって
					count バイトよりも近い位置にあり容量を拡充できない場合は例外 (NotSupportedException) が発生します。
	*/
	virtual void Write(const void* data, size_t byteCount) = 0;

	/**
		@brief	ストリームの内部バッファのデータを全てターゲット(ファイル等)に書き込み、内部バッファをクリアする
	*/
	virtual void Flush() = 0;

};

} // namespace Lumino
