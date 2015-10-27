﻿/**
	@file	StreamReader.h
*/
#pragma once
#include "../Text/Encoding.h"
#include "../Text/EncodingConverter.h"
#include "Stream.h"
#include "TextReader.h"

LN_NAMESPACE_BEGIN

/**
	@brief	特定のエンコーディングのストリームを文字列として読み込む TextReader です。
*/
class StreamReader
	: public TextReader
{
public:
	static const int DefaultBufferSize = 1024;

public:

	/**
		@brief		指定したストリーム用の StreamReader を初期化します。
		@param[in]	stream		: 読み込み元ストリーム
		@param[in]	encoding	: 読み込むテキストのエンコーディング
		@details	encoding が NULL の場合は UTF8Encoding を使用します。
	*/
	StreamReader(Stream* stream, Encoding* encoding = NULL);
	// TODO: UTF BOM 自動判別

	/**
		@brief		指定したパスのファイル用の StreamReader を初期化します。
		@param[in]	filePath	: 読み込み元ファイルのパス
		@param[in]	encoding	: 読み込むテキストのエンコーディング
		@details	encoding が NULL の場合は UTF8Encoding を使用します。
	*/
	StreamReader(const TCHAR* filePath, Encoding* encoding = NULL);

	~StreamReader();
	
public:
	virtual int Peek();
	virtual int Read();
	virtual bool ReadLine(String* line);
	virtual String ReadToEnd();
	virtual bool IsEOF();

private:
	void InitReader(Stream* stream, Encoding* encoding);
	int ReadBuffer();

	RefPtr<Stream>			m_stream;
	EncodingConverter		m_converter;
	ByteBuffer				m_byteBuffer;
	int						m_byteLen;
	int						m_charElementLen;
	int						m_charPos;			///< 次に読むべき文字がある文字列バッファの位置。有効最大は m_charElementLen - 1
};

LN_NAMESPACE_END
