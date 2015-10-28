/**
	@file	Logger.h
*/
#pragma once

LN_NAMESPACE_BEGIN

/**
	@brief		���O�t�@�C���̃��[�e�B���e�B�ł��B
*/
class Logger
{
public:

	/** �ʒm���x�� */
	enum class Level
	{
		Info = 0,		/**< ��� */
		Warning ,		/**< �x�� */
		Error,			/**< �G���[ */
	};

public:

	/**
		@brief		���O�@�\�����������A���O�t�@�C����V�K�쐬���܂��B
		@param[in]	filePath	: ���O�t�@�C���̃p�X
		@return		true=���� / false=���s
	*/
	static bool Initialize(const TCHAR* filePath) throw();

	/**
		@brief		�ʒm���x�����w�肵�ď����w�胁�b�Z�[�W���������݂܂��B
		@param[in]	level	: �ʒm���x�� (�w�肵�Ȃ��ꍇ�� Level_Info)
		@details	���O�@�\������������Ă��Ȃ��ꍇ�͉������܂���B
	*/
	static void WriteLine(Level level, const char* format, ...) throw();
	static void WriteLine(Level level, const wchar_t* format, ...) throw();		///< @overload WriteLine
	static void WriteLine(const char* format, ...) throw();						///< @overload WriteLine
	static void WriteLine(const wchar_t* format, ...) throw();					///< @overload WriteLine

};

LN_NAMESPACE_END
