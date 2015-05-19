
#pragma once

#include "../Base/String.h"
#include "TextReader.h"

namespace Lumino
{

/**
	@brief		�����񂩂�ǂݎ�� TextReader ���������܂��B
*/
class StringReader
	: public TextReader
{
public:

	/*
		@brief		�w�肵�������񂩂�ǂݎ�� StringReader �N���X�̐V�����C���X�^���X�����������܂��B
		@param[in]	str		: �ǂݎ�茳�̕�����
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
