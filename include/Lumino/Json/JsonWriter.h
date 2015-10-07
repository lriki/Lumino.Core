
#pragma once
#include "../Base/Stack.h"
#include "../IO/TextWriter.h"

namespace Lumino
{

/**
	@brief	
*/
class JsonWriter
{
public:
	JsonWriter(TextWriter* textWriter);
	virtual ~JsonWriter();

public:
	void StartObject();
	void EndObject();
	void StartArray();
	void EndArray();
	void WriteKey(const TCHAR* str, int length = -1);
	void WriteNull();
	void WriteBool(bool value);
	void WriteDouble(double value);
	void WriteString(const TCHAR* str, int length = -1);
	bool IsComplete() const;

protected:
	enum PrefixType
	{
		PrefixType_Array = 0,
		PrefixType_Object,
		PrefixType_Key,
	};
	virtual void OnPrefix(PrefixType type, int valueCount);
	virtual void OnStartObject();
	virtual void OnEndObject();
	virtual void OnStartArray();
	virtual void OnEndArray();
	virtual void OnKey(const TCHAR* str, int length);
	virtual void OnNull();
	virtual void OnBool(bool value);
	virtual void OnDouble(double value);
	virtual void OnString(const TCHAR* str, int length);

private:
	void WritePrefix();

private:
	struct Level
	{
		Level(bool inArray) : ValueCount(0), InArray(inArray), justSawKey(false) {}
		int		ValueCount;	///< ���݂̃l�X�g���x���Ɋ܂܂�Ă���l�̐��B=�z��̗v�f���܂��̓I�u�W�F�N�g�̃����o��
		bool	InArray;	///< true �Ȃ�z��Afalse �Ȃ�I�u�W�F�N�g
		bool	justSawKey;
	};

	TextWriter*		m_textWriter;
	Stack<Level>	m_levelStack;
	bool			m_isComplete;
};

} // namespace Lumino
