#pragma once

#include "Stream.h"
#include "PathName.h"

namespace Lumino
{
	
/// �t�@�C���ւ̃A�N�Z�X�D��x
enum FileAccessPriority
{
	FileAccessPriority_ArchiveFirst = 0,	///< �A�[�J�C�u�D��
	FileAccessPriority_DirectoryFirst,		///< �f�B���N�g���D��
	FileAccessPriority_ArchiveOnly,			///< �A�[�J�C�u�̂�   
};

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

	/*
		@brief		�w�肳�ꂽ�t�@�C�������݂��邩�ǂ������m�F���܂��B
		@details	���������Ɏw�肵���A�N�Z�X�D��x�ɏ]���A�o�^����Ă���A�[�J�C�u������΂��̓������m�F���܂��B
	*/
	bool ExistsFile(const char* filePath);
	bool ExistsFile(const wchar_t* filePath);	///< @overload ExistsFile
	bool ExistsFile(const PathName& filePath);	///< @overload ExistsFile

	/*
		@brief		�ǂݎ���p���[�h�Ńt�@�C���X�g���[�����J���܂��B
		@details	���������Ɏw�肵���A�N�Z�X�D��x�ɏ]���A�o�^����Ă���A�[�J�C�u������΂��̓������m�F���܂��B
	*/
	Stream* CreateFileStream(const char* filePath);
	Stream* CreateFileStream(const wchar_t* filePath);	///< @overload CreateInFileStream
	Stream* CreateFileStream(const PathName* filePath);	///< @overload CreateInFileStream

	/*
		@brief		���N�G�X�g����Ă��邷�ׂĂ̔񓯊��ǂݍ���/�������ݏ����̏I����ҋ@���܂��B
	*/
	void WaitForAllASyncProcess();

	/*
		@brief		���݂̊��̃t�@�C���V�X�e�����A�p�X������̑啶���Ə���������ʂ��邩���m�F���܂��B
	*/
	CaseSensitivity GetFileSystemCaseSensitivity() const;

private:
	FileManager();
	virtual ~FileManager();
};

} // namespace Lumino
