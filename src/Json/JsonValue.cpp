
#pragma once

#include "../Internal.h"
#include <Lumino/Json/JsonValue.h>

namespace Lumino
{
namespace Json
{

//=============================================================================
// JsonReader
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JsonValue::JsonValue()
	: m_type(Type_Null)
	, m_uint(0)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JsonValue::~JsonValue()
{
	Detach();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JsonValue::JsonValue(const JsonValue& obj)
{
	Copy(obj);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JsonValue& JsonValue::operator=(const JsonValue& obj)
{
	Copy(obj);
	return (*this);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonValue::SetNull()
{
	Detach();
	m_type = Type_Null;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonValue::SetBool(bool value)
{
	Detach();
	m_type = Type_Bool;
	m_bool = value;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonValue::GetBool() const
{
	if (LN_VERIFY_ASSERT(m_type == Type_Bool)) { return false; }
	return m_bool;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonValue::SetDouble(double value)
{
	Detach();
	m_type = Type_Double;
	m_double = value;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
double JsonValue::GetDouble() const
{
	if (LN_VERIFY_ASSERT(m_type == Type_Double)) { return 0.0; }
	return m_double;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonValue::SetString(const String& str)
{
	Detach();
	m_type = Type_String;
	m_string = LN_NEW String(str);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const String& JsonValue::GetString() const
{
	if (LN_VERIFY_ASSERT(m_type == Type_String && m_string != NULL)) { return String::GetEmpty(); }
	return *m_string;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonValue::SetArray()
{
	Detach();
	m_type = Type_Array;
	m_valueList = LN_NEW ValueList();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int JsonValue::GetCount() const
{
	if (LN_VERIFY_ASSERT(m_type == Type_Array && m_valueList != NULL)) { return 0; }
	return m_valueList->GetCount();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonValue::SetObject()
{
	Detach();
	m_type = Type_Object;
	m_memberList = LN_NEW MemberList();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int JsonValue::GetMemberCount() const
{
	if (LN_VERIFY_ASSERT(m_type == Type_Object && m_memberList != NULL)) { return 0; }
	return m_memberList->GetCount();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonValue::Copy(const JsonValue& obj)
{
	if (obj.m_type == Type_Null) {
		SetNull();
	}
	else if (obj.m_type == Type_Bool) {
		SetBool(obj.GetBool());
	}
	else if (obj.m_type == Type_Double) {
		SetDouble(obj.GetDouble());
	}
	else if (obj.m_type == Type_String) {
		SetString(obj.GetString());
	}
	else if (obj.m_type == Type_Array)
	{
		Detach();
		m_type = Type_Array;
		m_valueList = LN_NEW ValueList(*obj.m_valueList);
	}
	else if (obj.m_type == Type_Object)
	{
		Detach();
		m_type = Type_Object;
		m_memberList = LN_NEW MemberList(*obj.m_memberList);
	}
	// ↑もし同じ型であれば new しないようにすればもう少しパフォーマンス上げられる
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonValue::Detach()
{
	if (m_type == Type_String) {
		LN_SAFE_DELETE(m_string);
	}
	else if (m_type == Type_Array) {
		LN_SAFE_DELETE(m_valueList);
	}
	else if (m_type == Type_Object) {
		LN_SAFE_DELETE(m_memberList);
	}
	m_type = Type_Null;
}


} // namespace Json
} // namespace Lumino
