/**
	@file	Locale.h
*/
#pragma once
#include <locale>
#include "String.h"

LN_NAMESPACE_BEGIN

/**
	@brief	ロケールを表します。
*/
class Locale
{
public:

	/**
		@brief		デフォルトのロケールを使用してオブジェクトを構築します。
		@details	このロケールは GetDefault() で返されるロケールと同じになります。
	*/
	Locale();

	/**
		@brief		ロケール名を指定してオブジェクトを構築します。
	*/
	explicit Locale(const TCHAR* name);

	Locale(const Locale& locale);
	Locale& operator=(const Locale& locale);
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
	const std::locale& GetStdLocale() const;
	NativeLocale_t GetNativeLocale() const;

private:
	void Release();

private:
	std::locale		m_stdLocale;
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

LN_NAMESPACE_END
