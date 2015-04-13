#pragma once

#include "../Base/Common.h"

namespace Lumino
{
class IArchive;

/// �t�@�C�����J�����@�܂��͍쐬������@
enum FileMode
{
	FileMode_Create = 0,		///< �V�����t�@�C�����쐬����B���ɑ��݂���ꍇ�͏㏑�������
	FileMode_Open,				///< �����̃t�@�C�����J��
	FileMode_Append,			///< �t�@�C�����J���A�t�@�C���̖������V�[�N����B���݂��Ȃ��ꍇ�͐V�����t�@�C�����쐬�B

	FileMode_Max,				///< (terminator)
};

/// �t�@�C���ɃA�N�Z�X������@
enum FileAccess
{
	FileAccess_Read = 0,		///< �ǂݎ��A�N�Z�X
	FileAccess_ReadWrite,		///< �ǂݎ��y�я������݃A�N�Z�X
	FileAccess_Write,			///< �������݃A�N�Z�X

	FileAccess_Max,				///< (terminator)
};

/// �t�@�C���̋��L���@
enum FileShare
{
	FileShare_None = 0,			///< ���̂��ׂĂ̓ǂݏ��������ۂ���
	FileShare_Read,				///< �����ǂݎ���p�Ńt�@�C�����J�����Ƃ�������
	FileShare_Write,			///< �����������ݐ�p�Ńt�@�C�����J�����Ƃ�������
	FileShare_ReadWrite,		///< �����ǂݎ��܂��͏������݂Ńt�@�C�����J�����Ƃ������� (���ׂĂ̋��L��������)

	FileShare_Max,				///< (terminator)
};

/// �t�@�C���ւ̃A�N�Z�X�D��x
enum FileAccessPriority
{
	FileAccessPriority_ArchiveFirst = 0,	///< �A�[�J�C�u�D��
	FileAccessPriority_DirectoryFirst,		///< �f�B���N�g���D��
	FileAccessPriority_ArchiveOnly,			///< �A�[�J�C�u�̂�   
};

#pragma pack(1)
struct ArchiveHeader
{
	char	ID[4];			///< 'l' 'n' 'a' ' '
	uint8_t	Version;		///< �t�@�C���o�[�W����
	byte_t	Padding[11];	///< �\���̃T�C�Y�� 16 �̔{���ɂȂ�悤�ɂ��邽�߂̃p�f�B���O
};
#pragma pack()

} // namespace Lumino
