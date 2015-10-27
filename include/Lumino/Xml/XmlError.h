
#pragma once

LN_NAMESPACE_BEGIN

enum XmlErrorCode
{
	ParseError_NoError = 0,						///< エラーは発生していない

	ParseError_CommentDoubleHyphen,		///< コメント内に隣接する -- が見つかった

	ParseError_ElementNameNotFount,			///< 要素名が見つからなかった
	ParseError_ElementInvalidEmptyTagEnd,		///< 単一要素の終端が /> ではない

	ParseError_AttributeEqualNotFount,	///< 属性の = が見つからなかった
	ParseError_AttributeQuoteNotFount,	///< 属性の値が引用符で囲まれていない

	ParseError_InvalidEOF,		///< 予期せぬ EOF
};

class XmlError
{
public:
	XmlError() {}
	~XmlError() {}

public:
	void AddError(XmlErrorCode errorCode, int line, int col) {}	// TODO:
	
};

LN_NAMESPACE_END
