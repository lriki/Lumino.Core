
#pragma once

#include <vector>
#include <string>
#include "Array.h"
#include "Common.h"

namespace Lumino
{
class Encoding;
class RefBuffer;

/// String::Split() �̏o�͕��@
enum StringSplitOptions
{
	StringSplitOptions_None = 0,			///< �o�͂͋�̕�������܂�
	StringSplitOptions_RemoveEmptyEntries,	///< �o�͂͋�̕�������܂܂Ȃ�
};

/**
	@brief		�������\���N���X
	@details	std::basic_string ���x�[�X�ɋ@�\�g�����s����������N���X�ł��B
	
	@note		MFC �� CString �̂悤�� �R���X�g���N�^�ł� char �� wchar_t �ϊ��͍s��Ȃ��B
				�ʓr�����o�֐� (AssignCStr) ���Ăяo�����Ƃōs���B
				����̓e���v���[�g�^�����̃f�t�H���g�l���l�������p�����K�v�ł���A��������╡�G�ɂȂ邽�߁B
				�܂��A���̂悤�ȃe���v���[�g�̎����� Android �p�l�C�e�B�u�R���p�C���ł͏o���Ȃ������͂��B(���͂ł���悤�ɂȂ��Ă��邩���H)
	
	@note		Replace ���̕������ύX����֐��́A�ǐ�����̂��߂Ɋ�{�I�ɏ������ʂ�߂�l�ŕԂ��Ă���B
				VisualC++ �� GCC ���̃��W���[�ȃR���p�C���ł� RVO ���K�p����邽�߁A�w�ǂ̏ꍇ�R�s�[�R���X�g���N�^�͌Ă΂�Ȃ��B
				�l�ŕԂ����Ƃ̃I�[�o�[�w�b�h�́A�R���X�g���N�^�Ăяo��1��̂݁B

*/
template<typename TChar>
class BasicString : public std::basic_string<TChar>
{
public:
	static const int MaxFormatLength = 2048;

	typedef std::basic_string<TChar>	std_basic_string;
	typedef BasicString<TChar>			StringT;
    typedef std::size_t size_type;		// need GCC

public:

	// default & copys
	BasicString() : std_basic_string() {}
	BasicString(const BasicString& str) : std_basic_string(str) {}
	BasicString(const BasicString& str, size_type length) : std_basic_string(str, length) {}
	BasicString(const BasicString& str, size_type begin, size_type length) : std_basic_string(str, begin, length) {}

	// override std::basic_string
	explicit BasicString(const std_basic_string& str) : std_basic_string(str) {}
			 BasicString(size_type count, TChar ch)	: std_basic_string(count, ch) {}
	
	// TChar ���蓖��
	BasicString(const TChar* str) : std_basic_string(str) {}
	BasicString(const TChar* str, size_type length) : std_basic_string(str, length) {}
	BasicString(const TChar* str, size_type begin, size_type length) : std_basic_string(str + begin, length) {}

	// operators
	BasicString& operator=(const BasicString& right)		{ return static_cast<BasicString&>(std_basic_string::operator=(right)); }
	BasicString& operator=(const std_basic_string& right)	{ return static_cast<BasicString&>(std_basic_string::operator=(right)); }	// = std::string
	BasicString& operator=(const TChar* ptr)				{ return static_cast<BasicString&>(std_basic_string::operator=(ptr)); }
	BasicString& operator+=(const BasicString& right)		{ return static_cast<BasicString&>(std_basic_string::operator+=(right)); }
	BasicString& operator+=(const std_basic_string& right)	{ return static_cast<BasicString&>(std_basic_string::operator+=(right)); }	// += std::string
	BasicString& operator+=(const TChar* ptr)				{ return static_cast<BasicString&>(std_basic_string::operator+=(ptr)); }
	BasicString& operator+=(TChar ch)						{ return static_cast<BasicString&>(std_basic_string::operator+=(ch)); }
	operator const TChar*() const { return std_basic_string::c_str(); }

public:

	/// C����`���̕�����|�C���^��Ԃ�
	const TChar* GetCStr() const { return std_basic_string::c_str(); }
	
