/**
	@file	Locale.h
*/
#pragma once
#include "String.h"

namespace Lumino
{

/**
	@brief	���P�[����\���܂��B
*/
class Locale
{
public:

	/**
		@brief		�f�t�H���g�̃��P�[�����g�p���ăI�u�W�F�N�g���\�z���܂��B
		@details	���̃��P�[���� GetDefault() �ŕԂ���郍�P�[���Ɠ����ɂȂ�܂��B
	*/
	Locale();


	Locale(const Locale& locale);
	Locale& operator=(const Locale& locale);
	~Locale();

public:

	/**
		@brief		���݂̃f�t�H���g�̃��P�[�����擾���܂��B
		@details	Windows �̏ꍇ�A���[�U�[���P�[�����g�p���܂��B
	*/
	static const Locale& GetDefault();

	/**
		@brief		"C" ���P�[�����擾���܂��B
		@details	"C" ���P�[���� ANSI �ɏ��������ŏ�����\���܂��B
	*/
	static const Locale& GetC();

public:
	NativeLocale_t GetNativeLocale() const;

private:
	void Release();

private:
	NativeLocale_t	m_nativeLocale;
#ifdef LN_OS_WIN32
	StringW			m_nativeName;
#else
	StringA			m_nativeName;
#endif
};

/**
	@brief		
*/
template<typename TChar>
class GenericLocalizer
{
public:
	GenericLocalizer(const Locale& locale);

public:
	int Format(TChar* outBuf, int outBufLength, const TChar* format, ...);

private:
	Locale	m_locale;
};

} // namespace Lumino
