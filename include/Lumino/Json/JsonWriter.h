
#pragma once
#include "../Base/Stack.h"
#include "../IO/TextWriter.h"

LN_NAMESPACE_BEGIN

/**
	@brief	
*/
class JsonWriter
{
public:
	JsonWriter(TextWriter* textWriter);
	virtual ~JsonWriter();

public:
	void WriteStartObject();
	void WriteEndObject();
	void WriteStartArray();
	void WriteEndArray();
	void WritePropertyName(const TCHAR* str, int length = -1);
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
	void AutoComplete();

private:
	struct Level
	{
		Level(bool inArray) : valueCount(0), inArray(inArray), justSawKey(false) {}
		int		valueCount;	///< 現在のネストレベルに含まれている値の数。=配列の要素数またはオブジェクトのメンバ数
		bool	inArray;	///< true なら配列、false ならオブジェクト
		bool	justSawKey;
	};

	TextWriter*		m_textWriter;
	Stack<Level>	m_levelStack;
	bool			m_isComplete;
};

LN_NAMESPACE_END
