#pragma once

#include "../Base/Common.h"

namespace Lumino
{
class IArchive;

///< ファイルを開く方法
enum FileOpenMode
{
	FileOpenMode_Read		= 0x0001,									///< 読み取りアクセス
	FileOpenMode_Write		= 0x0002,									///< 書き込みアクセス
	FileOpenMode_ReadWrite	= FileOpenMode_Read | FileOpenMode_Write,	///< 読み取り及び書き込みアクセス

	FileOpenMode_Append		= 0x0004,		///< ファイルを開き、ファイルの末尾をシークする
	FileOpenMode_Truncate	= 0x0008,		///< ファイルが存在する場合、サイズを 0 にする
};

#if 0
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
#endif

/// ファイルへのアクセス優先度
enum FileAccessPriority
{
	FileAccessPriority_ArchiveFirst = 0,	///< アーカイブ優先
	FileAccessPriority_DirectoryFirst,		///< ディレクトリ優先
	FileAccessPriority_ArchiveOnly,			///< アーカイブのみ   
};

/// シーク位置の指定の基準
enum SeekOrigin
{
	SeekOrigin_Begin	= SEEK_SET,			/// ストリームの先頭
	SeekOrigin_Current	= SEEK_CUR,			/// ストリームの現在位置
	SeekOrigin_End		= SEEK_END,			/// ストリームの末尾
};

/// 非同期処理の状態
enum ASyncIOState
{
	ASyncIOState_Idle = 0,					///< 待機状態 (初期状態)
	ASyncIOState_Ready,						///< 実行可能状態 (実行待ちキューに入っている状態)
	ASyncIOState_Processing,				///< 実行状態 (処理中)
	ASyncIOState_Completed,					///< 終了状態 (処理終了後)
	ASyncIOState_Failed,					///< 終了状態 (処理終了後・処理に失敗した)

	ASyncIOState_Max,
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
