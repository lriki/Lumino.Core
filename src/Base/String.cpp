
#include "../Internal.h"
#include <sstream>
#include "../../include/Lumino/Base/RefBuffer.h"
#include "../../include/Lumino/Text/Encoding.h"
#include "../../include/Lumino/Base/StringUtils.h"
#include "../../include/Lumino/Base/String.h"
#include "../../include/Lumino/Platform/Environment.h"


/*
��[2015/2/21]���������R�[�h�͌Œ肵�Ȃ�

	QString �� NSString �͓��������R�[�h���Œ肵�Ă��邪�A���̃��C�u������ String �͌Œ肵�Ȃ��B
	����́A���̃��C�u�����������̑�K�͂ȃt���[�����[�N�����킯�ł͂��A
	�ق��̃��C�u������t���[�����[�N���T�|�[�g����ړI�����邽�߁B

	String �� BasicString<> �� typedef �ŁAWindows �ł� _UNICODE �ɂ�� <char> �܂��� <wchar_t> ���g�p����B
	Unix �n�́A���̂Ƃ��� <char> �������g���B(UTF-8)
	����́AUnix �ł� wchar �n�̐��퓮�삪�\���Ɋ��҂ł��Ȃ����߁B
	�Ⴆ�΁AUbuntu �ł� wsprintf �g���ƁA���{�ꂪ�S�ď����Ă��܂��B
	����ɁAwchar_t ���T�|�[�g���Ă���V�X�e��API�͖����A�K�� UTF-8 �ɕϊ����Ă���V�X�e��API�ɓn���K�v������B
	Unix�n�͂��� OS ���x���� wchar_t ���������Ă���ƍl���������悢�BMac �ł��� NSString �� UTF-16 �ɂ��Ă���B

	���S�Ɏ�������Ȃ�� printf �������Ŏ�������K�v������B
	�䂭�䂭�̓^�C�v�Z�[�t�� printf (https://github.com/c42f/tinyformat) ����낤���ƍl���Ă͂��邪�A
	���߂ł͂��̏d�v���͒Ⴂ�B

	�ŏI�I�ɂ́A�ǂ̂悤�ȃp�^�[�����l���Ă��꒷��Z������A�����I�ȗ��R����͌��肵����Ȃ��B
	�ƂȂ�ƃ��[�U�[���g�����Ƃ��ɁA�R�[�h�������ύX���Ȃ��Ă��� OS �ŃR���p�C���ł���悤�ɂȂ邱�Ƃ��Œ�����ƂȂ�B
	���̎��ɗ���̂��A���[�U�[���g���Ă��鑼�̃t���[�����[�N�⃉�C�u�����Ɠ������Ղ����ǂ����B�����ăp�t�H�[�}���X�B

	FreeType ���g���Ƃ��� UTF-32 ��������g���������ǂ����ACEGUI ���g���Ƃ��� UTF-8 ��������g���������ǂ��B
	Windows �� Unix �ԂŃt�@�C��������肷��Ƃ��̃t�H�[�}�b�g�� UTF-16 ���x�^�[���낤�B
	Ruby �ƘA�g����Ƃ��� UTF-8 �����AJava �� C# �Ƃ� UTF-16�B
	MFC(WinAPI)�Ǝg���Ȃ� char �� wchar_t ���}�N���Ő؂�ւ�����悤�ɁB
	�����āA�Ⴆ�Γ��������R�[�h���Œ肷��� IndexOf() �Ɏw�肵�����e�������C�`�C�`�����R�[�h�ϊ����Ȃ���΂Ȃ炸�A�p�t�H�[�}���X�ɉe�������Ȃ�o��B

	���������R�[�h���Œ肵�Ă��A�O�����C�u�����ƘA�g���邽�߂ɕϊ��֐������΂��������̘b�ł͂���̂����ǁA
	���̃��C�u�����̃��C���p�r (�Q�[���G���W��) �Ƃ��Ă̓p�t�H�[�}���X�����̂��߂̎�i�͗p�ӂ��Ă����ׂ��B

	�ȉ��A�����B

	�EUTF-8������R�[�h�Ƃ���ꍇ
		+ �V���� Unix �n�ł���� OS �̓����R�[�h�� UTF-8 �ƂȂ��Ă��邽�߁A����قǖ��͂Ȃ��B
		- Windows �ł� printf �� API ���Ăяo�������� THREAD_ACP �ɕϊ�����K�v������B
		- Windows �ł� �����񃊃e������ UTF-8 �ɃR���p�C������Ȃ����߁A���{�ꕶ�����܂� char* �� String::IndexOf �ł��Ȃ��B
		  ���{����n�[�h�R�[�f�B���O���邱�Ƃ͋H�����A����ł�����ꍇ�� 0 �ł͂Ȃ��B

	�EUTF-16������R�[�h�Ƃ���ꍇ
		+ ���{��������Ƃ��A������������ UTF �n�̒��ň�ԗǂ����A���R Unicode �̕�����S�Ĉ�����B
		+ Windows �ł� �� wchar_t �ƍl���� wchar �n�̕W�����C�u�������g����B
		- �������AUnix �v�ł� �� wchar_t �Ȃ̂ŕW�����C�u�����ɑ�������֐����قڂ��ׂĎ����Ŏ�������K�v������B

	�Ewchar_t ������R�[�h�Ƃ���ꍇ
		+ UTF-16 �Ɣ�ׂ�_T() �̃��e�����ŁA�����񌟍��Ƃ����Ղ��B
		- Unix �ł͎g�����ɂȂ�Ȃ��ƍl���������悢�Bwsprintf �Ƃ����{�ꂪ������BUTF-16�Ɠ������AAPI�ĂԂƂ��� UTF-8�ւ̕ϊ��K�{�B

	�E�m�[�^�b�`(MFC ���C�N)
		+ �����񃊃e�����Ō����Ƃ��Aprintf �̉ψ����Ɏw��ł���B
		- wprintf �Ƃ��g�����ɂȂ�Ȃ��͈̂ꏏ�B

	�Q�l�Fhttp://www.nubaria.com/en/blog/?p=289
*/

