
#pragma once

namespace Lumino
{
namespace Json
{

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
template<typename TChar>
class GenericValue
{
public:
	virtual ~GenericValue();

public:
	Type GetType()  const { return m_type; }
	bool IsNull()   const { return m_type == Type_Null; }
	bool IsBool()   const { return m_type == Type_Bool; }
	bool IsDouble() const { return m_type == Type_Double; }
	bool IsString() const { return m_type == Type_String; }
	bool IsArray()  const { return m_type == Type_Array; }
	bool IsObject() const { return m_type == Type_Object; }

private:
	Type	m_type;
};

} // namespace Json
} // namespace Lumino
