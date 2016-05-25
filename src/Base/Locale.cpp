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


	�� �eOS�̕W�����C�u������ locale ����
		���P�[�����l�����������񐶐����A�X���b�h�Z�[�t�Ŏ����������̂����E�E�E�B

		����:
			setlocale �́u�v���O�����v�̃��P�[����ύX����A�Ƃ���B
			�X���b�h�P�ʂƌ��ߕt���邱�Ƃ͏o���Ȃ��̂ŁA��{�I�Ɏg��Ȃ����j�ŁB

		Windows:
			_printf_l �̂悤�ɁA_l ���t���Ă���֐��̓��P�[���������Ŏw��ł���B

		Max:
			(���ۂɓ������Ă͂��Ȃ���) Windows �Ɠ������Aprintf_l �̂悤�Ȋ֐����p�ӂ���Ă���悤���B
			�������AWindows �ƈ���Đ擪�� _ ���t���Ȃ��B

		Linux:
			�������ނɂ���č����傫���B
			FreeBSD �� Max �Ɠ��l�炵���B
			Ubuntu �� _l ���t���֐��͖����B�ς��ɁAuselocale ���g���ƃX���b�h�̃��P�[����ύX�ł���B
			(__locale_raii �ŃO�O��Ƃ��낢��o�Ă���)
		
*/

#include "../Internal.h"
#include <locale.h>
#include <Lumino/Base/Locale.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// Locale
//==============================================================================


#ifdef LN_OS_WIN32
//---------------------------------------------
class ScopedLocaleRAII
{
public:
	ScopedLocaleRAII(NativeLocale_t loc) {}
};

//---------------------------------------------
static NativeLocale_t CreateNativeLocale(const wchar_t* locale)
{
	return _wcreate_locale(LC_ALL, locale);
}

//---------------------------------------------
static void GetNativeDefaultLocale(NativeLocale_t* outLocale, StringW* outName)
{
	// setlocale ���g�p�������@�� Windows �ł��\�����A�擾�ł���͕̂K���V�X�e�����P�[���ɂȂ��Ă��܂��B
	// Qt ���̂ق��̃t���[�����[�N�ł����[�U�[���P�[����D�悵�Ă��邵�A
	// Office ���̃��W���[�ȃc�[�������[�U�[���P�[����UI��\�����Ă���B
	// �����ɍ��킹�Ă����B
	WCHAR strNameBuffer[LOCALE_NAME_MAX_LENGTH];
	LCID  lcid = ::GetUserDefaultLCID();
	if (LCIDToLocaleName(lcid, strNameBuffer, LOCALE_NAME_MAX_LENGTH, 0) == 0)
	{
		// �G���[������������ς��� "C" ���g��
		*outLocale = _wcreate_locale(LC_ALL, L"C");
		*outName = L"C";
		return;
	}
	*outLocale = _wcreate_locale(LC_ALL, strNameBuffer);
	*outName = strNameBuffer;
}

#else
//---------------------------------------------
class ScopedLocaleRAII
{
	locale_t m_old;
public:
	ScopedLocaleRAII(NativeLocale_t loc)
	{
		m_old = uselocale(loc);
	}
	~ScopedLocaleRAII()
	{
		uselocale(m_old);
	}
};
//---------------------------------------------
static NativeLocale_t CreateNativeLocale(const char* locale)
{
	return newlocale(LC_ALL_MASK, locale, NULL);
}
//---------------------------------------------
static void GetNativeDefaultLocale(NativeLocale_t* outLocale, StringA* outName)
{
	//// How Programs Set the Locale
	//// http://www.gnu.org/software/libc/manual/html_node/Setting-the-Locale.html
	//StringA oldLocale = setlocale(LC_ALL, NULL);
	//char* newLocale = setlocale(LC_ALL, "");
	//setlocale(LC_ALL, oldLocale.c_str());

	*outLocale = newlocale(LC_ALL_MASK, "", NULL);
	*outName = "";
}
#endif

static void FreeNativeLocale(NativeLocale_t locale)
{
#ifdef LN_OS_WIN32
	_free_locale(locale);
#else
	freelocale(locale);
#endif
}

//------------------------------------------------------------------------------
Locale::Locale()
	: m_nativeLocale(0)
	, m_nativeName()
{
	GetNativeDefaultLocale(&m_nativeLocale, &m_nativeName);
	StringA name = m_nativeName.ToStringA();
	m_stdLocale = std::locale(name.c_str());
}

//------------------------------------------------------------------------------
Locale::Locale(const TCHAR* name)
	: m_nativeLocale(0)
	, m_nativeName(StringW::FromNativeCharString(name))
{
	m_nativeLocale = CreateNativeLocale(m_nativeName.c_str());
	StringA t = m_nativeName.ToStringA();
	m_stdLocale = std::locale(t.c_str());
}

//------------------------------------------------------------------------------
Locale::Locale(const Locale& locale)
	: m_nativeLocale(0)
	, m_nativeName(locale.m_nativeName)
{
	m_nativeLocale = CreateNativeLocale(m_nativeName.c_str());
	m_stdLocale = locale.m_stdLocale;
}

//------------------------------------------------------------------------------
Locale& Locale::operator=(const Locale& locale)
{
	m_nativeName = locale.m_nativeName;
	m_nativeLocale = CreateNativeLocale(m_nativeName.c_str());
	m_stdLocale = locale.m_stdLocale;
	return (*this);
}

//------------------------------------------------------------------------------
Locale::~Locale()
{
	Release();
}

//------------------------------------------------------------------------------
void Locale::Release()
{
	FreeNativeLocale(m_nativeLocale);
}

//------------------------------------------------------------------------------
const std::locale& Locale::GetStdLocale() const
{
	return m_stdLocale;
}

//------------------------------------------------------------------------------
NativeLocale_t Locale::GetNativeLocale() const
{
	return m_nativeLocale;
}

//------------------------------------------------------------------------------
const Locale& Locale::GetDefault()
{
	static Locale locale;
	return locale;
}

//------------------------------------------------------------------------------
const Locale& Locale::GetC()
{
	static Locale locale;
	static bool init = false;
	if (!init)
	{
		locale.m_stdLocale = std::locale("C");
#ifdef LN_OS_WIN32
		locale.m_nativeLocale = CreateNativeLocale(L"C");
		locale.m_nativeName = L"C";
#else
		locale.m_nativeLocale = CreateNativeLocale("C");
		locale.m_nativeName = "C";
#endif
		init = true;
	}
	return locale;
}

//==============================================================================
// GenericLocalizer
//==============================================================================
//------------------------------------------------------------------------------
template<typename TChar>
GenericLocalizer<TChar>::GenericLocalizer(const Locale& locale)
	: m_locale(locale)
{
}

//------------------------------------------------------------------------------
template<typename TChar>
int GenericLocalizer<TChar>::Format(TChar* outBuf, int outBufLength, const TChar* format, ...)
{

}

LN_NAMESPACE_END
