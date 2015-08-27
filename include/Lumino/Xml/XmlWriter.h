
#pragma once
#include "../IO/TextWriter.h"
#include "XmlError.h"

namespace Lumino
{

/**
	@brief		SAX �X�^�C���� XML ���C�^�[�ł��B
	@details	�uExtensible Markup Language (XML) 1.1 (����)�v���x�[�X�Ɏ�������Ă��܂��B
				http://www.eplusx.net/translation/W3C/REC-xml11-20060816/#NT-elementdecl
*/
class XmlWriter
{
public:
	XmlWriter(TextWriter* textWriter);
	~XmlWriter();

public:
	/** XML �錾���������݂܂��B*/
	void WriteStartDocument();

	/** XML �錾����A�I�[�v�f���������݂܂��B*/
	void WriteEndDocument();

private:
	RefPtr<TextWriter>		m_textWriter;
};

} // namespace Lumino
