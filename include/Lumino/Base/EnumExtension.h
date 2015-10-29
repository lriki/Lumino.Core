/**
	@file	EnumExtension.h
*/
/**
	@page	Doc_Core
	- @ref	Doc_EnumExtension_1
	@page	Doc_EnumExtension_1	

	@code
	@endcode
*/
#pragma once
#include "Array.h"
#include "String.h"
#include "StringArray.h"

#ifndef LN_DOXYGEN

/**
	@brief	�g�� enum �^���`���܂��B
	@see	Doc_EnumExtension_1
*/
#define LN_ENUM(enumName) \
	class enumName : public Enum \
	{ \
	public: \
enum _##enumName

/**
	@brief	�g�� enum �^���`���܂��B
	@see	Doc_EnumExtension_1
*/
#define LN_ENUM_DECLARE(enumName) \
	public: \
		typedef _##enumName enum_type; \
		enumName() { m_value = 0; } \
		enumName(enum_type v) { m_value = v; } \
		inline bool operator==(enumName right) const { return m_value == right.m_value; } \
		inline bool operator==(enum_type right) const { return m_value == right; } \
		inline bool operator!=(enumName right) const { return !operator==(right); } \
		inline bool operator!=(enum_type right) const { return !operator==(right); } \
		friend inline bool operator==(enumName::enum_type left, enumName right) throw(); \
		friend inline bool operator!=(enumName::enum_type left, enumName right) throw(); \
	}; \
	inline bool operator==(enumName::enum_type left, enumName right) throw() { return left == right.m_value; } \
	inline bool operator!=(enumName::enum_type left, enumName right) throw() { return left != right.m_value; } \


/**
	@brief	�g�� enum �^�̌^���ɃA�N�Z�X����@�\���`���܂��B
	@see	Doc_EnumExtension_1
*/
#define LN_ENUM_REFLECTION(enumName, ...) \
private: \
	struct LocalEnumParser : public EnumParser <_##enumName> { LocalEnumParser() { _##enumName values[] = { __VA_ARGS__ };  Init(values, LN_ARRAY_SIZE_OF(values), #__VA_ARGS__); } }; \
	static LocalEnumParser& GetEnumParser() { static LocalEnumParser parser; return parser; } \
public: \
	static int GetMemberCount() { return GetEnumParser().GetMemberList().GetCount(); } \
	String ToString() const { return GetEnumParser().ToString(m_value); } \
	static enumName Parse(const TCHAR* str) { return GetEnumParser().Parse(str); }; \
	static bool TryParse(const TCHAR* str, enumName* outValue) { return GetEnumParser().TryParse(str, (outValue) ? &outValue->m_value : NULL); }

