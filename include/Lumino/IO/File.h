
#pragma once

#include "../Base/RefObject.h"
#include "../Base/String.h"
#include "PathName.h"
#include "FileStream.h"

namespace Lumino
{

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

	virtual ~File();

public:

	/**
		@brief		コンストラクタで指定されたファイルパスを使用してファイルストリームを開きます。
		@param[in]	mode	: ファイルを開く方法
		@param[in]	access	: ファイルへのアクセス方法
	*/
	void Open(FileMode mode, FileAccess access);

	/**
		@brief		開いているファイルストリームを閉じます。
		@details	デストラクタからも呼び出されます。
	*/
	void Close();

	/**
		@brief		ファイルの絶対パスを取得します。
	*/
	PathName GetFilePath() const;

	/**
		@brief		拡張子を含むファイルの名前を取得します。
		@code
					File f("C:\ConsoleApplication1\Program.cs");
					f.GetFileName();	// => "Program.cs"
		@endcode
	*/
	String GetFileName() const;

	/**
		@brief		現在のファイルサイズをバイト単位で取得します。
	*/
	uint64_t GetLength() const;

public:
	// override Stream
	virtual bool CanRead();
	virtual bool CanWrite();

	/**
		@brief	ストリーム長 (バイト単位) の取得
	*/
	virtual size_t GetSize();
	
	/**
		@brief		バイト シーケンスを読み取り、読み取ったバイト数の分だけストリームの現在位置を進めます。
		@param		duffer		: 読み取ったデータの格納先アドレス
		@param		byteCount	: 読み取るバイト数
		@return		実際に読み取ったバイト数。EOF に到達している場合は 0 を返す。
	*/
	virtual size_t Read(void* duffer, size_t byteCount);

	/**
		@brief		現在のストリームにバイト シーケンスを書き込み、書き込んだバイト数の分だけストリームの現在位置を進めます。
		@param		data		: 書き込むデータ
		@param		byteCount	: バイト数
		@details	固定長バッファへの書き込み等で、現在のシーク位置がストリームの末尾に向かって
					count バイトよりも近い位置にあり容量を拡充できない場合は例外 (NotSupportedException) が発生します。
	*/
	virtual void Write(const void* data, size_t byteCount);

	/**
		@brief	ストリームの内部バッファのデータを全てターゲット(ファイル等)に書き込み、内部バッファをクリアする
	*/
	virtual void Flush();

private:
	PathName	m_filePath;
	FileAccess	m_fileAccess;
	FILE*		m_stream;

};

} // namespace Lumino
