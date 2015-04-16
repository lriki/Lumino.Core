#pragma once

#include "Common.h"
#include "Stream.h"

namespace Lumino
{

/**
	@brief	ファイル用ストリーム
*/
class FileStream
    : public Stream
{
public:
	FileStream();

	/**
		@brief	ファイルをバイナリモードで開く
		@param	filePath	: ファイル名
		@param	fileMode	: ファイルを開く方法または作成する方法
		@param	fileAccess	: ファイルにアクセスする方法
	*/
	FileStream(const TCHAR* filePath, FileMode fileMode, FileAccess fileAccess);

	virtual ~FileStream();

public:

	/**
		@brief	ファイルをバイナリモードで開く
		@param	filePath	: ファイル名
		@param	fileMode	: ファイルを開く方法または作成する方法
		@param	fileAccess	: ファイルにアクセスする方法
	*/
	void Open(const TCHAR* filePath, FileMode fileMode, FileAccess fileAccess);

	/**
		@brief	ファイルを閉じる
	*/
	void Close();

public:
	// override
	virtual bool CanRead() { return (mFileAccess == FileAccess_Read) || (mFileAccess == FileAccess_ReadWrite); }
	virtual bool CanWrite() { return (mFileAccess == FileAccess_Write) || (mFileAccess == FileAccess_ReadWrite); }
	virtual int64_t GetLength() const;
	virtual int64_t GetPosition() const;
	virtual size_t Read(void* buffer, size_t byteCount);
	virtual void Write(const void* data, size_t byteCount);
	virtual void Seek(int64_t offset, SeekOrigin origin);
	virtual void Flush();

private:
	FILE*		mStream;
	FileAccess	mFileAccess;
};

} // namespace Lumino
