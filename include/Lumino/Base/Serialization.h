
#pragma once

LN_NAMESPACE_BEGIN
namespace tr {

enum class ArchiveMode
{
	Save,
	Load,
};



/**
	@brief
*/
template<typename TRef>
class NameValuePair
{
public:
	const TCHAR* name;
	TRef& value;

	NameValuePair(const TCHAR* n, TRef& v) : name(n), value(v) {}

private:
	NameValuePair & operator=(NameValuePair const &) = delete;
};

template<typename TRef>
NameValuePair<TRef> MakeNVP(const TCHAR* name, TRef& valueRef)
{
	return NameValuePair<TRef>(name, valueRef);
}



/**
	@brief
*/
class ISerializeObjectElement
{
public:

	virtual void SetValueInt32(const TCHAR* name, int32_t value) = 0;
	virtual void SetValueDouble(const TCHAR* name, int32_t value) = 0;
};

/**
	@brief
*/
class ISerializeArrayElement
{
public:

};

/**
	@brief
*/
class Archive
{
public:
	//Archive(const PathName& filePath, ArchiveMode mode);

	//template<typename T> Archive& operator&(T && arg)
	//{
	//	Process(std::forward<T>(arg));
	//	return *self;
	//}

	template<class T>
	void Serialze(T* obj)
	{
		obj->lnsl_SerializeImpl(*this);
	}

	template<class TRef>
	Archive& operator & (const NameValuePair<TRef>& nvp)
	{
		Process(nvp.name, nvp.value);
		return *this;
	}

private:

	template<typename T> void Process(const TCHAR* name, T && value)
	{
		//switch (mode)
		//{
		//case ArchiveMode::Save:
		//	ProcessWrite(std::forward<T>(value));
		//	break;
		//case ArchiveMode::Load:
		//	ProcessRead(std::forward<T>(value));
		//	break;
		//default:
		//	assert(0);
		//	break;
		//}
	}

	template<typename T>
	void ProcessWrite(const TCHAR* name, T && value)
	{
		WriteValue(name, value);
	}

	template<typename T>
	void ProcessRead(const TCHAR* name, T && value)
	{
	}

	//void WriteValue(const TCHAR* name, SerializableObject* obj);
	void WriteValue(const TCHAR* name, bool value);
	void WriteValue(const TCHAR* name, int8_t value);
	void WriteValue(const TCHAR* name, int16_t value);
	void WriteValue(const TCHAR* name, int32_t value);
	void WriteValue(const TCHAR* name, int64_t value);
	void WriteValue(const TCHAR* name, uint8_t value);
	void WriteValue(const TCHAR* name, uint16_t value);
	void WriteValue(const TCHAR* name, uint32_t value);
	void WriteValue(const TCHAR* name, uint64_t value);
	void WriteValue(const TCHAR* name, float value);
	void WriteValue(const TCHAR* name, double value);
	template<typename T> void WriteValue(const TCHAR* name, T && obj);	 // nonÅ]intrusive Object


	ArchiveMode					m_mode;
//	ISerializeSourceElement*	m_currentObject;
};

} // namespace tr
LN_NAMESPACE_END
