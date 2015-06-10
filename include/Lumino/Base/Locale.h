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
	@brief		ロケールを表します。
*/
class Locale
{
public:

	/**
		@brief		デフォルトのロケールを使用してオブジェクトを構築します。
		@details	このロケールは GetDefault() で返されるロケールと同じになります。
	*/
	Locale();

	~Locale();

public:

	/**
		@brief		現在のデフォルトのロケールを取得します。
		@details	Windows の場合、ユーザーロケールを使用します。
	*/
	static const Locale& GetDefault();

	/**
		@brief		"C" ロケールを取得します。
		@details	"C" ロケールは ANSI に準拠した最小環境を表します。
	*/
	static const Locale& GetC();

public:
	NativeLocale_t GetNativeLocale() const;

private:
	NativeLocale_t	m_nativeLocale;
};

} // namespace Lumino
