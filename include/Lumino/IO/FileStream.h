#pragma once

#include "Stream.h"

namespace Lumino
{

/// �t�@�C�����J�����@�܂��͍쐬������@
enum FileMode
{
	FileMode_Create = 0,		///< �V�����t�@�C�����쐬����B���ɑ��݂���ꍇ�͏㏑�������
	FileMode_Open,				///< �����̃t�@�C�����J��
	FileMode_Append,			///< �t�@�C�����J���A�t�@�C���̖������V�[�N����B���݂��Ȃ��ꍇ�͐V�����t�@�C�����쐬�B

	FileMode_Max,
};

/// �t�@�C���ɃA�N�Z�X������@
enum FileAccess
{
	FileAccess_Read = 0,		///< �ǂݎ��A�N�Z�X
	FileAccess_ReadWrite,		///< �ǂݎ��y�я������݃A�N�Z�X
	FileAccess_Write,			///< �������݃A�N�Z�X

	FileAccess_Max,
};

/**
	@brief	�t�@�C���p�X�g���[��
*/
class FileStream
    : public Stream
{
public:
	FileStream();

	/**
		@brief	�t�@�C�����o�C�i�����[�h�ŊJ��
		@param	filePath	: �t�@�C����
		@param	fileMode	: �t�@�C�����J�����@�܂��͍쐬������@
		@param	fileAccess	: �t�@�C���ɃA�N�Z�X������@
	*/
	FileStream(const TCHAR* filePath, FileMode fileMode, FileAccess fileAccess);

	virtual ~FileStream();

public:

	/**
		@brief	�t�@�C�����o�C�i�����[�h�ŊJ��
		@param	filePath	: �t�@�C����
		@param	fileMode	: �t�@�C�����J�����@�܂��͍쐬������@
		@param	fileAccess	: �t�@�C���ɃA�N�Z�X������@
	*/
	void Open(const TCHAR* filePath, FileMode fileMode, FileAccess fileAccess);

	/**
		@brief	�t�@�C�������
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