namespace Lumino
{

//=============================================================================
// BasicString
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::Format(const TChar* format, ...)
{
	va_list args;
	va_start(args, format);
	try {
		StringUtils::FormatVAList(format, args, this);
		va_end(args);
	}
	catch (...) {
		va_end(args);
		throw;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::AssignCStr(const char* str, size_type begin, size_type length, bool* outUsedDefaultChar)
{
	LN_THROW(str != NULL, ArgumentException);	// std::string �� assing �� NULL ���n���ꂽ�Ƃ��̓���͖���`�BVS2013 �ł͐��䂪�Ԃ��Ă��Ȃ��Ȃ���


	if (length == std_basic_string::npos) {
		length = 0xFFFFFFFF;
	}
	size_type len = std::min(strlen(str), length);

	// �T�C�Y 0 �Ȃ�󕶎���ɂ��邾��
	if (len == 0) {
		SetEmpty();
	}
	else {
		ConvertFrom(str + begin, len, Text::Encoding::GetSystemMultiByteEncoding());
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::AssignCStr(const wchar_t* str, size_type begin, size_type length, bool* outUsedDefaultChar)
{
	LN_THROW(str != NULL, ArgumentException);	// std::string �� assing �� NULL ���n���ꂽ�Ƃ��̓���͖���`�BVS2013 �ł͐��䂪�Ԃ��Ă��Ȃ��Ȃ���


	if (length == std_basic_string::npos) {
		length = 0xFFFFFFFF;
	}
	size_type len = std::min(wcslen(str), length) * sizeof(wchar_t);

	// �T�C�Y 0 �Ȃ�󕶎���ɂ��邾��
	if (len == 0) {
		SetEmpty();
	}
	else {
		ConvertFrom(str + begin, len, Text::Encoding::GetWideCharEncoding());
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
void BasicString<TChar>::ConvertFrom(const void* buffer, int byteCount, const Text::Encoding* encoding, bool* outUsedDefaultChar)
{
	LN_THROW(encoding, ArgumentException);

	Text::Encoding* thisTypeEncoding = GetThisTypeEncoding();

	// �S�������G���R�[�f�B���O�Ȃ�ϊ��̕K�v�͖���
	if (thisTypeEncoding == encoding) 
	{
		std_basic_string::assign((const TChar*)buffer, byteCount / sizeof(TChar));
	}
	else
	{
		//size_t bytesUsed;
		//size_t charsUsed;
		//bool usedDefaultChar;
		Text::EncodingConversionResult info;
		RefPtr<RefBuffer> tmpBuffer(
			Text::Encoding::Convert(buffer, byteCount, encoding, thisTypeEncoding,
			&info));
		if (outUsedDefaultChar != NULL) {
			*outUsedDefaultChar = info.UsedDefaultChar;
		}

		std_basic_string::assign((const TChar*)tmpBuffer->GetPointer(), info.BytesUsed / sizeof(TChar));
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
RefBuffer* BasicString<TChar>::ConvertTo(const Text::Encoding* encoding, bool* outUsedDefaultChar) const
{
	//size_t bytesUsed;
	//size_t charsUsed;
	//bool usedDefaultChar;
	Text::EncodingConversionResult info;

	RefBuffer* buf = Text::Encoding::Convert(
		std_basic_string::c_str(), GetByteCount(), GetThisTypeEncoding(),
		encoding,
		&info);
	if (outUsedDefaultChar != NULL) {
		*outUsedDefaultChar = info.UsedDefaultChar;
	}

	return buf;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> BasicString<TChar>::SubString(size_type startIndex, size_type length) const
{
	return StringT(*this, startIndex, length);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> BasicString<TChar>::Trim() const
{
	const TChar* begin;
	int length;
	StringUtils::Trim(std_basic_string::c_str(), std_basic_string::size(), &begin, &length);
	return BasicString<TChar>(begin, length);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
BasicString<TChar> BasicString<TChar>::Replace(const TChar* from, const TChar* to) const
{
	size_type pos = 0;
	size_t from_length = StringUtils::StrLen(from);
	size_t to_length = StringUtils::StrLen(to);

	StringT output(*this);

	while (pos = output.find(from, pos), pos != String::npos)
	{
		output.replace(pos, from_length, to);
		pos += to_length;
	}

	return output;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
int BasicString<TChar>::IndexOf(const TChar* str, int startIndex) const
{
	size_type pos = this->find(str, startIndex);
	if (pos == std_basic_string::npos) {
		return -1;
	}
	return pos;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
Array< BasicString<TChar> > BasicString<TChar>::Split(const TChar* delim, StringSplitOptions option) const
{
	return StringUtils::Split(*this, delim, option);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static bool StrToInt(const char* str, int* value)
{
	// TODO:str �̓R�s�[����A�����ł� std::string �Ƃ��ĕێ������B
	//		�Ǝ��� stream ������ăR�s�[���Ȃ��悤�ɂ���΍������̗]�n������B
	std::istringstream iss(str);
	iss >> *value;
	return iss.eof();
}
static bool StrToInt(const wchar_t* str, int* value)
{
	std::wistringstream iss(str);
	iss >> *value;
	return iss.eof();
}

template<typename TChar>
int BasicString<TChar>::ToInt() const
{
	int tmp;
	bool r = StrToInt(Trim().c_str(), &tmp);
	LN_THROW(r, InvalidFormatException);
	return tmp;
}

template<typename TChar>
bool BasicString<TChar>::ToInt(int* value) const
{
	return StrToInt(Trim().c_str(), value);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
const BasicString<TChar>& BasicString<TChar>::GetNewLine()
{
	static BasicString<TChar> nl(Environment::GetNewLine<TChar>());
	return nl;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
Text::Encoding* BasicString<TChar>::GetThisTypeEncoding() const
{
	if (sizeof(TChar) == sizeof(char))
	{
		// this �̃G���R�[�f�B���O�̓V�X�e���ˑ��ł���
		return Text::Encoding::GetSystemMultiByteEncoding();
	}
	else if (sizeof(TChar) == sizeof(wchar_t))
	{
		// this �̃G���R�[�f�B���O�� wchar_t �ł���
		return Text::Encoding::GetWideCharEncoding();
	}
	else
	{
		LN_THROW(0, NotImplementedException);
	}
}

// �e���v���[�g�̃C���X�^���X��
template class BasicString<char>;
template class BasicString<wchar_t>;

} // namespace Lumino
