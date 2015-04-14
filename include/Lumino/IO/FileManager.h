/**
	@file	FileManager.h
*/
#pragma once

#include "Common.h"
#include "Stream.h"
#include "PathName.h"
#include "../Threading/Thread.h"
#include "../Threading/Mutex.h"

namespace Lumino
{

/**
	@brief	
*/
class FileManager
    : public RefObject
{
public:
	/// �������f�[�^
	struct ConfigData
	{
		FileAccessPriority	AccessMode;     ///< �t�@�C���ւ̃A�N�Z�X�D��x
	};

public:
	static FileManager& GetInstance();

public:

	/**
		@brief		�A�[�J�C�u�t�@�C����o�^���܂��B
		@details	�ȍ~���̃N���X�̋@�\��ʂ��āA�A�[�J�C�u�t�@�C�����t�H���_�ł��邩�̂悤��
					�����̃t�@�C���ɓǂݎ��A�N�Z�X���邱�Ƃ��ł��܂��B
	*/
	void RegisterArchive(const PathName& filePath, const String& password);

	/**
		@brief		�w�肳�ꂽ�t�@�C�������݂��邩�ǂ������m�F���܂��B
		@details	���������Ɏw�肵���A�N�Z�X�D��x�ɏ]���A�o�^����Ă���A�[�J�C�u������΂��̓������m�F���܂��B
	*/
	bool ExistsFile(const char* filePath);
	bool ExistsFile(const wchar_t* filePath);	///< @overload ExistsFile
	bool ExistsFile(const PathName& filePath);	///< @overload ExistsFile

	/**
		@brief		�ǂݎ���p���[�h�Ńt�@�C���X�g���[�����J���܂��B
		@details	���������Ɏw�肵���A�N�Z�X�D��x�ɏ]���A�o�^����Ă���A�[�J�C�u������΂��̓������m�F���܂��B
	*/
	Stream* CreateFileStream(const char* filePath);
	Stream* CreateFileStream(const wchar_t* filePath);	///< @overload CreateInFileStream
	Stream* CreateFileStream(const PathName& filePath);	///< @overload CreateInFileStream

	/**
		@brief		���N�G�X�g����Ă��邷�ׂĂ̔񓯊��ǂݍ���/�������ݏ����̏I����ҋ@���܂��B
	*/
	void WaitForAllASyncProcess();

	/**
		@brief		���݂̊��̃t�@�C���V�X�e�����A�p�X������̑啶���Ə���������ʂ��邩���m�F���܂��B
	*/
	CaseSensitivity GetFileSystemCaseSensitivity() const;

private:
	FileManager();
	virtual ~FileManager();

	void RefreshArchiveList();

private:
	typedef ArrayList<IArchive*>	ArchiveList;

	FileAccessPriority	m_fileAccessPriority;
	ArchiveList			m_archiveList;
	IArchive*			m_dummyArchive;
	Threading::Mutex	m_mutex;
};

} // namespace Lumino
