#pragma once

#include "Stream.h"

namespace Lumino
{

/// ファイルを開く方法または作成する方法
enum FileMode
{
	FileMode_Create = 0,		///< 新しいファイルを作成する。既に存在する場合は上書きされる
	FileMode_Open,				///< 既存のファイルを開く
	FileMode_Append,			///< ファイルを開き、ファイルの末尾をシークする。存在しない場合は新しいファイルを作成。

	FileMode_Max,
};

/// ファイルにアクセスする方法
enum FileAccess
{
	FileAccess_Read = 0,		///< 読み取りアクセス
	FileAccess_ReadWrite,		///< 読み取り及び書き込みアクセス
	FileAccess_Write,			///< 書き込みアクセス

	FileAccess_Max,
};

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
	virtual bool CanRead() { return (mFileAccess == FileAccess_Read) || (FileAccess_Read == FileAccess_ReadWrite); }
	virtual bool CanWrite() { return (mFileAccess == FileAccess_Write) || (FileAccess_Read == FileAccess_ReadWrite); }
	virtual size_t GetSize();
	virtual size_t Read(void* buffer, size_t bufferSize, size_t byteCount);
	virtual void Write(const void* data, size_t byteCount);
	virtual void Flush();

private:
	FILE*		mStream;
	FileAccess	mFileAccess;
};

} // namespace Lumino
