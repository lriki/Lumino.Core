﻿
#pragma once
#include <regex>
#include "String.h"

LN_NAMESPACE_BEGIN

template<typename TChar>
class GenericMatchResult;

/**
	@brief		
	@details	http://www.cplusplus.com/reference/regex/ECMAScript/
*/
template<typename TChar>
class GenericRegex
{

public:
	//typedef GenericStringRef<TChar>				StringRefT;
    using StringRefT = GenericStringRef<TChar>;

	GenericRegex(const StringRefT& pattern)
    : m_regex(pattern.GetBegin(), pattern.GetLength(), std::basic_regex<TChar>::ECMAScript)
	{
	}
	
	bool Match(const StringRefT& input, GenericMatchResult<TChar>* outResult = nullptr) const
	{
		if (outResult != nullptr) {
			return std::regex_match(input.GetBegin(), input.GetEnd(), outResult->m_matchResults, m_regex, std::regex_constants::match_default);
		}
		else {
			return std::regex_match(input.GetBegin(), input.GetEnd(), m_regex, std::regex_constants::match_default);
		}
	}
	
	bool Search(const StringRefT& input, GenericMatchResult<TChar>* outResult = nullptr) const
	{
		if (outResult != nullptr) {
			return std::regex_search(input.GetBegin(), input.GetEnd(), outResult->m_matchResults, m_regex, std::regex_constants::match_default);
		}
		else {
			return std::regex_search(input.GetBegin(), input.GetEnd(), m_regex, std::regex_constants::match_default);
		}
	}

	/** 
		@brief	対象の文字列が、正規表現パターンで表現できているかを調べます。（完全一致）
	*/
	static bool Match(const StringRefT& input, const StringRefT& pattern, GenericMatchResult<TChar>* outResult = nullptr)
	{
		GenericRegex re(pattern);
		return re.Search(input, outResult);
	}

	/**
		@brief	文字列の中から、正規表現パターンに該当する文字列があるかを調べます。（検索, 部分一致）
	*/
	static bool Search(const StringRefT& input, const StringRefT& pattern, GenericMatchResult<TChar>* outResult = nullptr)
	{
		GenericRegex re(pattern);
		return re.Search(input, outResult);
	}

private:
	//typedef std::basic_regex<TChar>	std_regex;
	std::basic_regex<TChar>	m_regex;
};


/**
	@brief
*/
template<typename TChar>
class GenericMatchResult
{
public:
	typedef GenericStringRef<TChar>	StringRefT;

	/** マッチ範囲の先頭インデックスを返します。*/
	int GetIndex() const { return static_cast<int>(m_matchResults.position()); }

	/** マッチ範囲の文字数を返します。*/
	int GetLength() const { return static_cast<int>(m_matchResults.length()); }

	StringRefT GetValue() const
	{
		return GetGroup(0);
	}
	
	int GetGroupCount() const { return (int)m_matchResults.size(); }
	
	// index=0 はマッチした全体を返す
	StringRefT GetGroup(int index) const
	{
		LN_THROW(0 <= index && index < (int)m_matchResults.size(), OutOfRangeException);
		return StringRef(m_matchResults[index].first, m_matchResults[index].second);
	}
	
	StringRefT operator[](int index) const
	{
		return GetGroup(index);
	}

private:
	template<typename T>
	friend class GenericRegex;

	typedef std::match_results<const TChar*>	std_match_results;

	std_match_results	m_matchResults;
};

#ifdef LN_DOXYGEN
/** @see GenericRegex */
class Regex {};
/** @see GenericRegex */
class RegexA {};
/** @see GenericRegex */
class RegexW {};
/** @see GenericMatchResult */
class MatchResult {};
/** @see GenericMatchResult */
class MatchResultA {};
/** @see GenericMatchResult */
class MatchResultW {};
#else
typedef GenericRegex<TCHAR>			Regex;
typedef GenericRegex<char>			RegexA;
typedef GenericRegex<wchar_t>		RegexW;
typedef GenericMatchResult<TCHAR>	MatchResult;
typedef GenericMatchResult<char>	MatchResultA;
typedef GenericMatchResult<wchar_t>	MatchResultW;
#endif

LN_NAMESPACE_END
