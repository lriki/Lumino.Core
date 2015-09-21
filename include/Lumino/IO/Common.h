﻿
#pragma once
#include "../Base/EnumExtension.h"
#include "../Base/Common.h"

namespace Lumino
{
class IArchive;






template<typename TEnum>
class EnumFlags
{
public:
	typedef TEnum enum_type;

	//EnumFlags()
};

#define LN_DECLARE_FLAGS(Flags, Enum) typedef EnumFlags<Enum> Flags;

#define LN_DECLARE_OPERATORS_FOR_FLAGS(FlagsEnumClass) \
	inline EnumFlags<FlagsEnumClass::enum_type> operator|(FlagsEnameType::enum_type f1, FlagsEnumClass::enum_type f2) throw() \
	{ \
		return EnumFlags<FlagsEnumClass::enum_type>(f1) | f2; \
	}
	//inline EnumFlags<FlagsEnumClass::enum_type> operator|(FlagsEnumClass::enum_type f1, EnumFlags<FlagsEnumClass::enum_type> f2) throw() \
	//{ \
	//	return f2 | f1; \
	//}

#define LN_CONSTEXPR

/** ファイルを開く際のアクセスモードを表します。*/
LN_ENUM_FLAGS(FileOpenMode)
{
	None = 0x0000,
	Read = 0x0001,				/**< 読み取りアクセス*/
	Write = 0x0002,				/**< 書き込みアクセス*/
	ReadWrite = Read | Write,	/**< 読み取り及び書き込みアクセス*/

	Append = 0x0004,			/**< ファイルを開き、ファイルの末尾をシークする (追記モード)*/
	Truncate = 0x0008,			/**< ファイルが存在する場合、サイズを 0 にする*/

	Deferring = 0x0100,			/**< ファイルを遅延モードで開く*/
};
LN_ENUM_FLAGS_DECLARE(FileOpenMode);

#if 0
///< ファイルを開く方法
class FileOpenMode
{
public:

	enum _FileOpenMode	// この enum の型名は必ず必要。
	{
	};
	//LN_DECLARE_FLAGS(Flags, _FileOpenMode);


	typedef _FileOpenMode enum_type;
	FileOpenMode() : m_value(0){}
	FileOpenMode(_FileOpenMode v)
	{
		m_value = v;
	}
	//FileOpenMode()
	//FileOpenMode operator&(_FileOpenMode right)
	//{
	//	FileOpenMode value;
	//	value.m_value = m_value & right;
	//	return value;
	//}
	FileOpenMode operator|(_FileOpenMode right)
	{
		FileOpenMode value;
		value.m_value = m_value | right;
		return value;
	}

	friend inline FileOpenMode operator&(const FileOpenMode& f1, FileOpenMode::enum_type f2) throw();

	LN_CONSTEXPR inline bool TestFlag(_FileOpenMode f) const throw() { return (m_value & f) == f && (f != 0 || m_value == f); }
private:
	int	m_value;
};
inline FileOpenMode operator&(const FileOpenMode& f1, FileOpenMode::enum_type f2) throw()
{
	FileOpenMode value;
	value.m_value = f1.m_value & f2;
	return value;
}
inline FileOpenMode operator|(FileOpenMode::enum_type f1, FileOpenMode::enum_type f2) throw()
{
	return FileOpenMode(f1) | f2;
}


//LN_DECLARE_OPERATORS_FOR_FLAGS(FileOpenMode);
//enum FileOpenMode
//{
//	FileOpenMode_Read		= 0x0001,									///< 読み取りアクセス
//	FileOpenMode_Write		= 0x0002,									///< 書き込みアクセス
//	FileOpenMode_ReadWrite	= FileOpenMode_Read | FileOpenMode_Write,	///< 読み取り及び書き込みアクセス
//
//	FileOpenMode_Append		= 0x0004,		///< ファイルを開き、ファイルの末尾をシークする (追記モード)
//	FileOpenMode_Truncate	= 0x0008,		///< ファイルが存在する場合、サイズを 0 にする
//
//	FileOpenMode_Deferring	= 0x0100,		///< ファイルを遅延モードで開く
//};

#endif



///< ファイルを書き込みモードでオープンする際の概要
enum FileWriteMode
{
	FileWriteMode_Truncate = 0,				///< ファイルが存在する場合、サイズを 0 にする
	FileWriteMode_Append,					///< ファイルを開き、ファイルの末尾をシークする (追記モード)
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
	ASyncIOState_Completed,					///< 終了状態 (正常終了)
	ASyncIOState_Failed,					///< 終了状態 (処理終了後・処理に失敗した)

	ASyncIOState_Max,
};

/** プロセスの状態 */
LN_ENUM(ProcessStatus)
{
	Running = 0,			/**< 実行中 */
	Finished,				/**< 正常終了した */
	Crashed,				/**< 異常終了した */
};
LN_ENUM_DECLARE(ProcessStatus);

/** 特別なフォルダのパスを取得を取得する際のオプションです。*/
LN_ENUM(SpecialFolderOption)
{
	None = 0,				/**< フォルダが存在しない場合空の文字列を返します。*/
	Create,					/**< フォルダが存在しない場合は作成します。*/
};
LN_ENUM_DECLARE(SpecialFolderOption);

#pragma pack(1)
struct ArchiveHeader
{
	char	ID[4];			///< 'l' 'n' 'a' ' '
	uint8_t	Version;		///< ファイルバージョン
	byte_t	Padding[11];	///< 構造体サイズが 16 の倍数になるようにするためのパディング
};
#pragma pack()

} // namespace Lumino
