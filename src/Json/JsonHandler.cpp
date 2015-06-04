
#include "../Internal.h"
#include <Lumino/Json/JsonValue.h>
#include <Lumino/Json/JsonDocument.h>
#include <Lumino/Json/JsonHandler.h>

namespace Lumino
{
namespace Json
{

//=============================================================================
// JsonHandler
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JsonHandler::~JsonHandler()
{
}

//=============================================================================
// JsonDOMHandler
//=============================================================================
/*
	On�`() �Ő����ɐe�� JsonValue �֒ǉ����āc �Ȃ�Ă��Ƃ���Ă�ƁA
	���Ȃ�̉񐔂̃������Ĕz�u����������B

	JsonValue �͊�{�I�ɒl�^�Ƃ��Ďg����BArrayList.Add() �Ȃ�Ă����
	JsonValue ���ۂ��ƃR�s�[�A�܂�q Value ���S���R�s�[���邱�ƂɂȂ�B
	(JsonValue �� RefObject �ɂ��������邪�A����͂���ŕp�ɂ� new ���K�v�ɂȂ�)

	���������邽�߁ADOM �̍\�z��2�X�e�b�v�ɕ�����Ă���B
	step1:
		On�`() �œ���ꂽ�l���o�C�i���f�[�^�ɂ��ă������Ɉꎞ�ۑ�����B
		1�x�S�ĉ�͂��邱�ƂŔz��̗v�f���ȂǁA�K�v�ȃ������T�C�Y���S�Ă킩��B

	step2:
		�ꎞ�ۑ���������������c���[�\�����\�z����B
		step1 �ŋ��߂��v�f���Ȃǂ��g�p���邱�ƂŁA�������m�ۉ񐔂��ŏ����ɗ}����B
*/

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JsonDOMHandler::JsonDOMHandler(JsonDocument* document)
	: m_document(document)
	, m_valueRawData()
	, m_writer(&m_valueRawData)
{
	if (LN_VERIFY_ASSERT(document != NULL)) { return; }
	m_valueRawData.Create(2048);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JsonDOMHandler::~JsonDOMHandler()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonDOMHandler::OnNull()
{
	m_writer.WriteUInt8(Type_Null);
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonDOMHandler::OnBool(bool value)
{
	m_writer.WriteUInt8(Type_Bool);
	m_writer.WriteUInt8(value ? 1 : 0);
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonDOMHandler::OnDouble(double value)
{
	m_writer.WriteUInt8(Type_Double);
	m_writer.WriteDouble(value);
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonDOMHandler::OnString(const TCHAR* str, int len)
{
	m_writer.WriteUInt8(Type_String);
	m_writer.WriteInt32(len);
	m_writer.Write(str, sizeof(TCHAR) * len);
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonDOMHandler::OnStartArray()
{
	m_writer.WriteUInt8(Type_Array);
	m_startIndexStack.Push((size_t)m_writer.GetPosition());	// ���݈ʒu�� Array �̊J�n�_�Ƃ��Ċo���Ă���
	m_writer.WriteUInt32(0);						// �v�f�� (�_�~�[)
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonDOMHandler::OnEndArray(int elementCount)
{
	size_t index;
	m_startIndexStack.Pop(&index);

	size_t curPos = (size_t)m_writer.GetPosition();
	m_writer.Seek(index, SeekOrigin_Begin);
	m_writer.WriteUInt32(elementCount);				// �_�~�[�Ƃ��Ċm�ۂ��Ă����ꏊ�ɗv�f��������
	m_writer.Seek(curPos, SeekOrigin_Begin);
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonDOMHandler::OnStartObject()
{
	m_writer.WriteUInt8(Type_Object);
	m_startIndexStack.Push((size_t)m_writer.GetPosition());	// ���݈ʒu�� Object �̊J�n�_�Ƃ��Ċo���Ă���
	m_writer.WriteUInt32(0);						// �v�f�� (�_�~�[)
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonDOMHandler::OnKey(const TCHAR* str, int len)
{
	m_writer.WriteUInt8(0x80 | Type_String);		// �L�[�ł��邱�Ƃ��������߂ɍŏ�� bit �𗧂ĂĂ���
	m_writer.WriteInt32(len);
	m_writer.Write(str, sizeof(TCHAR) * len);
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool JsonDOMHandler::OnEndObject(int memberCount)
{
	size_t index;
	m_startIndexStack.Pop(&index);

	size_t curPos = (size_t)m_writer.GetPosition();
	m_writer.Seek(index, SeekOrigin_Begin);
	m_writer.WriteUInt32(memberCount);				// �_�~�[�Ƃ��Ċm�ۂ��Ă����ꏊ�ɗv�f��������
	m_writer.Seek(curPos, SeekOrigin_Begin);
	return true;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonDOMHandler::Build()
{
	m_valueRawData.Seek(0, SeekOrigin_Begin);
	BinaryReader reader(&m_valueRawData);
	BuildValue(&reader, m_document);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonDOMHandler::BuildValue(BinaryReader* reader, JsonValue* v)
{
	uint8_t type = reader->ReadUInt8();
	switch (type)
	{
	case Type_Null:
		v->SetNull();
		break;
	case Type_Bool:
		v->SetBool(reader->ReadUInt8() != 0);
		break;
	case Type_Double:
		v->SetDouble(reader->ReadDouble());
		break;
	case Type_String:
	{
		int len = reader->ReadInt32();
		v->SetString(String((TCHAR*)m_valueRawData.GetBuffer((size_t)m_valueRawData.GetPosition()), len));	// �����������當����𐶐�
		m_valueRawData.Seek(len * sizeof(TCHAR), SeekOrigin_Current);								// ���ڃ�������ǂ񂾂̂Ŏ����Ői�߂�
		break;
	}
	case Type_Array:
		v->SetArray();
		v->ResizeValueList(reader->ReadUInt32());	// ���� uint32 ���v�f��
		for (int i = 0; i < v->GetCount(); i++)
		{
			BuildValue(reader, &v->GetAtValue(i));
		}
		break;
	case Type_Object:
	{
		v->SetObject();
		v->ResizeMemberList(reader->ReadUInt32());	// ���� uint32 ���v�f��
		for (int i = 0; i < v->GetMemberCount(); i++)
		{
			BuildMember(reader, &v->GetAtMember(i));
		}
		break;
	}
	default:
		LN_THROW(0, InvalidOperationException);	// fail safe.
		break;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void JsonDOMHandler::BuildMember(BinaryReader* reader, JsonMember* m)
{
	// ��ʂ͕K�� Key �ł���͂�
	if (LN_VERIFY_ASSERT(reader->ReadUInt8() == (0x80 | Type_String))) { return; }

	// �L�[������
	int len = reader->ReadInt32();
	m->Name = String((TCHAR*)m_valueRawData.GetBuffer((size_t)m_valueRawData.GetPosition()), len);	// �����������當����𐶐�
	m_valueRawData.Seek(len * sizeof(TCHAR), SeekOrigin_Current);							// ���ڃ�������ǂ񂾂̂Ŏ����Ői�߂�

	// �l
	BuildValue(reader, &m->Value);
}

} // namespace Json
} // namespace Lumino