/**
	@brief	�r�b�g�t�B�[���h�Ƃ��ăt���O�̑g�ݍ��킹��\�� �g�� enum �^���`���܂��B
	@see	Doc_EnumExtension_1
*/
#define LN_ENUM_FLAGS(enumName) \
	class enumName : public Enum \
	{ \
	public: \
enum _##enumName

/**
	@brief	�r�b�g�t�B�[���h�Ƃ��ăt���O�̑g�ݍ��킹��\�� �g�� enum �^���`���܂��B
	@see	Doc_EnumExtension_1
*/
#define LN_ENUM_FLAGS_DECLARE(enumName) \
	public: \
		typedef _##enumName enum_type; \
		enumName() { m_value = 0; } \
		enumName(enum_type v) { m_value = v; } \
		inline bool TestFlag(enum_type f) const throw() { return (m_value & f) == f && (f != 0 || m_value == f); } \
		inline bool operator==(enumName right) const { return m_value == right.m_value; } \
		inline bool operator==(_##enumName right) const { return m_value == right; } \
		inline bool operator!=(enumName right) const { return !operator==(right); } \
		inline bool operator!=(_##enumName right) const { return !operator==(right); } \
		inline enumName& operator &= (const enumName& v) { m_value &= v.m_value; return *this; } \
		inline enumName& operator &= (enum_type v) { m_value &= v; return *this; } \
		inline enumName& operator |= (const enumName& v) { m_value |= v.m_value; return *this; } \
		inline enumName& operator |= (enum_type v) { m_value |= v; return *this; } \
		inline enumName operator~() const throw() { enumName value; value.m_value = ~m_value; return value; } \
		friend inline bool operator==(enumName::enum_type left, enumName right) throw(); \
		friend inline bool operator!=(enumName::enum_type left, enumName right) throw(); \
		friend inline enumName operator&(const enumName& left, const enumName& right) throw(); \
		friend inline enumName operator&(const enumName& left, enum_type right) throw(); \
		friend inline enumName operator&(enum_type left, const enumName& right) throw(); \
		friend inline enumName operator&(enum_type left, enum_type right) throw(); \
		friend inline enumName operator|(const enumName& left, const enumName& right) throw(); \
		friend inline enumName operator|(const enumName& left, enum_type right) throw(); \
		friend inline enumName operator|(enum_type left, const enumName& right) throw(); \
		friend inline enumName operator|(enum_type left, enum_type right) throw(); \
		friend inline enumName operator~(enumName::enum_type v) throw(); \
	}; \
	inline bool operator==(enumName::enum_type left, enumName right) throw() { return left == right.m_value; } \
	inline bool operator!=(enumName::enum_type left, enumName right) throw() { return left != right.m_value; } \
	inline enumName operator&(const enumName& left, const enumName& right) throw()		{ enumName value; value.m_value = left.m_value & right.m_value; return value; } \
	inline enumName operator&(const enumName& left, enumName::enum_type right) throw()	{ enumName value; value.m_value = left.m_value & right; return value; } \
	inline enumName operator&(enumName::enum_type left, const enumName& right) throw()	{ enumName value; value.m_value = right.m_value & left;  return value; } \
	inline enumName operator&(enumName::enum_type left, enumName::enum_type right) throw() { return enumName(left) & right; } \
	inline enumName operator|(const enumName& left, const enumName& right) throw()		{ enumName value; value.m_value = left.m_value | right.m_value; return value; } \
	inline enumName operator|(const enumName& left, enumName::enum_type right) throw()	{ enumName value; value.m_value = left.m_value | right; return value; } \
	inline enumName operator|(enumName::enum_type left, const enumName& right) throw()	{ enumName value; value.m_value = right.m_value | left;  return value; } \
	inline enumName operator|(enumName::enum_type left, enumName::enum_type right) throw() { return enumName(left) | right; } \
	inline enumName operator~(enumName::enum_type v) throw() { enumName value; value.m_value = (~((int)v)); return value; }

/**
	@brief	�g�� enum �^�̌^���ɃA�N�Z�X����@�\���`���܂��BLN_ENUM_FLAGS �Œ�`���� enum �ɑ΂��Ďg�p���܂��B
	@see	Doc_EnumExtension_1
*/
#define LN_ENUM_FLAGS_REFLECTION(enumName, ...) \
private: \
	struct LocalEnumParser : public EnumFlagsParser <_##enumName> { LocalEnumParser() { _##enumName values[] = { __VA_ARGS__ };  Init(values, LN_ARRAY_SIZE_OF(values), #__VA_ARGS__); } }; \
	static LocalEnumParser& GetEnumParser() { static LocalEnumParser parser; return parser; } \
public: \
	String ToString(const TCHAR* separator = _T("|")) const { return GetEnumParser().ToString(m_value, separator); } \
	static enumName Parse(const TCHAR* str, TCHAR separator = '|') { return GetEnumParser().Parse(str, separator); }; \
	static bool TryParse(const TCHAR* str, enumName* outValue, TCHAR separator = '|') { return GetEnumParser().TryParse(str, (outValue) ? &outValue->m_value : NULL, separator); }


LN_NAMESPACE_BEGIN
	
/**
	@brief	�g�� enum �^�̃x�[�X�N���X�ł��B
*/
class Enum
{
protected:
	int	m_value;

public:
	inline operator int() const { return m_value; }

	// C++ operators
	// https://en.wikipedia.org/wiki/Operators_in_C_and_C++

	template<typename TEnum>
	struct EnumParser
	{
	// LN_ENUM_DECLARE �}�N�������ɒ����Ȃ�̂�����邽�߁A�����I�ɃN���X������

		struct Pair { String name; TEnum value; };
		typedef typename ::ln::Array<Pair> PairList;
		typedef typename ::ln::Array<Pair>& PairListReference;

		static PairListReference GetMemberList()
		{
			static PairList members; return members;	// �w�b�_ include �����ōς܂����߁Astatic �ϐ��͊֐����ɕ����߂Ă���
		}
		void Init(const TEnum* values, int valuesCount, const char* argNames)
		{
			PairList& members = GetMemberList();
			String names = argNames;
			Array<String> tokens = names.Split(_T(","));
			for (int i = 0; i < valuesCount; ++i)
			{
				Pair p;
				p.name = tokens[i].Trim();
				p.value = values[i];
				members.Add(p);
			}
		}
		static String ToString(int value)
		{
			PairList& members = GetMemberList();
			for (int i = 0; i < members.GetCount(); ++i)
			{
				if (members[i].value == value) {
					return members[i].name;
				}
			}
			LN_ASSERT(0);
			return String();
		}
		static TEnum Parse(const TCHAR* str)
		{
			int value;
			if (TryParse(str, &value)) {
				return (TEnum)value;
			}
			LN_THROW(0, ArgumentException);
			return TEnum();
		}
		static bool TryParse(const TCHAR* str, int* outValue)
		{
			PairList& members = GetMemberList();
			for (int i = 0; i < members.GetCount(); ++i)
			{
				if (members[i].name == str)
				{
					*outValue = members[i].value;
					return true;
				}
			}
			return false;
		}
	};

	template<typename TEnum>
	struct EnumFlagsParser : public EnumParser<TEnum>
	{
	public:
		typedef typename EnumParser<TEnum>::Pair Pair;
		typedef typename EnumParser<TEnum>::PairListReference PairListReference;

		static String ToString(int value, const TCHAR* separator)
		{
			PairListReference members = EnumParser<TEnum>::GetMemberList();
			// ��Ɋ��S��v��T�� (White=Red|Green|Blue �̂悤�ȃp�^�[���p)
			for (int i = 0; i < members.GetCount(); ++i)
			{
				if (members[i].value == value) {
					return members[i].name;
				}
			}
			// ���S��v��������Ε����̃t���O����������
			String out;
			for (int i = 0; i < members.GetCount(); ++i)
			{
				TEnum f = members[i].value;
				if ((value & f) == f && (f != 0 || value == f))
				{
					if (!out.IsEmpty()) { out += separator; }
					out += members[i].name;
				}
			}
			LN_THROW(!out.IsEmpty(), ArgumentException);
			return out;
		}
		static TEnum Parse(const TCHAR* str, TCHAR separator)
		{
			int value;
			if (TryParse(str, &value, separator)) {
				return (TEnum)value;
			}
			LN_THROW(0, ArgumentException);
			return TEnum();
		}
		static bool TryParse(const TCHAR* str, int* outValue, TCHAR separator)
		{
			int state = 0;
			const TCHAR* pos = str;
			const TCHAR* tokenBegin = NULL;
			*outValue = 0;

			while (*pos)
			{
				switch (state)
				{
				case 0:	// ���ʎq��������
					if (isalnum(*pos)) {
						tokenBegin = pos;
						state = 1;		// ���ʎq�̉�͂�
					}
					else if (isspace(*pos)) {
						// �p������
					}
					else {
						return false;	// �s���ȕ�������������
					}
					break;

				case 1:	// ���ʎq����͒�
					if (isalnum(*pos)) {
						// �p������
					}
					else {
						// ���ʎq�m��
						int value;
						if (TryParseInternal(tokenBegin, (pos - tokenBegin), &value)) {
							(*outValue) |= value;
						}
						else {
							// �����o�ł͂Ȃ����ʒl�����������B�Ƃ肠���������B
						}
						tokenBegin = NULL;
						state = 2;		// �Z�p���[�^�̌�����
						--pos;			// ���̎��_�� | ���w���Ă��邩������Ȃ��̂Ŗ߂��Ă���
					}
					break;

				case 2:	// �Z�p���[�^��������
					if (*pos == separator) {
						state = 0;		// ���ʎq�̌�����
					}
					else if (isspace(*pos)) {
						// �p������
					}
					else {
						return false;	// �s���ȕ�������������
					}
					break;
				}

				++pos;
			}

			// �I�[�����ʎq�ŏI����Ă����ꍇ�͂��̕���ǂݎ��
			if (tokenBegin != NULL)
			{
				int value;
				if (TryParseInternal(tokenBegin, (pos - tokenBegin), &value)) {
					(*outValue) |= value;
				}
				else {
					// �����o�ł͂Ȃ����ʒl�����������B�Ƃ肠���������B
				}
			}

			return true;
		}

		static bool TryParseInternal(const TCHAR* str, int len, int* outValue)
		{
			PairListReference members = EnumParser<TEnum>::GetMemberList();
			for (int i = 0; i < members.GetCount(); ++i)
			{
				if (members[i].name.Compare(str, len) == 0)
				{
					*outValue = members[i].value;
					return true;
				}
			}
			return false;
		}

	};
};
	
#else	// LN_DOXYGEN
#define LN_ENUM(enumName)		enum class enumName
#define LN_ENUM_DECLARE(enumName)
#define LN_ENUM_REFLECTION(enumName, ...)
#define LN_ENUM_FLAGS(enumName)	enum class enumName
#define LN_ENUM_FLAGS_DECLARE(enumName)
#define LN_ENUM_FLAGS_REFLECTION(enumName, ...)
#endif	// LN_DOXYGEN

LN_NAMESPACE_END
