
#include "../Internal.h"
#include <Lumino/Xml/XmlReader.h>
#include <Lumino/Base/Resource.h>

LN_NAMESPACE_BEGIN
	
//==============================================================================
// ResourceManager
//==============================================================================

//------------------------------------------------------------------------------
const String& ResourceManager::GetString(const String& name)
{
	std::map<String, String>::iterator itr = m_stringMap.find(name);
	if (itr != m_stringMap.end()) {
		return itr->second;
	}
	return String::GetEmpty();
}

//------------------------------------------------------------------------------
void ResourceManager::AddString(const String& name, const String& value)
{
	m_stringMap[name] = value;
}


//==============================================================================
// InternalResource
//==============================================================================

const String InternalResource::UnknownError(_T("UnknownError"));
const String InternalResource::VerifyError(_T("VerifyError"));
const String InternalResource::ArgumentError(_T("ArgumentError"));
const String InternalResource::InvalidOperationError(_T("InvalidOperationError"));
const String InternalResource::OutOfMemoryError(_T("OutOfMemoryError"));
const String InternalResource::OutOfRangeError(_T("OutOfRangeError"));
const String InternalResource::IOError(_T("IOError"));
const String InternalResource::EndOfStreamError(_T("EndOfStreamError"));
const String InternalResource::FileNotFoundError(_T("FileNotFoundError"));
const String InternalResource::DirectoryNotFoundError(_T("DirectoryNotFoundError"));
const String InternalResource::InvalidFormatError(_T("InvalidFormatError"));
const String InternalResource::NotImplementedError(_T("NotImplementedError"));
const String InternalResource::KeyNotFoundError(_T("KeyNotFoundError"));
const String InternalResource::EncodingError(_T("EncodingError"));
const String InternalResource::SystemError(_T("SystemError"));
const String InternalResource::OverflowError(_T("OverflowError"));
const String InternalResource::Win32Error(_T("Win32Error"));
const String InternalResource::ComError(_T("ComError"));

const String InternalResource::OpenGLError(_T("OpenGLError"));

class InternalResourceManager : public ResourceManager
{
public:
	InternalResourceManager()
	{
		AddString(InternalResource::UnknownError, _T("不明なエラーが発生しました。"));
		AddString(InternalResource::VerifyError, _T("前提条件の検証エラーです。"));
		AddString(InternalResource::ArgumentError, _T("引数に無効な値が渡されました。"));
		AddString(InternalResource::InvalidOperationError, _T("オブジェクトの状態に対して無効な呼び出しが行われました。"));
		AddString(InternalResource::NotImplementedError, _T("未実装の機能を呼び出しました。"));
		AddString(InternalResource::OutOfMemoryError, _T("十分なメモリ領域がありません。"));
		AddString(InternalResource::OutOfRangeError, _T("値が有効な範囲にありません。"));
		AddString(InternalResource::KeyNotFoundError, _T("指定されたキーは存在しませんでした。"));
		AddString(InternalResource::OverflowError, _T("数値演算によるオーバーフローが発生しました。"));
		AddString(InternalResource::IOError, _T("I/Oエラーが発生しました。"));
		AddString(InternalResource::FileNotFoundError, _T("指定されたファイルが見つかりません。"));
		AddString(InternalResource::DirectoryNotFoundError, _T("指定されたディレクトリが見つかりません。"));
		AddString(InternalResource::InvalidFormatError, _T("入力データの形式が不正、あるいはサポートされていません。"));
		AddString(InternalResource::EndOfStreamError, _T("ストリームの末尾を越えてアクセスしようとしました。"));
		AddString(InternalResource::EncodingError, _T("マッピングできない文字または不正シーケンスが見つかりました。"));
		AddString(InternalResource::SystemError, _T("システムAPIの呼び出しで予期しないエラーが発生しました。"));
		AddString(InternalResource::Win32Error, _T("WindowsAPI の呼び出しでエラーが発生しました。"));
		AddString(InternalResource::ComError, _T("COM の呼び出しでエラーが発生しました。"));

		AddString(InternalResource::OpenGLError, _T("OpenGL の呼び出しでエラーが発生しました。"));
	}
};
static InternalResourceManager m_internalResource;


//------------------------------------------------------------------------------
const String& InternalResource::GetString(const String& name)
{
	return m_internalResource.GetString(name);
}


//==============================================================================
// ResourceSet
//==============================================================================
class ResourceSet
{
public:
	ResourceSet() {}
	~ResourceSet() {}

	void LoadResourceFile(const String& filePath);
	String* GetString(const String& name);

private:
	LN_DISALLOW_COPY_AND_ASSIGN(ResourceSet);
	std::map<String, String>	m_stringMap;
};

//------------------------------------------------------------------------------
void ResourceSet::LoadResourceFile(const String& filePath)
{
	String name;
	String value;
	XmlFileReader reader(filePath);
	while (reader.Read())
	{
		if (reader.GetNodeType() == XmlNodeType::Element &&
			reader.GetName() == _T("string"))
		{
			if (reader.MoveToFirstAttribute())
			{
				do {
					if (reader.GetName() == _T("name")) { name = reader.GetValue(); }

				} while (reader.MoveToNextAttribute());
			}
		}
		if (reader.GetNodeType() == XmlNodeType::Text)
		{
			value = reader.GetValue();
		}
		if (reader.GetNodeType() == XmlNodeType::EndElement &&
			reader.GetName() == _T("string"))
		{
			m_stringMap[name] = value;
		}
	}
}

//------------------------------------------------------------------------------
String* ResourceSet::GetString(const String& name)
{
	std::map<String, String>::iterator itr = m_stringMap.find(name);
	if (itr != m_stringMap.end()) {
		return &itr->second;
	}
	return NULL;
}


//==============================================================================
// Resources
//==============================================================================

static ResourceSet g_resourceSet;

//------------------------------------------------------------------------------
void Resources::RegisterResource(const String& filePath)
{
	g_resourceSet.LoadResourceFile(filePath);
}

//------------------------------------------------------------------------------
const String& Resources::GetString(const String& name)
{
	String* str = g_resourceSet.GetString(name);
	LN_THROW(str != NULL, KeyNotFoundException, _T("Not found resource key."));
	return *str;
}



LN_NAMESPACE_END
