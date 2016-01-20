
#pragma once
#include "../Base/EnumExtension.h"
#include "../Base/String.h"

LN_NAMESPACE_BEGIN

/** JSON ��͂̃G���[�R�[�h */
LN_ENUM(JsonParseError)
{
	NoError = 0,						///< �G���[�͔������Ă��Ȃ�
	DocumentEmpty,						///< ���͕����񂪋󂾂���
	DocumentRootNotSingular,			///< �����̃��[�g�v�f����������
	ValueInvalid,						///< �����Ȓl
	StringEscapeInvalid,				///< �����ȃG�X�P�[�v�V�[�P���X
	StringMissQuotationMark,			///< ������̊J�n�ƏI�[����v���Ȃ����� (" �����Ȃ�)
	StringInvalidEncoding,				///< �ϊ��ł��Ȃ����������݂���
	ArrayMissCommaOrSquareBracket,		///< �z��̗v�f�̌�� , �� ] ����������
	ObjectMissKeyStart,					///< �I�u�W�F�N�g�����o�̖��O�̊J�n " ��������Ȃ�����
	ObjectMissColon,					///< �I�u�W�F�N�g�����o�� : ��������Ȃ�����
	ObjectMissCommaOrCurlyBracket,		///< �I�u�W�F�N�g�����o�̌�� , �� } ��������Ȃ�����

	NumberInvalid,						///< �����Ȑ��l
	NumberOverflow,						///< ���l�̕ϊ��ŃI�[�o�[�t���[����������

	Termination,						///< Hander �Œ��f���ꂽ
};
LN_ENUM_DECLARE(JsonParseError);

/**
	@brief	JSON ��͒��̃G���[��\���܂��B
*/
class JsonError
{
public:
	JsonError()
		: ErrorCode(JsonParseError::NoError)
		, Offset(0)
		, Message()
	{}

public:
	void SetError(JsonParseError errorCode, int offset) { ErrorCode = errorCode; Offset = offset; }

public:
	JsonParseError	ErrorCode;
	int				Offset;
	String			Message;
};





/** JSON ��͂̃G���[�R�[�h */
LN_ENUM(JsonParseError2)
{
	NoError = 0,						/**< �G���[�͔������Ă��Ȃ� */

	UnterminatedString,		/**< ������̏I�[��������O�� EOF �����������B*/
	InvalidStringChar,		/**< ��������� JSON �ł͎g�p�ł��Ȃ����������������B(���䕶���Ȃ�)*/
	InvalidStringEscape,	/**< ��������ɖ����ȃG�X�P�[�v�V�[�P���X�����������B*/
	UnexpectedToken,		/**< �\�����Ȃ��g�[�N�������������B*/
	InvalidNumber,			/**< �s���Ȑ��l�`���B*/
	NumberOverflow,			/**< ���l�̕ϊ��ŃI�[�o�[�t���[�����������B*/
	InvalidObjectClosing,	/**< �I�u�W�F�N�g�������������Ă��Ȃ��B*/

	ArrayInvalidClosing,	/**< �z�񂪐����������Ă��Ȃ��B*/

	ValueInvalid,			/**< �����Ȓl */
};
LN_ENUM_DECLARE(JsonParseError2);

/**
	@brief	JSON ��͒��̃G���[��\���܂��B
*/
class JsonError2
{
public:
	JsonParseError2	code = JsonParseError2::NoError;
	int				line = 0;
	int				column = 0;
	String			message;
};

LN_NAMESPACE_END
