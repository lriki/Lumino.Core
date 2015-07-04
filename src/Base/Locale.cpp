/*
	�� CLDR(Common Locale Data Repository)
	http://cldr.unicode.org/
	���C�Z���X�� apache-license �݂����B

	�Etwitter-cldr-rb
	https://github.com/twitter/twitter-cldr-rb


	�� GNU C ���P�[�����@����
	http://www.gnu.org/software/libc/manual/html_node/Locale-Names.html#Locale-Names

	�� MSDN ���P�[�����@����
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
	// setlocale ���g�p�������@�� Windows �ł��\�����A�擾�ł���͕̂K���V�X�e�����P�[���ɂȂ��Ă��܂��B
	// Qt ���̂ق��̃t���[�����[�N�ł����[�U�[���P�[����D�悵�Ă��邵�A
	// Office ���̃��W���[�ȃc�[�������[�U�[���P�[����UI��\�����Ă���B
	// �����ɍ��킹�Ă����B

	WCHAR strNameBuffer[LOCALE_NAME_MAX_LENGTH];
	LCID  lcid = ::GetUserDefaultLCID();
	if (LCIDToLocaleName(lcid, strNameBuffer, LOCALE_NAME_MAX_LENGTH, 0) == 0)
	{
		// �G���[������������ς��� "C" ���g��
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
