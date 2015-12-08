/**
	@file	StringRef.h
*/
#pragma once
#include <cstddef>

LN_NAMESPACE_BEGIN

/**
	@brief	���镶����ɑ΂��镔��������̎Q�Ƃ�ێ����܂��B
*/
template<typename TChar>
class GenericStringRef
{
public:
	GenericStringRef() : m_str(nullptr), m_pos(0), m_len(0) {}
	GenericStringRef(nullptr_t) : GenericStringRef(){}
	GenericStringRef(const TChar* str)
		: GenericStringRef()
	{
		m_str = str;
		m_len = StringTraits::StrLen(m_str);
	}
	GenericStringRef(const GenericString<TChar>& str)
		: GenericStringRef()
	{
		m_str = str.c_str();
		m_len = str.GetLength();
	}

	GenericStringRef(GenericStringRef const&) = default;
	//GenericStringRef(GenericStringRef&&) = default;	// VS2013 �����[�u�R���X�g���N�^�� default �𐶐����Ȃ��̂łƂ肠���������ŁBhttp://stackoverflow.com/questions/24573963/move-constructor-invalid-type-for-defaulted-constructor-vs-2013
	GenericStringRef& operator =(GenericStringRef const&) = default;
	//GenericStringRef& operator =(GenericStringRef&&) = default;
	~GenericStringRef() = default;

	const TChar* GetBegin() const { return m_str + m_pos; }
	const TChar* GetEnd() const { return m_str + m_pos + m_len; }
	int GetLength() const { return m_len; }

private:
	const TChar*	m_str;
	int				m_pos;
	int				m_len;
};

typedef GenericStringRef<TCHAR>		StringRef;
typedef GenericStringRef<char>		StringRefA;
typedef GenericStringRef<wchar_t>	StringRefW;

LN_NAMESPACE_END
