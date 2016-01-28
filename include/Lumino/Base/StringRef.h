﻿/**
	@file	StringRef.h
*/
#pragma once
#include <cstddef>
#include "StringTraits.h"

LN_NAMESPACE_BEGIN

template<typename TChar>
class GenericPathName;

/**
	@brief	ある文字列に対する部分文字列の参照を保持します。
*/
template<typename TChar>
class GenericStringRef
{
public:
	GenericStringRef() : m_str(nullptr), m_pos(0), m_len(0) {}
	GenericStringRef(std::nullptr_t) : GenericStringRef(){}
	GenericStringRef(const TChar* str)
		: GenericStringRef()
	{
		m_str = str;
		m_len = StringTraits::StrLen(m_str);	// TODO: これ strlen じゃなくて templateの[N]で取ればもう少し高速化できそう
	}
	GenericStringRef(const GenericString<TChar>& str)
		: GenericStringRef()
	{
		m_str = str.c_str();
		m_len = str.GetLength();
	}
	GenericStringRef(const TChar* begin, const TChar* end)
		: GenericStringRef()
	{
		if (begin != nullptr && end != nullptr && begin <= end)
		{
			m_str = begin;
			m_len = end - begin;
		}
	}
	GenericStringRef(const GenericPathName<TChar>& path)
		: GenericStringRef()
	{
		m_str = path.GetString().c_str();
		m_len = path.GetString().GetLength();
	}

	GenericStringRef(GenericStringRef const&) = default;
	//GenericStringRef(GenericStringRef&&) = default;	// VS2013 がムーブコンストラクタの default を生成しないのでとりあえず無しで。http://stackoverflow.com/questions/24573963/move-constructor-invalid-type-for-defaulted-constructor-vs-2013
	GenericStringRef& operator =(GenericStringRef const&) = default;
	//GenericStringRef& operator =(GenericStringRef&&) = default;
	~GenericStringRef() = default;

	const TChar* GetBegin() const { return m_str + m_pos; }
	const TChar* GetEnd() const { return m_str + m_pos + m_len; }

	bool IsEmpty() const { return m_len <= 0; }
	int GetLength() const { return m_len; }

	int Compare(const TChar* str, int count = -1, CaseSensitivity cs = CaseSensitivity::CaseSensitive) const { return StringTraits::Compare(GetBegin(), m_len, str, -1, count, cs); }

	const TChar& operator[](int index) const  { return *(m_str + m_pos + index); }
	
	bool Equals(const GenericStringRef<TChar>& str) const { return Compare(str.GetBegin(), str.GetLength()) == 0; }
	

private:
	const TChar*	m_str;
	int				m_pos;
	int				m_len;
};


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
inline bool operator==(const TChar* left, const GenericStringRef<TChar>& right)
{
	return right.Equals(left);
}

typedef GenericStringRef<TCHAR>		StringRef;
typedef GenericStringRef<char>		StringRefA;
typedef GenericStringRef<wchar_t>	StringRefW;

LN_NAMESPACE_END
