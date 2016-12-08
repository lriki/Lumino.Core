
#pragma once
#include "../IO/TextReader.h"
#include "JsonValue.h"

LN_NAMESPACE_BEGIN
namespace tr {
class JsonWriter;
class JsonObject2;
class JsonDocument2;

/**
	@brief	JSON データのルート要素です。
*/
class JsonDocument
	: public JsonValue
{
public:

	/*
		@brief	指定した JSON 形式文字列を解析し、ドキュメントを構築します。
	*/
	void Parse(const String& text);

	/*
		@brief	指定した JSON 形式文字列を解析し、ドキュメントを構築します。
	*/
	void Parse(const TCHAR* text, int len = -1);

	/*
		@brief	指定した TextReader から JSON 形式文字列を解析し、ドキュメントを構築します。
	*/
	void Parse(TextReader* textReader);

	//void Load();

private:
};






/** Json の値の型を示します。*/
enum class JsonValueType
{
	Null,
	Bool,
	Int32,
	Int64,
	Float,
	Double,
	String,
	Array,
	Object,
};



/**
	@brief	
*/
class JsonElement2
	: public RefObject
{
public:
	JsonValueType GetType() const { return m_type; }

protected:
	JsonElement2(JsonDocument2* owner);
	virtual ~JsonElement2();
	virtual void OnSave(JsonWriter* writer) = 0;

LN_INTERNAL_ACCESS:
	JsonDocument2* GetOwnerDocument() const { return m_ownerDoc; }
	void SetType(JsonValueType type) { m_type = type; }
	void Save(JsonWriter* writer) { OnSave(writer); }

private:
	JsonDocument2*	m_ownerDoc;
	JsonValueType	m_type;

	friend class JsonObject2;
};

/**
	@brief	
*/
class JsonValue2
	: public JsonElement2
{
public:
	void SetNull();
	void SetBool(bool value);
	void SetInt32(int32_t value);
	void SetInt64(int64_t value);
	void SetFloat(float value);
	void SetDouble(double value);
	void SetString(const StringRef& value);

	bool IsNull() const;
	bool GetBool() const;
	int32_t GetInt32() const;
	int64_t GetInt64() const;
	float GetFloat() const;
	double GetDouble() const;
	String GetString() const;

private:
	JsonValue2(JsonDocument2* ownerDoc);
	virtual ~JsonValue2();
	void CheckRelease();

	virtual void OnSave(JsonWriter* writer) override;

	union
	{
		bool	m_bool;
		int32_t	m_int32;
		int64_t	m_int64;
		float	m_float;
		double	m_double;
		ln::detail::GenericStringCore<TCHAR>*	m_stringCore;
	};

	friend class JsonDocument2;
};

/**
	@brief	
*/
class JsonArray2
	: public JsonElement2
{
public:

	void AddNull();
	void AddBool(bool value);
	void AddInt32(int32_t value);
	void AddInt64(int64_t value);
	void AddFloat(float value);
	void AddDouble(double value);
	void AddString(const StringRef& value);

	JsonArray2* AddArray();
	JsonObject2* AddObject();

private:
	JsonArray2(JsonDocument2* ownerDoc);
	virtual ~JsonArray2();
	virtual void OnSave(JsonWriter* writer) override;

	List<JsonElement2*>	m_itemList;

	friend class JsonDocument2;
};

/**
	@brief	
*/
class JsonObject2
	: public JsonElement2
{
public:

	void AddMemberNull(const StringRef& name);
	void AddMemberBool(const StringRef& name, bool value);
	void AddMemberInt32(const StringRef& name, int32_t value);
	void AddMemberInt64(const StringRef& name, int64_t value);
	void AddMemberFloat(const StringRef& name, float value);
	void AddMemberDouble(const StringRef& name, double value);
	void AddMemberString(const StringRef& name, const StringRef& value);

	JsonArray2* AddMemberArray(const StringRef& name);
	JsonObject2* AddMemberObject(const StringRef& name);

protected:
	JsonObject2(JsonDocument2* ownerDoc);
	virtual ~JsonObject2();
	virtual void OnSave(JsonWriter* writer) override;

LN_INTERNAL_ACCESS:
	static bool IsValueType(JsonValueType type);
	void Finalize() { m_memberList.Clear(); }

private:
	JsonValue2* GetValue(const StringRef& name);

	struct Member
	{
		String			name;
		JsonElement2*	value;
	};

	List<Member>	m_memberList;

	friend class JsonDocument2;
};

namespace detail
{

class JsonElementCache
{
public:
	void Initialize();
	void* Alloc(size_t size);

private:
	static const size_t BufferSize = 2048;

	struct BufferInfo
	{
		ByteBuffer	buffer;
		size_t		used;
	};
	List<BufferInfo>	m_buffers;
};

} // namespace detail


/**
	@brief	JSON データのルート要素です。
*/
class JsonDocument2
	: public JsonObject2
{
public:
	JsonDocument2();
	virtual ~JsonDocument2();

	///*
	//	@brief	指定した JSON 形式文字列を解析し、ドキュメントを構築します。
	//*/
	//void Parse(const String& text);

	///*
	//	@brief	指定した JSON 形式文字列を解析し、ドキュメントを構築します。
	//*/
	//void Parse(const TCHAR* text, int len = -1);

	///*
	//	@brief	指定した TextReader から JSON 形式文字列を解析し、ドキュメントを構築します。
	//*/
	//void Parse(TextReader* textReader);

	//void Load();

	void Save(const StringRef& filePath);

LN_INTERNAL_ACCESS:
	template<class T>
	T* NewElement()
	{
		T* buf = reinterpret_cast<T*>(m_cache.Alloc(sizeof(T)));
		new (buf)T(this);
		return buf;
	}

private:
	detail::JsonElementCache	m_cache;
};




} // namespace tr
LN_NAMESPACE_END
