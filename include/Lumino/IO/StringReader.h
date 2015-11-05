
#pragma once

#include "../Base/String.h"
#include "TextReader.h"

LN_NAMESPACE_BEGIN

/**
	@brief		�����񂩂�ǂݎ�� TextReader ���������܂��B

	@section	�t�@�C�����J���A1�s���ǂݎ��
	~~~~~~~~~~~~~~~{.cpp}
	StreamReader reader(argv[1]);
	String line;
	while (reader.ReadLine(&line))
	{
		Console.WriteLine(line);
	}
	~~~~~~~~~~~~~~~
*/
class StringReader
	: public TextReader
{
public:

	/**
		@brief		�w�肵�������񂩂�ǂݎ�� StringReader �N���X�̐V�����C���X�^���X�����������܂��B
		@param[in]	str		: �ǂݎ�茳�̕�����
	*/
	StringReader(const String& str);

	virtual ~StringReader();

public:
	virtual int Peek();
	virtual int Read();
	virtual bool ReadLine(String* line);
	virtual String ReadToEnd();
	//virtual int GetPosition() const;
	virtual bool IsEOF();

private:
	String	m_src;
	int		m_pos;
};

LN_NAMESPACE_END
