/**
	@file	Locale.h
*/
#pragma once

#ifdef LN_WIN32
#else
	#include <locale.h>
	#include <xlocale.h>
	#include <ctype.h>
	#include <stdlib.h>
#endif

namespace Lumino
{

/**
	@brief		ロケールを表します。
*/
class Locale
{
public:
	Locale();
	~Locale();

private:
#ifdef LN_WIN32
	typedef _locale_t InternalLocale_t;
#else
	typedef locale_t InternalLocale_t;
#endif
};

} // namespace Lumino
