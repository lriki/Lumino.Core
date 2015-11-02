
#pragma once
#include "Array.h"
#include "String.h"

LN_NAMESPACE_BEGIN

/**
	@brief	文字列の配列です。
*/
template<typename TChar>
class GenericStringArray
	: public Array< GenericString<TChar> >
{
public:
	typedef GenericString<TChar>	StringType;


public:
	GenericStringArray() {}

	/** 初期化子リストから作成します。*/
	GenericStringArray(std::initializer_list<const TChar*> list)
	{
		Reserve(list.size());
		for (const TChar* str : list) {
			Add(String(str));
		}
	}

	virtual ~GenericStringArray() {}

public:

	/** 指定した文字列がこの配列内に存在するかどうかを判断します。*/
	bool Contains(const TChar* str) const
	{
		return std::find(begin(), end(), str) != end();
	}
	bool Contains(const StringType& str) const		/**< @overload EndsWith */
	{
		return Contains(str.c_str());
	}

};

typedef GenericStringArray<TCHAR>	StringArray;
typedef GenericStringArray<char>	StringArrayA;
typedef GenericStringArray<wchar_t>	StringArrayW;

LN_NAMESPACE_END
