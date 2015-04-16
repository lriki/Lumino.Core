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
	@brief		�t�@�C��������s���N���X�ł��B
*/
class File
	: public Stream
{
public:

	/**
		@brief		�t�@�C�������w�肵�ăC���X�^���X�����������܂��B
		@details	���̃R���X�g���N�^�́A���ۂɃt�@�C���X�g���[�����J���܂���B
					�J���Ă��Ȃ���Ԃł� Stream �N���X�̋@�\���g�p���邱�Ƃ͂ł��܂���B
					�J���ɂ́AOpen() ���g�p���܂��B
	*/
	File(const String& filePath);
	
	/**
		@brief		�t�@�C�������w�肵�ăC���X�^���X�����������܂��B
		@details	���̃R���X�g���N�^�́A���ۂɃt�@�C���X�g���[�����J���܂���B
					�J���Ă��Ȃ���Ԃł� Stream �N���X�̋@�\���g�p���邱�Ƃ͂ł��܂���B
					�J���ɂ́AOpen() ���g�p���܂��B
	*/
	//explicit File(const PathName& filePath);

	virtual ~File();

public:

	/**
		@brief		�R���X�g���N�^�Ŏw�肳�ꂽ�t�@�C���p�X���g�p���ăt�@�C���X�g���[�����J���܂��B
		@param[in]	mode	: �t�@�C�����J�����@
		@param[in]	access	: �t�@�C���ւ̃A�N�Z�X���@
	*/
	void Open(FileMode mode, FileAccess access);

	/**
		@brief		�J���Ă���t�@�C���X�g���[������܂��B
		@details	�f�X�g���N�^������Ăяo����܂��B
	*/
	void Close();

	/**
		@brief		�t�@�C���̐�΃p�X���擾���܂��B
	*/
	PathName GetFilePath() const;

	/**
		@brief		�g���q���܂ރt�@�C���̖��O���擾���܂��B
		@code
					File f("C:\ConsoleApplication1\Program.cs");
					f.GetFileName();	// => "Program.cs"
		@endcode
	*/
	String GetFileName() const;

	/**
		@brief		���݂̃t�@�C���T�C�Y���o�C�g�P�ʂŎ擾���܂��B
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
