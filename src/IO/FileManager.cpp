
#include "../Internal.h"
#include "Archive.h"
#include <Lumino/IO/FileUtils.h>
#include <Lumino/IO/FileManager.h>

namespace Lumino
{

//=============================================================================
// FileManager
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FileManager& FileManager::GetInstance()
{
	static FileManager instance;
	return instance;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FileManager::FileManager()
	: m_fileAccessPriority(FileAccessPriority_DirectoryFirst)
	, m_dummyArchive(LN_NEW DummyArchive())
{
	m_archiveList.Add(m_dummyArchive);
	m_dummyArchive->AddRef();	// m_archiveList からの参照を示す
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
FileManager::~FileManager()
{
	LN_FOREACH(IArchive* a, m_archiveList) {
		a->Release();
	}
	m_archiveList.Clear();

	LN_SAFE_RELEASE(m_dummyArchive);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileManager::RegisterArchive(const PathName& filePath, const String& password)
{
	Threading::MutexScopedLock lock(m_mutex);

	Archive* archive = LN_NEW Archive();
	archive->Open(filePath, password);
	m_archiveList.Add(archive);

	RefreshArchiveList();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool FileManager::ExistsFile(const char* filePath)
{
	return ExistsFile(PathName(filePath));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool FileManager::ExistsFile(const wchar_t* filePath)
{
	return ExistsFile(PathName(filePath));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool FileManager::ExistsFile(const PathName& filePath)
{
	LN_FOREACH(IArchive* archive, m_archiveList)
	{
		if (archive->ExistsFile(filePath)) {
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Stream* FileManager::CreateFileStream(const char* filePath)
{
	return CreateFileStream(PathName(filePath));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Stream* FileManager::CreateFileStream(const wchar_t* filePath)
{
	return CreateFileStream(PathName(filePath));
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Stream* FileManager::CreateFileStream(const PathName& filePath)
{
	PathName absPath = filePath.CanonicalizePath();

	Stream* stream;
	LN_FOREACH(IArchive* archive, m_archiveList)
	{
		if (archive->TryCreateStream(absPath, &stream)) {
			break;
		}
	}

	LN_THROW(stream != NULL, FileNotFoundException, absPath);	// ファイルが見つからなかった
	return stream;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CaseSensitivity FileManager::GetFileSystemCaseSensitivity() const
{
#ifdef LN_WIN32
	return CaseSensitivity_CaseInsensitive;
#else
	return CaseSensitivity_CaseSensitive;
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void FileManager::RefreshArchiveList()
{
	// 一度ダミーをリストから外す
	if (m_archiveList.Contains(m_dummyArchive))
	{
		m_archiveList.Remove(m_dummyArchive);
		m_dummyArchive->Release();
	}

	// ディレクトリ優先ならダミーを先頭に追加し直す
	if (m_fileAccessPriority == FileAccessPriority_DirectoryFirst) {
		m_archiveList.Insert(0, m_dummyArchive);
		m_dummyArchive->AddRef();
	}
	// アーカイブ優先ならダミーを末尾に追加し直す
	else if (m_fileAccessPriority == FileAccessPriority_ArchiveFirst) {
		m_archiveList.Add(m_dummyArchive);
		m_dummyArchive->AddRef();
	}
	// アーカイブのみであればダミーを追加する必要は無い
	else {
	}
}

} // namespace Lumino
