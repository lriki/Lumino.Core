
#pragma once

#include "../Base/Array.h"
#include "../Base/String.h"

namespace Lumino
{
namespace Json
{
class JsonMember;

/// Json �̒l�̌^
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
	@brief	JSON�̒l��\���܂��B
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
		@brief	���̒l�� Null ��ݒ肵�܂��B
	*/
	void SetNull();
	//@}

	///@name Bool
	//@{

	/*
		@brief	���̒l�� Bool �l��ݒ肵�܂��B
	*/
	void SetBool(bool value);

	/*
		@brief	���̒l�� Bool �l���擾���܂��B
	*/
	bool GetBool() const;

	//@}

	///@name Double
	//@{

	/*
		@brief	���̒l�� Double �l��ݒ肵�܂��B
	*/
	void SetDouble(double value);

	/*
		@brief	���̒l�� Double �l���擾���܂��B
	*/
	double GetDouble() const;

	//@}

	///@name String
	//@{

	/*
		@brief	���̒l�ɕ������ݒ肵�܂��B
	*/
	void SetString(const String& str);

	/*
		@brief	���̒l�̕�������擾���܂��B
	*/
	const String& GetString() const;

	//@}

	///@name Array
	//@{

	/*
		@brief	���̒l�ɋ�̔z���ݒ肵�܂��B
	*/
	void SetArray();

	/*
		@brief	���̔z��Ɋ܂܂�Ă���l�̐����擾���܂��B
	*/
	int GetCount() const;

	//@}

	///@name Object
	//@{

	/*
		@brief	���̒l�ɋ�̃I�u�W�F�N�g��ݒ肵�܂��B
	*/
	void SetObject();

	/*
		@brief	���̃I�u�W�F�N�g�Ɋ܂܂�Ă��郁���o�̐����擾���܂��B
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
	@brief	JSON �� Object �^�̒l���������o��\���܂��B
*/
class JsonMember
{
public:
	String		Name;
	JsonValue	Value;
};

} // namespace Json
} // namespace Lumino
