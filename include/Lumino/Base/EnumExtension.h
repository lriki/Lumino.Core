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

/**
	@brief	拡張 enum 型を定義します。
	@see	Doc_EnumExtension_1
*/
#define LN_ENUM(enumName) \
	class enumName : public Enum \
	{ \
	public: \
		enum _##enumName

/**
	@brief	拡張 enum 型を定義します。
	@see	Doc_EnumExtension_1
*/
#define LN_ENUM_DECLARE(enumName) \
	private: \
		int	m_value; \
	public: \
		typedef _##enumName enum_type; \
		enumName() : m_value(0) {} \
		enumName(enum_type v) : m_value(v) {} \
		inline operator int() const { return m_value; } \
		inline bool operator==(enumName right) const { return m_value == right.m_value; } \
		inline bool operator==(enum_type right) const { return m_value == right; } \
		inline bool operator!=(enumName right) const { return !operator==(right); } \
		inline bool operator!=(enum_type right) const { return !operator==(right); } \
		friend inline bool operator==(enumName::enum_type left, enumName right) throw(); \
		friend inline bool operator!=(enumName::enum_type left, enumName right) throw(); \
	}; \
	inline bool operator==(enumName::enum_type left, enumName right) throw() { return left == right.m_value; } \
	inline bool operator!=(enumName::enum_type left, enumName right) throw() { return left != right.m_value; }

/**
	@brief	拡張 enum 型の型情報にアクセスする機能を定義します。
	@see	Doc_EnumExtension_1
*/
#define LN_ENUM_REFLECTION(enumName, ...) \
private: \
	struct LocalEnumParser : public EnumParser <_##enumName> { LocalEnumParser() { _##enumName values[] = { __VA_ARGS__ };  Init(values, LN_ARRAY_SIZE_OF(values), #__VA_ARGS__); } }; \
	static LocalEnumParser& GetEnumParser() { static LocalEnumParser parser; return parser; } \
public: \
	String ToString() const { return GetEnumParser().ToString(m_value); } \
	static enumName Parse(const TCHAR* str) { return GetEnumParser().Parse(str); }; \
	static bool TryParse(const TCHAR* str, enumName* outValue) { return GetEnumParser().TryParse(str, (outValue) ? &outValue->m_value : NULL); }

/**
	@brief	ビットフィールドとしてフラグの組み合わせを表す 拡張 enum 型を定義します。
	@see	Doc_EnumExtension_1
*/
#define LN_ENUM_FLAGS(enumName) \
	class enumName : public Enum \
	{ \
	public: \
		enum _##enumName

/**
	@brief	ビットフィールドとしてフラグの組み合わせを表す 拡張 enum 型を定義します。
	@see	Doc_EnumExtension_1
*/
#define LN_ENUM_FLAGS_DECLARE(enumName) \
	private: \
		int	m_value; \
	public: \
		typedef _##enumName enum_type; \
		enumName() : m_value(0) {} \
		enumName(enum_type v) : m_value(v) {} \
		inline bool TestFlag(enum_type f) const throw() { return (m_value & f) == f && (f != 0 || m_value == f); } \
		inline operator int() const { return m_value; } \
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
	@brief	拡張 enum 型の型情報にアクセスする機能を定義します。LN_ENUM_FLAGS で定義した enum に対して使用します。
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


namespace Lumino
{
	
/**
	@brief	拡張 enum 型のベースクラスです。
*/
class Enum
{
protected:

	// C++ operators
	// https://en.wikipedia.org/wiki/Operators_in_C_and_C++

	template<typename TEnum>
	struct EnumParser
	{
		// LN_ENUM_DECLARE マクロが非常に長くなるのを避けるため、部分的にクラス化した
	public:
		struct Pair { String Name; TEnum Value; };
		typedef typename ArrayList<Pair> PairList;

		static PairList& GetMemberList()
		{
			static PairList members; return members;	// ヘッダ include だけで済ますため、static 変数は関数内に閉じ込めておく
		}
		void Init(const TEnum* values, int valuesCount, const char* argNames)
		{
			PairList& members = GetMemberList();
			String names = argNames;
			ArrayList<String> tokens = names.Split(_T(","));
			for (int i = 0; i < valuesCount; ++i)
			{
				Pair p;
				p.Name = tokens[i].Trim();
				p.Value = values[i];
				members.Add(p);
			}
		}
		static String ToString(int value)
		{
			PairList& members = GetMemberList();
			for (int i = 0; i < members.GetCount(); ++i)
			{
				if (members[i].Value == value) {
					return members[i].Name;
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
				if (members[i].Name == str)
				{
					*outValue = members[i].Value;
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

		static String ToString(int value, const TCHAR* separator)
		{
			PairList& members = EnumParser<TEnum>::GetMemberList();
			String out;
			for (int i = 0; i < members.GetCount(); ++i)
			{
				TEnum f = members[i].Value;
				if ((value & f) == f && (f != 0 || value == f))
				{
					if (!out.IsEmpty()) { out += separator; }
					out += members[i].Name;
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
				case 0:	// 識別子を検索中
					if (isalnum(*pos)) {
						tokenBegin = pos;
						state = 1;		// 識別子の解析へ
					}
					else if (isspace(*pos)) {
						// 継続する
					}
					else {
						return false;	// 不正な文字が見つかった
					}
					break;

				case 1:	// 識別子を解析中
					if (isalnum(*pos)) {
						// 継続する
					}
					else {
						// 識別子確定
						int value;
						if (TryParseInternal(tokenBegin, (pos - tokenBegin), &value)) {
							(*outValue) |= value;
						}
						else {
							// メンバではない識別値が見つかった。とりあえず無視。
						}
						tokenBegin = NULL;
						state = 2;		// セパレータの検索へ
						--pos;			// この時点で | を指しているかもしれないので戻しておく
					}
					break;

				case 2:	// セパレータを検索中
					if (*pos == separator) {
						state = 0;		// 識別子の検索へ
					}
					else if (isspace(*pos)) {
						// 継続する
					}
					else {
						return false;	// 不正な文字が見つかった
					}
					break;
				}

				++pos;
			}

			// 終端が識別子で終わっていた場合はその分を読み取る
			if (tokenBegin != NULL)
			{
				int value;
				if (TryParseInternal(tokenBegin, (pos - tokenBegin), &value)) {
					(*outValue) |= value;
				}
				else {
					// メンバではない識別値が見つかった。とりあえず無視。
				}
			}

			return true;
		}

		static bool TryParseInternal(const TCHAR* str, int len, int* outValue)
		{
			PairList& members = EnumParser<TEnum>::GetMemberList();
			for (int i = 0; i < members.GetCount(); ++i)
			{
				//if (_tcsncmp(members[i].Name.GetCStr(), str, len) == 0)
				if (members[i].Name.Compare(str, len) == 0)
				{
					*outValue = members[i].Value;
					return true;
				}
			}
			return false;
		}

	};
};


} // namespace Lumino
