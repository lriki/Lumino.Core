
#include "../Internal.h"
#include <Lumino/Base/DllLoader.h>

namespace Lumino
{

//=============================================================================
// DllLoader
//=============================================================================

static void* LoadDLL(const TCHAR* filePath)
{
#ifdef _WIN32
	return ::LoadLibrary(filePath);
#else
	LN_LOCAL_MBCS_FILEPATH(mbcsPath, filePath);
	returndlopen(mbcsPath, RTLD_LAZY);
#endif
}

static void UnloadDLL(void* module)
{
#ifdef _WIN32
	::FreeLibrary((HMODULE)module);
#else
	dlclose(module);
#endif
}

static void* GetProcAddr(void* module, const char* procName)
{
#ifdef _WIN32
	return ::GetProcAddress((HMODULE)module, procName);
#else
	return dlsym(module, procName);
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DllLoader::DllLoader()
	: m_module(NULL)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DllLoader::DllLoader(const TCHAR* filePath)
	: m_module(NULL)
{
	Load(filePath);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
DllLoader::~DllLoader()
{
	Unload();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool DllLoader::Exists(const TCHAR* filePath)
{
	// ロードできるかでチェック
	void* module = LoadDLL(filePath);
	if (module != NULL)
	{
		UnloadDLL(module);
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DllLoader::Load(const TCHAR* filePath)
{
	Unload();
	m_module = LoadDLL(filePath);
	LN_THROW(m_module != NULL, FileNotFoundException);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void DllLoader::Unload()
{
	if (m_module != NULL)
	{
		UnloadDLL(m_module);
		m_module = NULL;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void* DllLoader::GetProcAddress(const char* procName)
{
	LN_THROW(m_module != NULL, InvalidOperationException);
	void* proc = GetProcAddr(m_module, procName);
	LN_THROW(proc != NULL, InvalidOperationException);
	return proc;
}

} // namespace Lumino
