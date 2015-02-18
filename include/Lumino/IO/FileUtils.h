#pragma once

#include <stdio.h>
#include "../Base/RefObject.h"
#include "../Base/String.h"

namespace Lumino
{
namespace Text { class Encoding; }
class RefBuffer;

/// �t�@�C���ƃf�B���N�g���̑���
enum FileAttribute
{
	FileAttribute_Normal		= 0x0000,	///< ���ɑ����������Ȃ��ʏ�̃t�@�C��
	FileAttribute_Directory		= 0x0001,	///< �f�B���N�g��
	FileAttribute_ReadOnly		= 0x0002,	///< �ǂݎ���p
	FileAttribute_Hidden		= 0x0004,	///< �B���t�@�C��
};

/**
	@brief	�t�@�C�����[�e�B���e�B
*/
class FileUtils
{
public:
	/// fopen �� template ����
	//template<typename TChar>
	//static FILE* fopen(const TChar* filePath, const TChar* pMode);
	
	/**
		@brief		�w�肳�ꂽ�t�@�C�������݂��邩�m�F���܂��B
		@details	���̊֐��͎w�肳�ꂽ�t�@�C���ɃA�N�Z�X�ł���ꍇ�� true ��Ԃ��܂��B
					�Ⴆ�� Windows �ł́A���ۂɃt�@�C�������݂��Ă��Ă����̃t�@�C����
					�ʂ̃p�X���[�h���b�N���ꂽ���[�U�[�t�H���_���ɑ��݂��Ă���ꍇ�� false ��Ԃ��܂��B
					(���[�U�[A �� ���[�U�[B �� "�}�C�h�L�������g" �t�H���_�̃t�@�C���ɃA�N�Z�X�ł��Ȃ�)
					�܂��A�p�X���󕶎��� NULL �̏ꍇ�� false ��Ԃ��܂��B
	*/
	static bool Exists(const char* filePath);
	static bool Exists(const wchar_t* filePath);

	/**
		@brief		�t�@�C���̑������擾����
		@param[in]	filePath		: �t�@�C����
		@return		�t�@�C���̑��� (FileAttribute �̃r�b�g�̑g�ݍ��킹)
	*/
	static uint32_t GetAttribute(const char* filePath);
	static uint32_t GetAttribute(const wchar_t* filePath);

	/**
		@brief		�t�@�C���̑�����ݒ肷��
		@param[in]	filePath		: �t�@�C����
		@param[in]	attr			: �t�@�C���̑��� (FileAttribute �̃r�b�g�̑g�ݍ��킹)
		@details	
	*/
	static void SetAttribute(const char* filePath, uint32_t attr);
	static void SetAttribute(const wchar_t* filePath, uint32_t attr);

	/**
		@brief		�t�@�C�����R�s�[����
		@param[in]	sourceFileName	: �R�s�[���t�@�C����
		@param[in]	destFileName	: �R�s�[��t�@�C����
		@param[in]	overwrite		: �R�s�[����㏑������ꍇ�� true
		@details	�ǂݎ���p�t�@�C���ɏ㏑�����邱�Ƃ͂ł��܂���B
	*/
	static void Copy(const char* sourceFileName, const char* destFileName, bool overwrite);
	static void Copy(const wchar_t* sourceFileName, const wchar_t* destFileName, bool overwrite);

	/**
		@brief		�t�@�C�����폜����
		@param[in]	filePath		: �폜����t�@�C����
		@details	�폜����t�@�C�������݂��Ȃ��ꍇ�A��O�̓X���[����܂���B
	*/
	static void Delete(const char* filePath);
	static void Delete(const wchar_t* filePath);

	/// �t�@�C���T�C�Y���擾����
	static size_t GetFileSize(const TCHAR* filePath);

	/// �t�@�C���T�C�Y���擾����
	static size_t GetFileSize( FILE* stream );

	/// �t�@�C���̓��e�����ׂēǂݍ��� (�o�C�i���`��)
	static RefBuffer* ReadAllBytes(const char* filePath);
	static RefBuffer* ReadAllBytes(const wchar_t* filePath);

	/// �t�@�C���̓��e�����ׂēǂݍ��݁A������Ƃ��ĕԂ�
	static String ReadAllText(const TCHAR* filePath, const Text::Encoding* encoding = NULL);

	/// �z��̓��e���o�C�i���t�@�C���Ƃ��ď����o��
	static void WriteAllBytes(const TCHAR* filePath, const void* buffer, size_t size);

	/// ��������e�L�X�g�t�@�C���Ƃ��ď����o��
	static void WriteAllText(const TCHAR* filePath, const String& str, const Text::Encoding* encoding = NULL);

	/// ���݂̈ʒu�ƃf�[�^(�t�@�C��)�T�C�Y�A�I�t�Z�b�g�A�(SEEK_xxxx)���󂯎���āA�V�����V�[�N�ʒu��Ԃ�
	size_t CalcSeekPoint(size_t curPoint, size_t maxSize, size_t offset, int origin);
};

} // namespace Lumino
