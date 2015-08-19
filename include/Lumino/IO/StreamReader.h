/**
	@file	StreamReader.h
*/
#pragma once
#include "../Text/Encoding.h"
#include "../Text/EncodingConverter.h"
#include "Stream.h"
#include "TextReader.h"

namespace Lumino
{

/**
	@brief	特定のエンコーディングのストリームを文字列として読み込む TextReader です。
*/
class StreamReader
	: public TextReader
{
public:

	/**
		@brief		指定したストリーム用の StreamReader を初期化します。
		@param[in]	stream		: 書込み先ストリーム
		@param[in]	encoding	: 書き込むテキストのエンコーディング
		@details	encoding が NULL の場合は文字列の書き込み時に文字コードのデコードを行いません。
	*/
	StreamReader(Stream* stream, Text::Encoding* encoding = NULL);
	// TODO: UTF BOM 自動判別

	~StreamReader();
	
public:
	virtual int Peek();
	virtual int Read();
	virtual bool ReadLine(String* line);
	virtual String ReadToEnd();
	virtual bool IsEOF();

private:
	int ReadBuffer();

	static const int BufferSize = 1024;
	Stream*					m_stream;
	Text::EncodingConverter	m_converter;
	ByteBuffer				m_byteBuffer;
	int						m_byteLen;
	int						m_charElementLen;
	int						m_charPos;			///< 次に読むべき文字がある文字列バッファの位置。有効最大は m_charElementLen - 1
};

} // namespace Lumino
