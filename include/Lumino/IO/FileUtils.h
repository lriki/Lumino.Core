#pragma once

#include <stdio.h>
#include "../Base/RefObject.h"
#include "../Base/String.h"

namespace Lumino
{
namespace Text { class Encoding; }
class RefBuffer;

/// ファイルとディレクトリの属性
enum FileAttribute
{
	FileAttribute_Normal		= 0x0000,	///< 特に属性を持たない通常のファイル
	FileAttribute_Directory		= 0x0001,	///< ディレクトリ
	FileAttribute_ReadOnly		= 0x0002,	///< 読み取り専用
	FileAttribute_Hidden		= 0x0004,	///< 隠しファイル
};

/**
	@brief	ファイルユーティリティ
*/
class FileUtils
{
public:
	/// fopen の template 実装
	//template<typename TChar>
	//static FILE* fopen(const TChar* filePath, const TChar* pMode);
	
	/// 指定されたファイルが存在するか確認する (パスが空文字や NULL の場合も false を返す)
	static bool Exists(const char* filePath);
	static bool Exists(const wchar_t* filePath);

	/**
		@brief		ファイルの属性を取得する
		@param[in]	filePath		: ファイル名
		@return		ファイルの属性 (FileAttribute のビットの組み合わせ)
	*/
	static uint32_t GetAttribute(const char* filePath);
	static uint32_t GetAttribute(const wchar_t* filePath);

	/**
		@brief		ファイルの属性を設定する
		@param[in]	filePath		: ファイル名
		@param[in]	attr			: ファイルの属性 (FileAttribute のビットの組み合わせ)
		@details	
	*/
	static void SetAttribute(const char* filePath, uint32_t attr);
	static void SetAttribute(const wchar_t* filePath, uint32_t attr);

	/**
		@brief		ファイルをコピーする
		@param[in]	sourceFileName	: コピー元ファイル名
		@param[in]	destFileName	: コピー先ファイル名
		@param[in]	overwrite		: コピー先を上書きする場合は true
		@details	読み取り専用ファイルに上書きすることはできません。
	*/
	static void Copy(const char* sourceFileName, const char* destFileName, bool overwrite);
	static void Copy(const wchar_t* sourceFileName, const wchar_t* destFileName, bool overwrite);

	/**
		@brief		ファイルを削除する
		@param[in]	filePath		: 削除するファイル名
		@details	削除するファイルが存在しない場合、例外はスローされません。
	*/
	static void Delete(const char* filePath);
	static void Delete(const wchar_t* filePath);

	/// ファイルサイズを取得する
	static size_t GetFileSize(const TCHAR* filePath);

	/// ファイルサイズを取得する
	static size_t GetFileSize( FILE* stream );

	/// ファイルの内容をすべて読み込む (バイナリ形式)
	static RefBuffer* ReadAllBytes(const char* filePath);
	static RefBuffer* ReadAllBytes(const wchar_t* filePath);

	/// ファイルの内容をすべて読み込み、文字列として返す
	static String ReadAllText(const TCHAR* filePath, const Text::Encoding* encoding = NULL);

	/// 配列の内容をバイナリファイルとして書き出す
	static void WriteAllBytes(const TCHAR* filePath, const void* buffer, size_t size);

	/// 文字列をテキストファイルとして書き出す
	static void WriteAllText(const TCHAR* filePath, const String& str, const Text::Encoding* encoding = NULL);
};

} // namespace Lumino
