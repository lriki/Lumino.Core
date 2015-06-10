/**
	@file	Locale.h
*/
#pragma once

#ifdef LN_WIN32
	typedef _locale_t NativeLocale_t;
#else
	#include <locale.h>
	#include <xlocale.h>
	#include <ctype.h>
	#include <stdlib.h>
	typedef locale_t NativeLocale_t;
#endif

namespace Lumino
{

/**
	@brief		���P�[����\���܂��B
*/
class Locale
{
public:

	/**
		@brief		�f�t�H���g�̃��P�[�����g�p���ăI�u�W�F�N�g���\�z���܂��B
		@details	���̃��P�[���� GetDefault() �ŕԂ���郍�P�[���Ɠ����ɂȂ�܂��B
	*/
	Locale();

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
	NativeLocale_t	m_nativeLocale;
};

} // namespace Lumino
