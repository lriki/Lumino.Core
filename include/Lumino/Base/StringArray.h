
#pragma once
#include "Array.h"
#include "String.h"

LN_NAMESPACE_BEGIN

/**
	@brief	������̔z��ł��B
*/
template<typename TChar>
class GenericStringArray
	: public Array< GenericString<TChar> >
{
public:
	typedef GenericString<TChar>	StringType;


public:
	GenericStringArray() {}

	/** �������q���X�g����쐬���܂��B*/
	GenericStringArray(std::initializer_list<const TChar*> list)
	{
		Reserve(list.size());
		for (const TChar* str : list) {
			Add(String(str));
		}
	}

	virtual ~GenericStringArray() {}

public:

	/** �w�肵�������񂪂��̔z����ɑ��݂��邩�ǂ����𔻒f���܂��B*/
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
