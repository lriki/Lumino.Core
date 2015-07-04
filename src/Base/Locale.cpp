/*
	■ CLDR(Common Locale Data Repository)
	http://cldr.unicode.org/
	ライセンスは apache-license みたい。

	・twitter-cldr-rb
	https://github.com/twitter/twitter-cldr-rb


	■ GNU C ロケール名　書式
	http://www.gnu.org/software/libc/manual/html_node/Locale-Names.html#Locale-Names

	■ MSDN ロケール名　書式
	https://msdn.microsoft.com/ja-jp/library/hzz3tw78.aspx

*/

#include "../Internal.h"
#include <locale.h>
#include <Lumino/Base/Locale.h>

namespace Lumino
{

//=============================================================================
// Locale
//=============================================================================

static NativeLocale_t CreateNativeLocale(const char* locale)
{
#ifdef LN_WIN32
	return _create_locale(LC_ALL, locale);
#else
	return newlocale(LC_ALL_MASK, locale, NULL);
#endif
}

static void FreeNativeLocale(NativeLocale_t locale)
{
#ifdef LN_WIN32
	_free_locale(locale);
#else
	freelocale(locale);
#endif
}

static NativeLocale_t GetNativeDefaultLocale()
{
#ifdef LN_WIN32
	// setlocale を使用した方法は Windows でも可能だが、取得できるのは必ずシステムロケールになってしまう。
	// Qt 等のほかのフレームワークでもユーザーロケールを優先しているし、
	// Office 等のメジャーなツールもユーザーロケールでUIを表示している。
	// これらに合わせておく。

	WCHAR strNameBuffer[LOCALE_NAME_MAX_LENGTH];
	LCID  lcid = ::GetUserDefaultLCID();
	if (LCIDToLocaleName(lcid, strNameBuffer, LOCALE_NAME_MAX_LENGTH, 0) == 0)
	{
		// エラーが発生したら変わりに "C" を使う
		return _create_locale(LC_ALL, "C");
	}
	return _wcreate_locale(LC_ALL, strNameBuffer);
#else
	//// How Programs Set the Locale
	//// http://www.gnu.org/software/libc/manual/html_node/Setting-the-Locale.html
	//StringA oldLocale = setlocale(LC_ALL, NULL);
	//char* newLocale = setlocale(LC_ALL, "");
	//setlocale(LC_ALL, oldLocale.GetCStr());

	return newlocale(LC_ALL_MASK, "", NULL);
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Locale::Locale()
	: m_nativeLocale(GetNativeDefaultLocale())
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Locale::~Locale()
{
	FreeNativeLocale(m_nativeLocale);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const Locale& Locale::GetDefault()
{
	static Locale locale;
	locale.m_nativeLocale = GetNativeDefaultLocale();
	return locale;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const Locale& Locale::GetC()
{
	static Locale locale;
	static bool init = false;
	if (!init)
	{
		locale.m_nativeLocale = CreateNativeLocale("C");
		init = true;
	}
	return locale;
}

} // namespace Lumino
