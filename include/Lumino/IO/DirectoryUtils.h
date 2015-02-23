#pragma once

#include "../Base/Array.h"

namespace Lumino
{

/**
	@brief	
*/
class DirectoryUtils
{
public:

	/**
		@brief		ディレクトリ内に含まれる全てのファイルのパスを取得する
		@param[in]	dirPath	: 検索するディレクトリ
		@param[in]	pattern	: 検索文字列 (ワイルドカード指定可能)
	*/
	static Array<String> GetFiles(const TCHAR* dirPath, const TCHAR* pattern);
};

} // namespace Lumino
