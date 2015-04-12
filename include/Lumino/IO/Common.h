#pragma once

namespace Lumino
{

#pragma pack(1)
struct ArchiveHeader
{
	char	ID[4];			///< 'l' 'n' 'a' ' '
	uint8_t	Version;		///< ファイルバージョン
	byte_t	Padding[11];	///< 構造体サイズが 16 の倍数になるようにするためのパディング
};
#pragma pack()

} // namespace Lumino
