#pragma once

#include "Stream.h"
#include "PathName.h"

namespace Lumino
{
	
/// ファイルへのアクセス優先度
enum FileAccessPriority
{
	FileAccessPriority_ArchiveFirst = 0,	///< アーカイブ優先
	FileAccessPriority_DirectoryFirst,		///< ディレクトリ優先
	FileAccessPriority_ArchiveOnly,			///< アーカイブのみ   
};

/**
	@brief	
*/
class FileManager
    : public RefObject
{
public:
	/// 初期化データ
	struct ConfigData
	{
		FileAccessPriority	AccessMode;     ///< ファイルへのアクセス優先度
	};

public:
	FileManager(const ConfigData& configData);
	virtual ~FileManager();

public:

	/*
		@brief		指定されたファイルが存在するかどうかを確認します。
		@details	初期化時に指定したアクセス優先度に従い、登録されているアーカイブがあればその内部も確認します。
	*/
	bool ExistsFile(const char* filePath);
	bool ExistsFile(const wchar_t* filePath);	///< @overload ExistsFile
	bool ExistsFile(const PathName& filePath);	///< @overload ExistsFile

	/*
		@brief		読み取り専用モードでファイルストリームを開きます。
		@details	初期化時に指定したアクセス優先度に従い、登録されているアーカイブがあればその内部も確認します。
	*/
	Stream* CreateFileStream(const char* filePath);
	Stream* CreateFileStream(const wchar_t* filePath);	///< @overload CreateInFileStream
	Stream* CreateFileStream(const PathName* filePath);	///< @overload CreateInFileStream

	/*
		@brief		リクエストされているすべての非同期読み込み/書き込み処理の終了を待機します。
	*/
	void WaitForAllASyncProcess();
};

} // namespace Lumino
