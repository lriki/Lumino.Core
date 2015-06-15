#pragma once

#include "Common.h"
#include "Stream.h"
#include "PathName.h"

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
		@param	openMode	: ファイルを開く方法 (FileOpenMode のフラグの組み合わせ)
	*/
	FileStream(const TCHAR* filePath, FileOpenMode openMode);

	virtual ~FileStream();

public:

	/**
		@brief	ファイルをバイナリモードで開く
		@param	filePath	: ファイル名
		@param	openMode	: ファイルを開く方法 (FileOpenMode のフラグの組み合わせ)
	*/
	void Open(const TCHAR* filePath, FileOpenMode openMode);

	/**
		@brief	ファイルを閉じる
	*/
	void Close();

public:
	// override
	virtual bool CanRead() const;
	virtual bool CanWrite() const;
	virtual int64_t GetLength() const;
	virtual int64_t GetPosition() const;
	virtual size_t Read(void* buffer, size_t byteCount);
	virtual void Write(const void* data, size_t byteCount);
	virtual void Seek(int64_t offset, SeekOrigin origin);
	virtual void Flush();

private:
	void CheckOpen() const;
	void Open() const;

private:
	mutable FILE*	m_stream;
	PathName		m_filePath;
	FileOpenMode	m_openModeFlags;
};

} // namespace Lumino
