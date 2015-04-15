/**
	@file	FileManager.h
*/
#pragma once

#include "Common.h"
#include "NonCopyable.h"

namespace Lumino
{

/**
	@file	DLL �܂��� ���L���C�u���������[�h���A�A�N�Z�X���邽�߂̃N���X�ł��B
*/
class DllLoader
	: public NonCopyable
{
public:

	/**
		@brief		�R���X�g���N�^
		@details	�ʓr Load() �֐����Ăяo���āA���C�u���������[�h����K�v������܂��B
	*/
	DllLoader();

	/**
		@brief		�w�肵�����C�u���������[�h���A�C���X�^���X�����������܂��B
		@param[in]	filePath	: ���C�u�����̖��O�܂��̓p�X
	*/
	DllLoader(const TCHAR* filePath);

	/**
		@brief		���[�h�ς݂̃��C�u������������܂��B
	*/
	~DllLoader();

public:

	/**
		@brief		�w�肵�����C�u���������݂��邩���m�F���܂��B
		@return		�A�N�Z�X�ł���� true�A�����łȂ���� false
	*/
	static bool Exists(const TCHAR* filePath);

	/**
		@brief		�w�肵�����C�u���������[�h���܂��B
		@param[in]	filePath	: ���C�u�����̖��O�܂��̓p�X
	*/
	void Load(const TCHAR* filePath);

	/**
		@brief		���[�h�ς݂̃��C�u������������܂��B
		@details	���[�h����Ă��Ȃ���Ή������܂���B�f�X�g���N�^�ł����s����܂��B
	*/
	void Unload();

	/**
		@brief		���C�u�����Ɋ܂܂��֐��̃A�h���X���擾���܂��B
	*/
	void* GetProcAddress(const char* procName);

private:
	void*	m_module;
};

} // namespace Lumino
