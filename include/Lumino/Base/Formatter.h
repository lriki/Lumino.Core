
#pragma once
#include <array>
#include "Common.h"

#if defined(_MSC_VER) && _MSC_VER <= 1800 // VS2013
	#define LN_FORMAT_BRACED_INIT_WORKAROUND(x) (x)
#else
	#define LN_FORMAT_BRACED_INIT_WORKAROUND(x) {x}
#endif

LN_NAMESPACE_BEGIN

template<typename TChar, typename TValue>
struct Formatter;

namespace detail
{

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

	GenericString<TChar> DoFormat(const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam) const
	{
		return m_formatImpl(format, formatParam, m_value);
	}

private:
	template<typename T>
	static GenericString<TChar> FormatImpl(const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, const void* value)
	{
		return Formatter2<TChar, T>::Format(format, formatParam, *static_cast<const T*>(value));
		//return FormatValue(format, formatParam, *static_cast<const T*>(value));
	}

	const void* m_value;
	GenericString<TChar>(*m_formatImpl)(const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, const void* value);
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


template<typename TChar, typename TValue>
struct Formatter
{
	static GenericString<TChar> Format(const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, const TValue& value)
	{
		static_assert(0, "Invalid Format args.");
		return GenericString<TChar>();
	}
};
template<typename TChar, std::size_t N>
struct Formatter<TChar, const TChar [N]>
{
	static GenericString<TChar> Format(const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, const TChar* value)
	{
		return GenericString<TChar>(value);
	}
};
template<typename TChar>
struct Formatter<TChar, int32_t>
{
	static GenericString<TChar> Format(const GenericStringRef<TChar>& format, const GenericStringRef<TChar>& formatParam, const TChar* value)
	{
		return GenericString<TChar>(value);
	}
};


//template<typename TChar>
//struct Formatter2<TChar, String>
//{
//	static String Format(const StringRef& format, const StringRef& formatParam, const String* value)
//	{
//
//	}
//};



LN_NAMESPACE_END
