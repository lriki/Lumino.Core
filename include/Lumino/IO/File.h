
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
	File(const PathName& filePath);

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
	uint64_t GetLength() const;

public:
	// override Stream
	virtual bool CanRead();
	virtual bool CanWrite();

	/**
		@brief	�X�g���[���� (�o�C�g�P��) �̎擾
	*/
	virtual size_t GetSize();
	
	/**
		@brief		�o�C�g �V�[�P���X��ǂݎ��A�ǂݎ�����o�C�g���̕������X�g���[���̌��݈ʒu��i�߂܂��B
		@param		duffer		: �ǂݎ�����f�[�^�̊i�[��A�h���X
		@param		byteCount	: �ǂݎ��o�C�g��
		@return		���ۂɓǂݎ�����o�C�g���BEOF �ɓ��B���Ă���ꍇ�� 0 ��Ԃ��B
	*/
	virtual size_t Read(void* duffer, size_t byteCount);

	/**
		@brief		���݂̃X�g���[���Ƀo�C�g �V�[�P���X���������݁A�������񂾃o�C�g���̕������X�g���[���̌��݈ʒu��i�߂܂��B
		@param		data		: �������ރf�[�^
		@param		byteCount	: �o�C�g��
		@details	�Œ蒷�o�b�t�@�ւ̏������ݓ��ŁA���݂̃V�[�N�ʒu���X�g���[���̖����Ɍ�������
					count �o�C�g�����߂��ʒu�ɂ���e�ʂ��g�[�ł��Ȃ��ꍇ�͗�O (NotSupportedException) ���������܂��B
	*/
	virtual void Write(const void* data, size_t byteCount);

	/**
		@brief	�X�g���[���̓����o�b�t�@�̃f�[�^��S�ă^�[�Q�b�g(�t�@�C����)�ɏ������݁A�����o�b�t�@���N���A����
	*/
	virtual void Flush();

private:
	PathName	m_filePath;
	FileAccess	m_fileAccess;
	FILE*		m_stream;

};

} // namespace Lumino
