
#pragma once
#include <LuminoCore.h>

LN_NAMESPACE_BEGIN
namespace parser
{
typedef TCHAR TokenChar;
typedef GenericString<TokenChar> TokenString;
typedef GenericStringRef<TokenChar> TokenStringRef;
typedef GenericPathName<TokenChar> TokenPathName;

class Context;
class CompileOptions;
class UnitFile;
class IncludeFile;
class CompileUnitFile;
class TokenBuffer;
class TokenList;
typedef RefPtr<TokenList>	TokenListPtr;
typedef RefPtr<UnitFile>	UnitFilePtr;
typedef RefPtr<IncludeFile>	IncludeFilePtr;

class DiagnosticsItemSet;

class MacroDefine;

class Scope;
class ReferenceTracker;

typedef size_t DataLocation;

/** ASCII �R�[�h�̕������ */
LN_ENUM_FLAGS(AlphaNumTypeFlags)
{
	MBC			= 0x0000,		/**< �}���`�o�C�g�����ɔz�u�����R�[�h */
	Control		= 0x0001,		/**< ���䕶�� */
	OpChar		= 0x0002,		/**< �I�y���[�V�����R�[�h */
	Number		= 0x0004,		/**< ���� */
	Alphabet	= 0x0008,		/**< �ʏ핶�� */
	HexAlpha	= 0x0100,		/**< 16�i�����Ƃ��Ă��g���镶�� */
};
LN_ENUM_FLAGS_DECLARE(AlphaNumTypeFlags);

/** ��{�I�ȃg�[�N����� (����ԋ���) */
LN_ENUM(CommonTokenType)
{
	Unknown = 0,			/**< ���ʎ�ʂƂ��Ă͕��ނ���Ȃ� */
	SpaceSequence,			/**< �󔒕��� (C�� EscapeNewLine ���܂�) */
	NewLine,				/**< ���s ("\r" "\n" "\r\n" �̂����ꂩ�B���тł͂Ȃ��B"\r\n\r\n" ��2�� NewLine �g�[�N���ƂȂ�) */
	Identifier,				/**< ���ʎq */
	Keyword,				/**< �L�[���[�h */
	Operator,				/**< ���Z�q */
	ArithmeticLiteral,		/**< �Z�p�^���e���� (�����A�����BC����� char ���Z�p�^) */
	StringLiteral,			/**< ������ */
	Comment,				/**< �R�����g */
	MbsSequence,			/**< �}���`�o�C�g�������� */
	TextTokens,				/**< �v���v���Z�b�T�ȂǂŎg���郁�b�Z�[�W������ */
	Eof,					/**< �o�b�t�@ EOF */
};
LN_ENUM_DECLARE(CommonTokenType);


enum class ResultState
{
	Success = 0,
	Error = 1,
};

} // namespace Parser
LN_NAMESPACE_END


