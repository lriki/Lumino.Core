
#pragma once

namespace Lumino
{

enum XmlErrorCode
{
	ParseError_NoError = 0,						///< �G���[�͔������Ă��Ȃ�

	ParseError_CommentDoubleHyphen,		///< �R�����g���ɗאڂ��� -- ����������

	ParseError_ElementNameNotFount,			///< �v�f����������Ȃ�����
	ParseError_ElementInvalidEmptyTagEnd,		///< �P��v�f�̏I�[�� /> �ł͂Ȃ�

	ParseError_AttributeEqualNotFount,	///< ������ = ��������Ȃ�����
	ParseError_AttributeQuoteNotFount,	///< �����̒l�����p���ň͂܂�Ă��Ȃ�

	ParseError_InvalidEOF,		///< �\������ EOF
};

class XmlError
{
public:
	XmlError() {}
	~XmlError() {}

public:
	void AddError(XmlErrorCode errorCode, int line, int col) {}	// TODO:
	
};

} // namespace Lumino
