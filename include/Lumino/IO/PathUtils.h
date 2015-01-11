#pragma once

namespace Lumino
{

/**
	@brief	�l�C�e�B�u������ɑ΂���p�X�����񑀍���܂Ƃ߂��N���X
*/
class PathUtils
{
public:
#ifdef _WIN32
	static const uint8_t DirectorySeparatorChar		= '\\';
	static const uint8_t AltDirectorySeparatorChar	= '/';
	static const uint8_t VolumeSeparatorChar		= ':';
#else
	static const uint8_t DirectorySeparatorChar		= '/';
	static const uint8_t AltDirectorySeparatorChar	= NULL;
	static const uint8_t VolumeSeparatorChar		= NULL;
#endif

public:

	/// path �����[�g�p�X�ł��邩�𔻒肷�� ("C:/", "C:", "/" ��)
	template<typename TChar>
	static bool IsRootPath(const TChar* path);

	/// path ����΃p�X�ł��邩�𔻒肷�� ("C:/AAA"�A"/AAA" ��������)
	template<typename TChar>
	static bool IsAbsolutePath(const TChar* path);

	/**
		@brief		�p�X�����񂩂�f�B���N�g�����������o��
		@param[in]	path	: �t�@�C���܂��̓f�B���N�g���̃p�X
		@details	�o�͗�
					- "C:\MyDir\MySubDir\myfile.ext" �� "C:\MyDir\MySubDir"
					- "C:\MyDir\MySubDir" �� "C:\MyDir"
					- "C:\MyDir\" �� "C:\MyDir"
					- "C:\MyDir" �� "C:\"
					- "C:\" �� ""
					- "/MyDir/" �� "/MyDir"
					- "/MyDir" �� "/"
					- "/" �� ""
	*/
	template<typename TChar>
	static BasicString<TChar> GetDirectoryPath(const TChar* path);

	/**
		@brief		�p�X������̒�����g���q���܂ރt�@�C�����̕�����Ԃ�
		@param[in]	path	: �p�X������
		@details	path �� NULL �̏ꍇ�͋󕶎����Ԃ��܂��B
	*/
	template<typename TChar>
	static BasicString<TChar> GetFileName(const TChar* path);

	/**
		@brief		�p�X��P��������
		@param[in]	srcPath	: ���t�@�C���p�X
		@param[in]	outPath	: �P���������p�X�̊i�[�� (LN_MAX_PATH + 1 �̗̈悪���邱��)
	*/
	template<typename TChar>
	static void CanonicalizePath(const TChar* srcPath, TChar* outPath);

	/**
		@brief		2�̃p�X��������r����
		@param[in]	path1	: �p�X������
		@param[in]	path2	: �p�X������
		@return		���בւ��������������� (0 �ň�v)
		@details	DirectorySeparatorChar �� AltDirectorySeparatorChar �͓����Ƃ݂Ȃ��܂��B
					�܂��A�啶������������ʂ��܂���B
	*/
	template<typename TChar>
	static int Compare(const TChar* path1, const TChar* path2);


	/**
		@brief		2�̃p�X�����񂪓����������`�F�b�N����
		@param[in]	path1	: �p�X������
		@param[in]	path2	: �p�X������
		@details	DirectorySeparatorChar �� AltDirectorySeparatorChar �͓����Ƃ݂Ȃ��܂��B
					�܂��A�啶������������ʂ��܂���B
	*/
	template<typename TChar>
	static bool Equals(const TChar* path1, const TChar* path2);
};

} // namespace Lumino
