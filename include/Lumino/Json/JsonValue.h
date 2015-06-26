
#pragma once

#include "../Base/Array.h"
#include "../Base/String.h"

namespace Lumino
{
namespace Json
{
class JsonMember;

/// Json の値の型
enum Type
{
	Type_Null = 0,
	Type_Bool,
	Type_Double,
	Type_String,
	Type_Array,
	Type_Object,
};

/*
	@brief	JSONの値を表します。
*/
class JsonValue
{
public:
	JsonValue();
	virtual ~JsonValue();
	JsonValue(const JsonValue& obj);
	JsonValue& operator=(const JsonValue& obj);

public:
	Type GetType()  const { return m_type; }
	bool IsNull()   const { return m_type == Type_Null; }
	bool IsBool()   const { return m_type == Type_Bool; }
	bool IsDouble() const { return m_type == Type_Double; }
	bool IsString() const { return m_type == Type_String; }
	bool IsArray()  const { return m_type == Type_Array; }
	bool IsObject() const { return m_type == Type_Object; }

	///@name Null
	//@{
	/*
		@brief	この値に Null を設定します。
	*/
	void SetNull();
	//@}

	///@name Bool
	//@{

	/*
		@brief	この値に Bool 値を設定します。
	*/
	void SetBool(bool value);

	/*
		@brief	この値の Bool 値を取得します。
	*/
	bool GetBool() const;

	//@}

	///@name Double
	//@{

	/*
		@brief	この値に Double 値を設定します。
	*/
	void SetDouble(double value);

	/*
		@brief	この値の Double 値を取得します。
	*/
	double GetDouble() const;

	//@}

	///@name String
	//@{

	/*
		@brief	この値に文字列を設定します。
	*/
	void SetString(const String& str);

	/*
		@brief	この値の文字列を取得します。
	*/
	const String& GetString() const;

	//@}

	///@name Array
	//@{

	/*
		@brief	この値に空の配列を設定します。
	*/
	void SetArray();

	/*
		@brief	この配列に含まれている値の数を取得します。
	*/
	int GetCount() const;

	//@}

	///@name Object
	//@{

	/*
		@brief	この値に空のオブジェクトを設定します。
	*/
	void SetObject();

	/*
		@brief	このオブジェクトに含まれているメンバの数を取得します。
	*/
	int GetMemberCount() const;
	
	//@}

private:
	void Copy(const JsonValue& obj);
	void Detach();

	friend class JsonDOMHandler;
	void ResizeValueList(int count) { m_valueList->Resize(count); }
	JsonValue& GetAtValue(int index) { return m_valueList->GetAt(index); }
	void ResizeMemberList(int count) { m_memberList->Resize(count); }
	JsonMember& GetAtMember(int index) { return m_memberList->GetAt(index); }

private:
	typedef Array<JsonValue>	ValueList;
	typedef Array<JsonMember>	MemberList;

	Type	m_type;
	union
	{
		uint64_t	m_uint;
		bool		m_bool;
		double		m_double;
		String*		m_string;
		ValueList*	m_valueList;
		MemberList*	m_memberList;
	};
};

/*
	@brief	JSON の Object 型の値が持つメンバを表します。
*/
class JsonMember
{
public:
	String		Name;
	JsonValue	Value;
};

} // namespace Json
} // namespace Lumino
