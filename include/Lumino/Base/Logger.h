/**
	@file	Logger.h
*/
#pragma once

namespace Lumino
{

/**
	@brief		���O�t�@�C���̃��[�e�B���e�B�ł��B
*/
class Logger
{
public:

	/// �ʒm���x��
	enum Level
	{
		Level_Info = 0,		///< �⑫���
		Level_Warning ,		///< �x��
		Level_Error,		///< �G���[
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
		@details	���O�@�\������������Ă��Ȃ��ꍇ�͉������܂���B
	*/
	static void WriteLine(Level level, const char* format, ...) throw();
	static void WriteLine(Level level, const wchar_t* format, ...) throw();		///< @overload WriteLine

};

} // namespace Lumino
