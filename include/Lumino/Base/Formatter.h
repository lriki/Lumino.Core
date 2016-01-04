
#pragma once
#include <array>
#include <strstream>
#include "Common.h"

#if defined(_MSC_VER) && _MSC_VER <= 1800 // VS2013
	#define LN_FORMAT_BRACED_INIT_WORKAROUND(x) (x)
#else
	#define LN_FORMAT_BRACED_INIT_WORKAROUND(x) {x}
#endif

LN_NAMESPACE_BEGIN

template<typename TChar, typename TKind, typename TValue>
struct Formatter;




namespace detail
{


struct FormatArgType
{
	struct KindArithmetic {};
	struct KindString {};
};













// 引数1つ分。データへの参照は void* で持つ
template<typename TChar>
class FormatArg
{
public:
	FormatArg() {}

	template<typename T>
	FormatArg(const T& value)
		: m_value(static_cast<const void*>(&value))
		, m_formatImpl(&FormatImpl<T>)	// T に応じた変換関数のポインタ
	{
	}

	GenericString<TChar> DoFormat(const Locale& locale, const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam) const
	{
		return m_formatImpl(locale, format, formatParam, m_value);
	}

private:
	template<typename T>
	static GenericString<TChar> FormatImpl(const Locale& locale, const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, const void* value)
	{
		using typeKind = STLUtils::first_enabled_t<
			std::enable_if<std::is_arithmetic<T>::value, detail::FormatArgType::KindArithmetic>,
			std::enable_if<std::is_same<T, String>::value, detail::FormatArgType::KindString>,
			std::false_type>;
		return Formatter<TChar, typeKind, T>::Format(locale, format, formatParam, *static_cast<const T*>(value));
	}

	const void* m_value;
	GenericString<TChar>(*m_formatImpl)(const Locale& locale, const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, const void* value);
};

template<typename TChar>
class FormatList
{
public:
	FormatList(FormatArg<TChar>* argList, int count)
		: m_argList(argList), m_count(count)
	{}

	const FormatArg<TChar>& GetArg(int index) const { return m_argList[index]; }
	int GetCount() const { return m_count; }

private:
	//friend void vformat(std::ostream& out, const char* fmt, const FormatList& list);
	const FormatArg<TChar>* m_argList;
	int m_count;
};


template<typename TChar, int N>
class FormatListN : public FormatList<TChar>
{
public:
	template<typename... Args>
	FormatListN(const Args&... args)
		: FormatList<TChar>(&m_argsStore[0], N)
		, m_argsStore LN_FORMAT_BRACED_INIT_WORKAROUND({ FormatArg<TChar>(args)... })	// この部分は → のように展開される {FormatArg(e1), FormatArg(e2), FormatArg(e3)} http://en.cppreference.com/w/cpp/language/parameter_pack
	{
		static_assert(sizeof...(args) == N, "Invalid args count.");
	}

private:
	std::array<FormatArg<TChar>, N> m_argsStore;
};

// 引数リスト0個の場合の特殊化
template<typename TChar>
class FormatListN<TChar, 0> : public FormatList<TChar>
{
public: FormatListN() : FormatList<TChar>(0, 0) {}
};

//template<> class FormatListN<0> : public FormatList
//{
//public: FormatListN() : FormatList(0, 0) {}
//};






template<typename TChar, typename... Args>
static FormatListN<TChar, sizeof...(Args)> MakeArgList(const Args&... args)
{
	return FormatListN<TChar, sizeof...(args)>(args...);
}

} // namespace detail

/**
	@brief		
	https://msdn.microsoft.com/ja-jp/library/txafckwd(v=vs.110).aspx#Anchor_1
*/
//template<typename TChar>
//class Formatter
//{
//public:
//
//	//static GenericString<TChar> FormatValue(const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, const String& value)
//	//{
//	//	return value;
//	//}
//	//static GenericString<TChar> FormatValue(const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, const TCHAR* value)
//	//{
//	//	return GenericString<TChar>(value);
//	//}
//
//
//
//
//public:
//
//	template<typename... TArgs>
//	static void Format(const TChar* format, const TArgs&... args)
//	{
//		auto list = MakeArgList(args...);
//		printf("");
//	}
//
//
//};

//
//template<typename TChar>
//struct FormatterTraits
//{
//};
//template<>
//struct FormatterTraits<char>
//{
//	typedef std::strstream tstrstream;
//};
//template<>
//struct FormatterTraits<wchar_t>
//{
//	typedef std::wstringstream tstrstream;
//};


template<typename TChar>
class StdCharArrayBuffer : public std::basic_streambuf<TChar, std::char_traits<TChar> >
{
public:
	TChar* m_begin;

	StdCharArrayBuffer(TChar* buffer, size_t bufferLength)
	{
		m_begin = buffer;
		TChar* bufferEnd = buffer + bufferLength;
		setp(buffer, bufferEnd);
		setg(buffer, buffer, bufferEnd);
	}

	const TChar* GetCStr()
	{
		*pptr() = '\0';
		return m_begin;
	}
};














template<typename TChar, typename TKind, typename TValue>
struct Formatter
{
	static GenericString<TChar> Format(const Locale& locale, const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, const TValue& value)
	{
		// ここでエラーとなる場合、引数リストに不正な型を指定している。
		static_assert(0, "Invalid Format args.");
		return GenericString<TChar>();
	}
};
template<typename TChar, std::size_t N>
struct Formatter<TChar, std::false_type, const TChar[N]>
{
	static GenericString<TChar> Format(const Locale& locale, const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, const TChar* value)
	{
		return GenericString<TChar>(value);
	}
};

template<typename TChar>
struct Formatter<TChar, detail::FormatArgType::KindArithmetic, char>
{
	static GenericString<TChar> Format(const Locale& locale, const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, char value)
	{
		return GenericString<TChar>((TChar)value);
	}
};

template<typename TChar, typename TValue>
struct Formatter<TChar, detail::FormatArgType::KindArithmetic, TValue>
{
	static GenericString<TChar> Format(const Locale& locale, const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, TValue value)
	{
		TCHAR buf[64];
		StdCharArrayBuffer<TCHAR> b(buf, 64);
		std::basic_ostream<TCHAR, std::char_traits<TCHAR> > os(&b);
		os.imbue(locale.GetStdLocale());

		if (format.IsEmpty())
		{
		}
		else if (format.GetLength() == 1)
		{
			if (format[0] == 'x' || format[0] == 'X')
			{
				os << std::hex;
				if (format[0] == 'X') { os << std::uppercase; }
			}
			else if (format[0] == 'e' || format[0] == 'E')
			{
				os << std::scientific;
				if (format[0] == 'E') { os << std::uppercase; }

				if (!formatParam.IsEmpty())
				{
					NumberConversionResult result;
					const TChar* dummy;
					uint32_t n = StringTraits::ToInt32(formatParam.GetBegin(), formatParam.GetLength(), 10, &dummy, &result);
					LN_THROW(result == NumberConversionResult::Success, InvalidFormatException);
					os << std::setprecision(n);
				}
			}
		}
		else
		{
			LN_THROW(0, InvalidFormatException);
		}
		// http://sla0.jp/2012/04/cpp%E3%81%AEiostream%E3%83%95%E3%82%A9%E3%83%BC%E3%83%9E%E3%83%83%E3%83%88%E6%8C%87%E5%AE%9A%E6%97%A9%E8%A6%8B%E8%A1%A8/

		os << value;
		return GenericString<TChar>(b.GetCStr());
	}
};



LN_NAMESPACE_END
