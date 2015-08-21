
#pragma once
#include "Stream.h"
#include "PathName.h"
#include "TextWriter.h"

namespace Lumino
{

/**
	@brief	文字列を特定のエンコーディングでストリームに書き込むための TextWriter の実装です。
*/
class StreamWriter
	: public TextWriter
{
public:

	/**
		@brief		指定したストリーム用の StreamWriter を初期化します。
		@param[in]	filePath	: 書き込み先ファイルのパス
		@param[in]	encoding	: 書き込み時に使用するエンコーディング
		@details	encoding が NULL の場合は文字列の書き込み時に文字コードのデコードを行いません。
	*/
	StreamWriter(Stream* stream, Text::Encoding* encoding = NULL);

	/**
		@brief		パス名で指定したファイル用の StreamWriter を初期化します。
		@param[in]	filePath	: 書き込み先ファイルのパス
		@param[in]	encoding	: 書き込み時に使用するエンコーディング
		@param[in]	mode		: ファイルのオープンモード
		@details	encoding が NULL の場合は文字列の書き込み時に文字コードのデコードを行いません。
	*/
	StreamWriter(const PathName& filePath, Text::Encoding* encoding = NULL, FileWriteMode mode = FileWriteMode_Truncate);

	virtual ~StreamWriter();
	
private:
	void Init(Stream* stream, Text::Encoding* encoding);
	virtual void Flash();
	virtual void WriteOverride(const void* data, size_t byteCount);

private:
	RefPtr<Stream>	m_stream;
};

} // namespace Lumino
