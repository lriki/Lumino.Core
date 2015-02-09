
#pragma once

#include <string>
//#include "TChar.h"
#include "STLUtils.h"
#include "Array.h"

namespace Lumino
{

enum StringComparison
{
	StringComparison_Normal = 0,		///< �啶������������ʂ��Ĕ�r���s��
	StringComparison_IgnoreCase,		///< �啶������������ʂ��Ȃ��Ŕ�r���s��
};

/**
	@brief		�����񃆁[�e�B���e�B
	@details	char ����� wchar_t �^������ɑ΂���e������s�����[�e�B���e�B�֐��S�ł��B
				�����̓��e�͌^���Ⴄ�����œ���Ȃ̂� template �֐����g�p���Ă��܂��B
				�������A������ cpp �ɉB�����邽�ߖ����I�C���X�^���X�����s���Ă��܂��B
				���̂��� char �܂͂� wchar_t �^�݂̂ɂ����g���܂���B(�قƂ�ǖ��Ȃ��Ǝv���܂���)
*/
class StringUtils
{
public:

	/**
		@brief		NULL �I�[������̒��������߂�
	*/
	static size_t GetLength(const char* str) { return ::strlen(str); }

	/** @copydoc	GetLength */
	static size_t GetLength(const wchar_t* str) { return ::wcslen(str); }

	/**
		@brief		������啶��������
	*/
	static char ToUpper(char ch) { return (char)::toupper(ch); }

	/** @copydoc	ToUpper */
	static wchar_t ToUpper(wchar_t ch) { return (wchar_t)::towupper(ch); }
	
	/**
		@brief		
		@attention	���������v��@�\�͕ۏႵ�Ȃ��Blinux �ł� wchar_t �p�ɕ�������v��֐����������߁B
	*/
	static int VSPrintf(char* out, int charCount, const char* format, va_list args);
	static int VSPrintf(wchar_t* out, int charCount, const wchar_t* format, va_list args);


	/**
		@brief		�}���`�o�C�g����������C�h������ɕϊ�����
		@details	input �̕����R�[�h�� setlocale() �Ɉˑ����܂��B
					��{�I��OS��API���Ԃ��������ϊ����������Ɏg�p���Ă��������B
	*/
	static void ConvertMultiToWide(std::wstring* out, const char* input, int inputLength);

	/**
		@brief		�������󔒕����ł��邩���m�F����
		@details	�W���󔒗ޕ��� �F �����^�u�i'\t'�j�A���s�i'\n'�j�A�����^�u�i'\v'�j�A��������i'\f'�j�A���A�i'\r'�j�A�󔒁i' '�j
					���P�[���̉e�����󂯂܂��B(��F���{����ł���΁Awchar_t �̑S�p�X�y�[�X���X�y�[�X�Ƃ��Ĉ����܂��B)
	*/
	static bool IsSpace(char ch) { return isspace(ch) != 0; }

	/** @copydoc	IsSpace */
	static bool IsSpace(wchar_t ch) { return iswspace(ch) != 0; }

	static void StrNCpy(char* dest, size_t destSize, const char* src, int count) { strncpy_s(dest, destSize, src, count); }
	static void StrNCpy(wchar_t* dest, size_t destElementsSize, const wchar_t* src, int count) { wcsncpy_s(dest, destElementsSize, src, count); }	// �����P�ʁB�o�C�g�P�ʂł͂Ȃ�

	/// strncmp �� overload ����
	static int StrNCmp(const char* str1, const char* str2, size_t count) { return strncmp(str1, str2, count); }
	static int StrNCmp(const wchar_t* str1, const wchar_t* str2, size_t count) { return wcsncmp(str1, str2, count); }

	/**
		@brief		��������������A���������ŏ��̕����̃C���f�b�N�X��Ԃ�
		@param[in]	str1		: �����Ώە�����
		@param[in]	str2		: ����������
		@param[in]	startIndex	: �������J�n����C���f�b�N�X (�ȗ������ꍇ�͐擪����)
		@return		������Ȃ������ꍇ�� -1
	*/
	template<typename TChar>
	static int IndexOf(const TChar* str1, const TChar* str2, int startIndex = 0);

	/**
		@brief		������̑O��ɂ���󔒂�������������͈̔͂𒲂ׂ�
		@param[in]	
	*/
	template<typename TChar>
	static void Trim(const TChar* begin, int length, const TChar** outBegin, int* outLength);

	/**
		@brief		����������Ɖϒ��������X�g���當����𐶐�����
		@param[in]	format		: ����������
		@param[in]	...			: �������X�g
		@return		�������ꂽ������
		@attention	��������镶������ MaxFormatLength �ȓ��Ɏ��܂�Ȃ���΂Ȃ�܂���B(���ӂꂽ�ꍇ�A��O��throw���܂�)
					����́A_vsnwprintf �ɑ�������֐���Windows�ȊO�ł͎g�p�ł����A���炩���ߐ�����̕K�v�o�b�t�@�T�C�Y�𑪂邱�Ƃ��ł��Ȃ����߂ł��B<br>
					Format() �͊�{�I�ɐ��l����̕ϊ����A�Z��������ɂ̂ݎg�p���A������̘A���� += ���Z�q�����g�p���Ă��������B
					�܂��A�ϒ��������X�g�ɕ�����N���X�̃C���X�^���X�𒼐ڎw�肷�邱�Ƃ͂ł��܂���B
					GetCStr() ���Ŏ擾����������|�C���^�^���w�肵�Ă��������B
	*/
	template<typename TChar>
	static BasicString<TChar> Format(const TChar* format, ...);

	/**
		@brief		����������Ɖϒ��������X�g���當����𐶐�����
	*/
	template<typename TChar>
	static void FormatVAList(const TChar* format, va_list args, BasicString<TChar>* out);

	/**
		@brief		������̖�������v���邩�𒲂ׂ�
		@details	str2 ���󕶎��̏ꍇ�͕K�� true ���Ԃ�܂��B
		@code
		str = "abc def";
		if (EndsWith(str, -1, "def", -1)) {
			// ��v����
		}
		@endcodes
	*/
	template<typename TChar>
	static bool EndsWith(const TChar* str1, int len1, const TChar* str2, int len2, StringComparison comparisonType);
	
	/**
		@brief		��������f���~�^�ŕ�������
		@param[in]	str		: �����Ώۂ̕�����
		@param[in]	delim	: �f���~�^������
		@param[in]	option	: �������@
		@return		�������ʂ̕�����z��
	*/
	template<typename TChar>
	static Array< BasicString<TChar> > Split(const BasicString<TChar>& str, const TChar* delim, StringSplitOptions option = StringSplitOptions_None);

	/**
		@brief		[start] �` [end - 1] �͈̔͂̐擪�����s���𔻕ʂ��A��v�����當������Ԃ� ("\r" �� "\n" �Ȃ� 1�A"\r\n" �Ȃ� 2)
	*/
	template<typename TChar>
	static int CheckNewLineSequence(const TChar* start, const TChar* end);
};

} // namespace Lumino