	/// �����񂪋�̎���true��Ԃ�
	bool IsEmpty() const { return std_basic_string::empty(); }

	/**
		@brief		����������Ɖϒ��������X�g���當����𐶐�����
		@param[in]	format		: ����������
		@param[in]	...			: �������X�g
		@attention	��������镶������ MaxFormatLength �ȓ��Ɏ��܂�Ȃ���΂Ȃ�܂���B(���ӂꂽ�ꍇ�A��O��throw���܂�)
					����́A_vsnwprintf �ɑ�������֐���Windows�ȊO�ł͎g�p�ł����A���炩���ߐ�����̕K�v�o�b�t�@�T�C�Y�𑪂邱�Ƃ��ł��Ȃ����߂ł��B<br>
					Format() �͊�{�I�ɐ��l����̕ϊ����A�Z��������ɂ̂ݎg�p���A������̘A���� += ���Z�q�����g�p���Ă��������B
					�܂��A�ϒ��������X�g�ɂ��̃N���X�̃C���X�^���X�𒼐ڎw�肷�邱�Ƃ͂ł��܂���B
					GetCStr() ���Ŏ擾����������|�C���^�^���w�肵�Ă��������B
	*/
	void Format(const TChar* format, ...);

	/**
		@brief		�l�C�e�B�u�^����������蓖�Ă�
		@param[in]	str				: �ݒ肷�镶����
		@param[in]	begin			: �R�s�[�͈͂̊J�n�C���f�b�N�X (�ȗ������ꍇ�͐擪����)
		@param[in]	length			: �R�s�[���镶���� (�ȗ������ꍇ�͏I�[ \0 �܂�)
		@param[in]	usedDefaultChar	: �ϊ��s�������f�t�H���g���� ('?') �ɕϊ��������ǂ���
		@details	���̊֐��� char �p�Awchar_t �p���ꂼ��I�[�o�[���[�h����܂��B
					�^���� TChar �ɑ΂��ĕK�v�ł���΁A
					���ꂼ�� Encoding::GetSystemMultiByteEncoding()�AEncoding::GetWideCharEncoding() �Ŏ擾�ł���G���R�[�f�B���O���g�p����
					�����R�[�h���̕ϊ����s���܂��B
					TChar �� str �̌^�������ꍇ�͕����R�[�h�̕ϊ����s���܂���B
	*/
	void AssignCStr(const char* str, size_type begin, size_type length, bool* usedDefaultChar = NULL);

	/// @copydoc AssignCStr
	void AssignCStr(const char* str, size_type length, bool* usedDefaultChar = NULL) { AssignCStr(str, 0, length, usedDefaultChar); }

	/// @copydoc AssignCStr
	void AssignCStr(const char* str, bool* usedDefaultChar = NULL) { AssignCStr(str, 0, std_basic_string::npos, usedDefaultChar); }

	/// @copydoc AssignCStr
	void AssignCStr(const wchar_t* str, size_type begin, size_type length, bool* usedDefaultChar = NULL);

	/// @copydoc AssignCStr
	void AssignCStr(const wchar_t* str, size_type length, bool* usedDefaultChar = NULL) { AssignCStr(str, 0, length, usedDefaultChar); }

	/// @copydoc AssignCStr
	void AssignCStr(const wchar_t* str, bool* usedDefaultChar = NULL) { AssignCStr(str, 0, std_basic_string::npos, usedDefaultChar); }

	/**
		@brief		�w�肵���G���R�[�f�B���O���g�p���A�������ϊ����Đݒ肷��
		@param[in]	buffer				: �ϊ��������񂪊i�[����Ă���o�b�t�@
		@param[in]	byteCount			: buffer �̃o�C�g�� (-1 �� \0 �܂�)
		@param[in]	encoding			: �ϊ��������G���R�[�f�B���O
		@param[out]	usedDefaultChar		: �ϊ��s�������f�t�H���g���� ('?') �ɕϊ��������ǂ���
	*/
	void ConvertFrom(const void* buffer, int byteCount, const Encoding* encoding, bool* usedDefaultChar = NULL);

