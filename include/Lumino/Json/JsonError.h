
#pragma once

#include <Lumino/Base/String.h>

namespace Lumino
{
namespace Json
{

/// JSON ��͂̃G���[�R�[�h
enum ParseError
{
	ParseError_NoError = 0,					///< �G���[�͔������Ă��Ȃ�
	ParseError_DocumentEmpty,				///< ���͕����񂪋󂾂���
	ParseError_DocumentRootNotSingular,		///< �����̃��[�g�v�f����������
	ParseError_ValueInvalid,				///< �����Ȓl

	ParseError_Termination,					///< Hander �Œ��f���ꂽ
};

/*
	@brief	JSON ��͒��̃G���[��\���܂��B
*/
class JsonError
{
public:
	void SetError(ParseError errorCode, int offset) { ErrorCode = errorCode; Offset = offset; }

public:
	ParseError	ErrorCode;
	int			Offset;
	String		Message;
};

} // namespace Json
} // namespace Lumino