
#pragma once

#include "../Base/String.h"
#include "TextReader.h"

namespace Lumino
{

/**
	@brief		文字列から読み取る TextReader を実装します。
*/
class StringReader
	: public TextReader
{
public:

	/*
		@brief		指定した文字列から読み取る StringReader クラスの新しいインスタンスを初期化します。
		@param[in]	str		: 読み取り元の文字列
	*/
	StringReader(const String& str);

	virtual ~StringReader();

public:
	virtual int Peek() const;
	virtual int Read();
	virtual bool ReadLine(String* line);
	virtual String ReadToEnd();
	virtual int GetPosition() const;
	virtual bool IsEOF() const;

private:
	String	m_src;
	int		m_pos;
};

} // namespace Lumino
