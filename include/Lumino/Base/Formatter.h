
#pragma once
#include <array>
#include "Common.h"

LN_NAMESPACE_BEGIN

#if 0
/**
	@brief		
	https://msdn.microsoft.com/ja-jp/library/txafckwd(v=vs.110).aspx#Anchor_1
*/
template<typename TChar>
class Formatter
{
private:

	class FormatArg
	{
	public:
		FormatArg() {}

		template<typename T>
		FormatArg(const T& value)
			: m_value(static_cast<const void*>(&value))
			, m_formatImpl(&formatImpl<T>)
		{
		}

		void format(std::ostream& out, const char* fmtBegin,
			const char* fmtEnd, int ntrunc) const
		{
			m_formatImpl(out, fmtBegin, fmtEnd, ntrunc, m_value);
		}

	private:
		template<typename T>
		static void formatImpl(std::ostream& out, const char* fmtBegin,
			const char* fmtEnd, int ntrunc, const void* value)
		{
			formatValue(out, fmtBegin, fmtEnd, ntrunc, *static_cast<const T*>(value));
		}

		const void* m_value;
		void(*m_formatImpl)(std::ostream& out, const char* fmtBegin,
			const char* fmtEnd, int ntrunc, const void* value);
		int(*m_toIntImpl)(const void* value);
	};

	//class FormatList
	//{
	//public:
	//	FormatList(detail::FormatArg* formatters, int N)
	//		: m_formatters(formatters), m_N(N) { }

	//	friend void vformat(std::ostream& out, const char* fmt,
	//		const FormatList& list);

	//private:
	//	const detail::FormatArg* m_formatters;
	//	int m_N;
	//};

	template<int N>
	class FormatListN// : public FormatList
	{
	public:
		template<typename... Args>
		FormatListN(const Args&... args)
			:/* FormatList(&m_formatterStore[0], N)
			, */m_formatterStore({ FormatArg(args)... })
		{
			static_assert(sizeof...(args) == N, "Invalid args count.");
		}


	private:
		std::array<FormatArg, N> m_formatterStore;
	};

	//template<> class FormatListN<0> : public FormatList
	//{
	//public: FormatListN() : FormatList(0, 0) {}
	//};


	template<typename... Args>
	FormatListN<sizeof...(Args)> MakeArgList(const Args&... args)
	{
		return FormatListN<sizeof...(args)>(args...);
	}

public:

	template<typename... TArgs>
	static void Format(const TChar* format, const TArgs&... args)
	{
		auto list = MakeArgList(args);
		printf("");
	}


};

#endif

LN_NAMESPACE_END
