
#include "../Internal.h"
#include <Lumino/Base/Resource.h>

namespace Lumino
{
	
//=============================================================================
// ResourceManager
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const String& ResourceManager::GetString(const String& name)
{
	std::map<String, String>::iterator itr = m_stringMap.find(name);
	if (itr != m_stringMap.end()) {
		return itr->second;
	}
	return String::GetEmpty();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void ResourceManager::AddString(const String& name, const String& value)
{
	m_stringMap[name] = value;
}


//=============================================================================
// InternalResource
//=============================================================================


const String InternalResource::OutOfMemory(_T("OutOfMemory"));

class InternalResourceManager : public ResourceManager
{
public:
	InternalResourceManager()
	{
		AddString(InternalResource::OutOfMemory, _T("è\ï™Ç»ÉÅÉÇÉäóÃàÊÇ™Ç†ÇËÇ‹ÇπÇÒÅB"));
	}
};
static InternalResourceManager m_internalResource;


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const String& InternalResource::GetString(const String& name)
{
	return m_internalResource.GetString(name);
}



} // namespace Lumino
