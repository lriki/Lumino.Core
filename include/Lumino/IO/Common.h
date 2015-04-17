#pragma once

#include "../Base/Common.h"

namespace Lumino
{
class IArchive;

/// ファイルを開く方法または作成する方法
enum FileMode
{
	FileMode_Create = 0,		///< 新しいファイルを作成する。既に存在する場合は上書きされる
	FileMode_Open,				///< 既存のファイルを開く
	FileMode_Append,			///< ファイルを開き、ファイルの末尾をシークする。存在しない場合は新しいファイルを作成。

	FileMode_Max,				///< (terminator)
};

/// ファイルにアクセスする方法
enum FileAccess
{
	FileAccess_Read = 0,		///< 読み取りアクセス
	FileAccess_ReadWrite,		///< 読み取り及び書き込みアクセス
	FileAccess_Write,			///< 書き込みアクセス

	FileAccess_Max,				///< (terminator)
};

/// ファイルの共有方法
enum FileShare
{
	FileShare_None = 0,			///< 他のすべての読み書きを拒否する
	FileShare_Read,				///< 他が読み取り専用でファイルを開くことを許可する
	FileShare_Write,			///< 他が書き込み専用でファイルを開くことを許可する
	FileShare_ReadWrite,		///< 他が読み取りまたは書き込みでファイルを開くことを許可する (すべての共有を許可する)

	FileShare_Max,				///< (terminator)
};

/// ファイルへのアクセス優先度
enum FileAccessPriority
{
	FileAccessPriority_ArchiveFirst = 0,	///< アーカイブ優先
	FileAccessPriority_DirectoryFirst,		///< ディレクトリ優先
	FileAccessPriority_ArchiveOnly,			///< アーカイブのみ   
};

#pragma pack(1)
struct ArchiveHeader
{
	char	ID[4];			///< 'l' 'n' 'a' ' '
	uint8_t	Version;		///< ファイルバージョン
	byte_t	Padding[11];	///< 構造体サイズが 16 の倍数になるようにするためのパディング
};
#pragma pack()

} // namespace Lumino
