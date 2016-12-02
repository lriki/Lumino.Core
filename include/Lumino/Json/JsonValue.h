
#pragma once
#include "../Base/EnumExtension.h"
#include "../Base/List.h"
#include "../Base/String.h"

LN_NAMESPACE_BEGIN
namespace tr {
class JsonMember;

/** Json �̒l�̌^�������܂��B*/
LN_ENUM(JsonType)
{
	Null = 0,
	Bool,
	Double,
	String,
	Array,
	Object,
};
LN_ENUM_DECLARE(JsonType);

/**
	@brief	JSON�̒l��\���܂��B
*/
class JsonValue
{
public:
	JsonValue();
	explicit JsonValue(bool value);
	JsonValue(double value);
	explicit JsonValue(const TCHAR* value);
	explicit JsonValue(const String& value);
	explicit JsonValue(const JsonValue& value);
	explicit JsonValue(JsonType::enum_type type);
	virtual ~JsonValue();
	JsonValue& operator=(const JsonValue& obj);

public:
	JsonType GetType()  const { return m_type; }
	bool IsNull()   const { return m_type == JsonType::Null; }
	bool IsBool()   const { return m_type == JsonType::Bool; }
	bool IsDouble() const { return m_type == JsonType::Double; }
	bool IsString() const { return m_type == JsonType::String; }
	bool IsArray()  const { return m_type == JsonType::Array; }
	bool IsObject() const { return m_type == JsonType::Object; }

	/*-----------------------------------------------------------------------*/
	/** @name Null */
	/** @{ */

	/** ���̒l�� Null ��ݒ肵�܂��B*/
	void SetNull();

	/** @} */
	/*-----------------------------------------------------------------------*/
	/** @name Bool */
	/** @{ */

	/** ���̒l�� Bool �l��ݒ肵�܂��B*/
	void SetBool(bool value);

	/** ���̒l�� Bool �l���擾���܂��B*/
	bool GetBool() const;

	/** @} */
	/*-----------------------------------------------------------------------*/
	/** @name Double */
	/** @{ */

	/** ���̒l�� Double �l��ݒ肵�܂��B*/
	void SetDouble(double value);

	/** ���̒l�� Double �l���擾���܂��B*/
	double GetDouble() const;

	/** @} */
	/*-----------------------------------------------------------------------*/
	/** @name String */
	/** @{ */

	/** ���̒l�ɕ������ݒ肵�܂��B*/
	void SetString(const String& str);

	/** ���̒l�̕�������擾���܂��B*/
	const String& GetString() const;

	/** @} */
	/*-----------------------------------------------------------------------*/
	/** @name Array */
	/** @{ */

	/** ���̒l�ɋ�̔z���ݒ肵�܂��B*/
	void SetArray();

	/** ���̔z��Ɋ܂܂�Ă���l�̐����擾���܂��B*/
	int GetItemCount() const;

	/** ���̔z��̖����ɗv�f��ǉ����܂��B*/
	void AddItem(const JsonValue& value);

	/** �w�肵���C���f�b�N�X�̗v�f���擾���܂��B*/
	JsonValue& operator[](int index);
	const JsonValue& operator[](int index) const;		/**< @copydoc operator[] */

	/** @} */
	/*-----------------------------------------------------------------------*/
	/** @name Object */
	/** @{ */

	/** ���̒l�ɋ�̃I�u�W�F�N�g��ݒ肵�܂��B*/
	void SetObject();

	/** ���̃I�u�W�F�N�g�Ɋ܂܂�Ă��郁���o�̐����擾���܂��B*/
	int GetMemberCount() const;

	/** ���̃I�u�W�F�N�g�ɐV���������o��ǉ����܂��B*/
	void AddMember(const String& name, const JsonValue& value);

	/** @} */

private:
	void Copy(const JsonValue& obj);
	void Detach();

	friend class JsonDOMHandler;
	void ResizeValueList(int count) { m_valueList->Resize(count); }
	JsonValue& GetAtValue(int index) { return m_valueList->GetAt(index); }
	void ResizeMemberList(int count) { m_memberList->Resize(count); }
	JsonMember& GetAtMember(int index) { return m_memberList->GetAt(index); }

private:
	typedef List<JsonValue>	ValueList;
	typedef List<JsonMember>	MemberList;

	JsonType	m_type;
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

/**
	@brief	JSON �� Object �^�̒l���������o��\���܂��B
*/
class JsonMember
{
public:
	String		Name;
	JsonValue	Value;
};

} // namespace tr
LN_NAMESPACE_END
