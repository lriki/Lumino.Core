
#pragma once
//#include "../Base/EnumExtension.h"
//#include "../Base/String.h"
//
//LN_NAMESPACE_BEGIN
//
///** JSON ��͂̃G���[�R�[�h */
//LN_ENUM(JsonParseError)
//{
//	NoError = 0,						///< �G���[�͔������Ă��Ȃ�
//	DocumentEmpty,						///< ���͕����񂪋󂾂���
//	DocumentRootNotSingular,			///< �����̃��[�g�v�f����������
//	ValueInvalid,						///< �����Ȓl
//	StringEscapeInvalid,				///< �����ȃG�X�P�[�v�V�[�P���X
//	StringMissQuotationMark,			///< ������̊J�n�ƏI�[����v���Ȃ����� (" �����Ȃ�)
//	StringInvalidEncoding,				///< �ϊ��ł��Ȃ����������݂���
//	ArrayMissCommaOrSquareBracket,		///< �z��̗v�f�̌�� , �� ] ����������
//	ObjectMissKeyStart,					///< �I�u�W�F�N�g�����o�̖��O�̊J�n " ��������Ȃ�����
//	ObjectMissColon,					///< �I�u�W�F�N�g�����o�� : ��������Ȃ�����
//	ObjectMissCommaOrCurlyBracket,		///< �I�u�W�F�N�g�����o�̌�� , �� } ��������Ȃ�����
//
//	NumberInvalid,						///< �����Ȑ��l
//	NumberOverflow,						///< ���l�̕ϊ��ŃI�[�o�[�t���[����������
//
//	Termination,						///< Hander �Œ��f���ꂽ
//};
//LN_ENUM_DECLARE(JsonParseError);
//
///**
//	@brief	JSON ��͒��̃G���[��\���܂��B
//*/
//class JsonError
//{
//public:
//	JsonError()
//		: ErrorCode(JsonParseError::NoError)
//		, Offset(0)
//		, Message()
//	{}
//
//public:
//	void SetError(JsonParseError errorCode, int offset) { ErrorCode = errorCode; Offset = offset; }
//
//public:
//	JsonParseError	ErrorCode;
//	int				Offset;
//	String			Message;
//};
//
//LN_NAMESPACE_END