	/**
		@brief		�w�肵���G���R�[�f�B���O���g�p���A�ϊ�����������o�b�t�@���擾����
		@param[in]	encoding			: �ϊ��敶���G���R�[�f�B���O
		@param[out]	pUsedDefaultChar	: �ϊ��s�������f�t�H���g���� ('?') �ɕϊ��������ǂ���
		@return		\0�I�[�����͕t������܂���BGetSize() �ɂ��g�p�o�C�g�����m�F�ł��܂��B
					�g�p��ARelease() �ŊJ������K�v������܂��B
	*/
	RefBuffer* ConvertTo(const Encoding* encoding, bool* usedDefaultChar = NULL) const;

	/**
		@brief		�󕶎����ݒ肷��
	*/
	void SetEmpty() { this->clear(); }

	/**
		@brief		������������擾����
		@param[in]	startIndex	: ������� 0 ����n�܂�J�n�����ʒu
		@param[in]	length		: ����������̕����� (�ȗ������ꍇ�͖����܂�)
	*/
	StringT SubString(size_type startIndex = 0, size_type length = std::string::npos) const;

	/**
		@brief		������̐擪�Ɩ����̋󔒂�S�č폜����
		@details	�󔒕����̓��P�[���Ɉˑ����܂��B���{����ł���ΑS�p�X�y�[�X���󔒈����ł��B

	*/
	StringT Trim() const;

	/**
		@brief		������̒u�����s��
		@param[in]	from
		@param[in]	to
		@return		�u�����ʂ̕�����
		@details	from �Ɉ�v���邷�ׂĂ̕������ to �ɒu�����܂��B

	*/
	StringT Replace(const TChar* from, const TChar* to) const;

	/**
		@brief		��������������A���������ŏ��̕����̃C���f�b�N�X��Ԃ�
		@param[in]	str			: ����������
		@param[in]	startIndex	: �������J�n����C���f�b�N�X (�ȗ������ꍇ�͐擪����)
		@return		������Ȃ������ꍇ�� -1
	*/
	int IndexOf(const TChar* str, int startIndex = 0) const;

	/**
		@brief		��������f���~�^�ŕ�������
		@param[in]	delim	: �f���~�^������
		@param[in]	option	: �������@
		@return		�������ʂ̕�����z��
	*/
	Array< BasicString<TChar> > Split(const TChar* delim, StringSplitOptions option = StringSplitOptions_None) const;

	/**
		@brief		��������\������o�C�g�����擾����
	*/
	int GetByteCount() const { return std_basic_string::size() * sizeof(TChar); }

	/**
		@brief		������𐮐��l�ɕϊ�����
		@details	std::istringstream �ɂ��ϊ��ł��B
					�S�Ă̕������p�[�X�o�����r���Ŏ��s�����ꍇ�͗�O�� throw ����܂��B
	*/
	int ToInt() const;

	/**
		@brief		������𐮐��l�ɕϊ�����
		@details	�p�[�X�Ɏ��s�����ꍇ�͗�O�� throw ����̂ł͂Ȃ� false ��Ԃ��܂��B
					��ʂ̃��[�v�������A��O�ɂ��p�t�H�[�}���X�ւ̉e�������O�����ꍇ�Ɏg�p���Ă��������B
	*/
	bool ToInt(int* value) const;

	/// �I�[ \0 �܂ł̕�������Ԃ� (�}���`�o�C�g�����͍l�����Ȃ��BCString::GetLength �Ɠ��l�̊֐��ł�)
	int GetLength() const { return std_basic_string::size(); } 

public:
	/// ���݂̊��Œ�`����Ă�����s��������擾����
	static const StringT& GetNewLine();

private:
	Encoding* GetThisTypeEncoding() const;
};

template<typename TChar>
inline BasicString<TChar> operator+(const BasicString<TChar>& left, const BasicString<TChar>& right)
{
	BasicString<TChar> str;
	str.reserve(left.size() + right.size());
	str += left;
	str += right;
	return str;
}
template<typename TChar>
inline BasicString<TChar> operator+(const BasicString<TChar>& left, const TChar* right)
{
	BasicString<TChar> str;
	str += left;
	str += right;
	return str;
}

typedef BasicString<TCHAR>		String;
typedef BasicString<char>		StringA;
typedef BasicString<wchar_t>	StringW;

} // namespace Lumino