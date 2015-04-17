/**
	@file	File.h
*/
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
	//explicit File(const PathName& filePath);

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
	//uint64_t GetLength() const;

public:
	// override Stream
	virtual bool CanRead();
	virtual bool CanWrite();
	virtual int64_t GetLength() const;
	virtual int64_t GetPosition() const;
	virtual size_t Read(void* buffer, size_t byteCount);
	virtual void Write(const void* data, size_t byteCount);
	virtual void Seek(int64_t offset, SeekOrigin origin);
	virtual void Flush();

private:
	PathName	m_filePath;
	FileAccess	m_fileAccess;
	FILE*		m_stream;

};

} // namespace Lumino
