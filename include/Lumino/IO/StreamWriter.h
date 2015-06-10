
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
		@brief		パス名で指定したファイル用の StreamWriter を初期化します。
		@param[in]	filePath	: 書き込み先ファイルのパス
		@param[in]	encoding	: 書き込み時に使用するエンコーディング (NULL の場合、文字コード変換は行わない)
		@param[in]	mode		: ファイルのオープンモード
	*/
	StreamWriter(const PathName& filePath, Text::Encoding* encoding = NULL, FileWriteMode mode = FileWriteMode_Truncate);

	virtual ~StreamWriter();
	
private:
	virtual void Flash();
	virtual void WriteOverride(const void* data, size_t byteCount);

private:
	RefPtr<Stream>	m_stream;
};

} // namespace Lumino
