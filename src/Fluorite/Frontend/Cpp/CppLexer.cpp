
#include "../../Internal.h"
#include "../../DiagnosticsManager.h"
#include "CppLexer.h"

LN_NAMESPACE_BEGIN
namespace parser
{

//=============================================================================
// CppLexer
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CppLexer::CppLexer()
	: m_seqPPDirective(PPDirectiveSeq::LineHead)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CppLexer::~CppLexer()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadToken(const Range& buffer, TokenList* list)
{
	int len;
	ReadResult result;

	// �󔒕���
	len = ReadSpaceSequence(buffer, &result);
	if (len > 0) { list->Add(result.token); return len; }
	// #include �̃w�b�_��
	if (m_seqPPDirective == PPDirectiveSeq::ReadingPPHeaderName)
	{
		len = ReadPPHeaderName(buffer, &result);
		if (len > 0) { list->Add(result.token); return len; }
	}
	// �v���v���Z�b�T�g�[�N����
	else if (m_seqPPDirective == PPDirectiveSeq::ReadingPPTokens)
	{
		len = ReadPPTokens(buffer, &result);
		if (len > 0) { list->Add(result.token); return len; }
	}
	// ���s
	len = ReadNewLine(buffer, &result);
	if (len > 0) { list->Add(result.token); return len; }
	// �\���
	len = ReadKeyword(buffer, &result);
	if (len > 0) { list->Add(result.token); return len; }
	// �������e����
	len = ReadCharLiteral(buffer, &result);
	if (len > 0) { list->Add(result.token); return len; }
	// �����񃊃e����
	len = ReadStringLiteral(buffer, &result);
	if (len > 0) { list->Add(result.token); return len; }
	// ���ʎq
	len = ReadIdentifier(buffer, &result);
	if (len > 0) { list->Add(result.token); return len; }
	// ���l���e����
	len = ReadNumericLiteral(buffer, &result);
	if (len > 0) { list->Add(result.token); return len; }
	// �u���b�N�R�����g
	len = ReadBlockComment(buffer, &result);
	if (len > 0) { list->Add(result.token); return len; }
	// �s�R�����g (���Z�q����Ɍ��Ă���)
	len = ReadLineComment(buffer, &result);
	if (len > 0) { list->Add(result.token); return len; }
	// ���Z�q
	len = ReadOperator(buffer, &result);
	if (len > 0) { list->Add(result.token); return len; }
	// �s���G�X�P�[�v
	len = ReadEscapeNewLine(buffer, &result);
	if (len > 0) { list->Add(result.token); return len; }
	// �}���`�o�C�g��������
	len = ReadMBSSequence(buffer, &result);
	if (len > 0) { list->Add(result.token); return len; }
	

	/* �����񃊃e�����͎��ʎq�̑O�ɉ�͂���B����́AL ���̃v���t�B�b�N�X�Ή��̂��߁B
	* �Ȃ��ACheckHexLiteralStart() �� 0x ���v���t�B�b�N�X�Ƃ��ē��ʈ������Ă��邪�A
	* ������ł͂��̂悤�� L ����ʈ����͂��Ȃ��B
	* ����� C++11 �� raw string �Ή�������ɓ��ꂽ���́B
	*
	* raw string �͎��̂悤�ɏ������Ƃ��ł���B
	*		str = R"**(AAA )" BBB)**";
	* ���̃��e�����ł́A
	* �E�J�n�g�[�N��	�c R"**(
	* �E������			�c AAA )" BBB
	* �E�I�[�g�[�N��	�c )**"
	* �ł���B
	*
	* �v���t�B�b�N�X�� L R U ���l�X����A��������ʈ������ăp�[�X���Ă��A
	* ���� " ���ɂ́u���̎�ނł��邩�v�� CheckStringStart() �ɓn���Ȃ���΂Ȃ�Ȃ��B
	* �v���t�B�b�N�X�� " �̊Ԃɋ󔒂��\�ł���΂܂������A�킴�킴���ʈ������Ċ֐��𕪂���Ƌt�ɕ��G�ɂȂ��Ă��܂��B
	*/
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CppLexer::PollingToken(const Token& token)
{
	// �������Ă��Ȃ��B���s��T���B
	if (m_seqPPDirective == PPDirectiveSeq::Idle)
	{
		if (token.GetCommonType() == CommonTokenType::NewLine)
		{
			m_seqPPDirective = PPDirectiveSeq::LineHead;		// ���s�����������B�s����Ԃ�
		}
	}
	// �s���ɂ���B# ��T���B
	else if (m_seqPPDirective == PPDirectiveSeq::LineHead)
	{
		if (token.GetCommonType() == CommonTokenType::Operator &&
			token.GetLangTokenType() == TT_CppOP_Sharp)
		{
			m_seqPPDirective = PPDirectiveSeq::FoundSharp;	// "#" ��������
		}
		else {
			m_seqPPDirective = PPDirectiveSeq::Idle;		// "#" �ȊO�̃g�[�N���������BIdle �ցB
		}
	}
	// # �܂Ō����Ă���B���� "include" ��T���B
	else if (m_seqPPDirective == PPDirectiveSeq::FoundSharp)
	{
		if (token.EqualString(_T("if"), 2) ||
			token.EqualString(_T("ifdef"), 5) ||
			token.EqualString(_T("ifndef"), 6) ||
			token.EqualString(_T("elif"), 4) ||
			token.EqualString(_T("else"), 4) ||
			token.EqualString(_T("define"), 6) ||
			token.EqualString(_T("undef"), 5))
		{
			m_seqPPDirective = PPDirectiveSeq::Idle;	// �ȍ~�App-tokens �Ƃ��ĉ�͂����A���ʂ̃g�[�N���������܂킷
			//m_seqPPDirective = PPDirectiveSeq::FoundInclude;	// "include" ��������
		}
		else if (token.EqualString(_T("include"), 7))
		{
			m_seqPPDirective = PPDirectiveSeq::ReadingPPHeaderName;	// "include" ���������̂� HeaderName �̉�͂�
		}
		else
		{
			m_seqPPDirective = PPDirectiveSeq::ReadingPPTokens;	// �ȍ~�A���s�܂ł� pp-tokens �Ƃ��ēǂݎ��
			//m_seqPPDirective = PPDirectiveSeq::Idle;		// #" �ȊO�̃g�[�N���������B"include" �ȊO�̃v���v���f�B���N�e�B�u�B
		}
	}
	// �` �s��
	else if (m_seqPPDirective == PPDirectiveSeq::ReadingPPHeaderName ||
			 m_seqPPDirective == PPDirectiveSeq::ReadingPPTokens)
	{
		if (token.GetCommonType() == CommonTokenType::NewLine)
		{
			m_seqPPDirective = PPDirectiveSeq::LineHead;		// ���s�����������B�s����Ԃ�
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CppLexer::OnStart()
{
	Lexer::OnStart();

	// ������Ԃ͉��s���㈵���Ƃ���B�t�@�C���擪�� # �f�B���N�e�B�u�����邱�Ƃɔ�����B
	m_seqPPDirective = PPDirectiveSeq::LineHead;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsSpaceChar(const Range& r)
{
	if (r.pos[0] == ' ' || r.pos[0] == '\t' || r.pos[0] == '\f' || r.pos[0] == '\v')
	{
		return 1;
	}

	//if (isspace(*buffer))
	//	return 1;
	//if (C# �̎��� UTF-8�S�p�X�y�[�X�Ƃ�)
	//	return 3;
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadSpaceSequence(const Range& buffer, ReadResult* outResult)
{
	// �A������X�y�[�X�����̐���Ԃ�
	// (�S�p�X�y�[�X�����e����ꍇ�͂���S�̂̕��������J�E���g)
	Range r = buffer;
	while (r.pos < r.end)
	{
		int len = IsSpaceChar(r);
		if (len == 0) {
			break;
		}
		r.pos += len;
	}

	// �g�[�N���쐬
	if (buffer.pos < r.pos) {
		outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::SpaceSequence, buffer.pos, r.pos));
	}
	return r.pos - buffer.pos;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsKeyword(const Range& buffer, int* langTokenType)
{
	struct WordData
	{
		const TokenChar*	word;
		int					length;
		int					type;
	};
	static const WordData wordList[] =
	{
#define TChar TokenChar	// �ȉ��A�Â��R�[�h�̃e�[�u�����g���܂킵�Ă���̂ł��̃P�A
		{ LN_T(TChar, "asm"),					3,	TT_CppKW_asm },
		{ LN_T(TChar, "auto"),					4,	TT_CppKW_auto },
		{ LN_T(TChar, "bool"),					4,	TT_CppKW_bool },
		{ LN_T(TChar, "break"),					5,	TT_CppKW_break },
		{ LN_T(TChar, "case"),					4,	TT_CppKW_case },
		{ LN_T(TChar, "catch"),					5,	TT_CppKW_catch },
		{ LN_T(TChar, "char"),					4,	TT_CppKW_char },
		{ LN_T(TChar, "class"),					5,	TT_CppKW_class },
		{ LN_T(TChar, "const"),					5,	TT_CppKW_const },
		{ LN_T(TChar, "const_cast"),			10,	TT_CppKW_const_cast },
		{ LN_T(TChar, "continue"),				8,	TT_CppKW_continue },
		{ LN_T(TChar, "default"),				7,	TT_CppKW_default },
		{ LN_T(TChar, "delete"),				6,	TT_CppKW_delete },
		{ LN_T(TChar, "do"),					2,	TT_CppKW_do },
		{ LN_T(TChar, "double"),				6,	TT_CppKW_double },
		{ LN_T(TChar, "dynamic_cast"),			12,	TT_CppKW_dynamic_cast },
		{ LN_T(TChar, "else"),					4,	TT_CppKW_else },
		{ LN_T(TChar, "enum"),					4,	TT_CppKW_enum },
		{ LN_T(TChar, "explicit"),				8,	TT_CppKW_explicit },
		{ LN_T(TChar, "export"),				6,	TT_CppKW_export },
		{ LN_T(TChar, "extern"),				6,	TT_CppKW_extern },
		{ LN_T(TChar, "false"),					5,	TT_CppKW_false },
		{ LN_T(TChar, "float"),					5,	TT_CppKW_float },
		{ LN_T(TChar, "for"),					3,	TT_CppKW_for },
		{ LN_T(TChar, "friend"),				6,	TT_CppKW_friend },
		{ LN_T(TChar, "goto"),					4,	TT_CppKW_goto },
		{ LN_T(TChar, "if"),					2,	TT_CppKW_if },
		{ LN_T(TChar, "inline"),				6,	TT_CppKW_inline },
		{ LN_T(TChar, "int"),					3,	TT_CppKW_int },
		{ LN_T(TChar, "long"),					4,	TT_CppKW_long },
		{ LN_T(TChar, "mutable"),				7,	TT_CppKW_mutable },
		{ LN_T(TChar, "namespace"),				9,	TT_CppKW_namespace },
		{ LN_T(TChar, "new"),					3,	TT_CppKW_new },
		{ LN_T(TChar, "operator"),				8,	TT_CppKW_operator },
		{ LN_T(TChar, "private"),				7,	TT_CppKW_private },
		{ LN_T(TChar, "protected"),				9,	TT_CppKW_protected },
		{ LN_T(TChar, "public"),				6,	TT_CppKW_public },
		{ LN_T(TChar, "register"),				8,	TT_CppKW_register },
		{ LN_T(TChar, "reinterpret_cast"),		16,	TT_CppKW_reinterpret_cast },
		{ LN_T(TChar, "return"),				6,	TT_CppKW_return },
		{ LN_T(TChar, "short"),					5,	TT_CppKW_short },
		{ LN_T(TChar, "signed"),				6,	TT_CppKW_signed },
		{ LN_T(TChar, "sizeof"),				6,	TT_CppKW_sizeof },
		{ LN_T(TChar, "static"),				6,	TT_CppKW_static },
		{ LN_T(TChar, "static_cast"),			11,	TT_CppKW_static_cast },
		{ LN_T(TChar, "struct"),				6,	TT_CppKW_struct },
		{ LN_T(TChar, "switch"),				6,	TT_CppKW_switch },
		{ LN_T(TChar, "template"),				8,	TT_CppKW_template },
		{ LN_T(TChar, "this"),					4,	TT_CppKW_this },
		{ LN_T(TChar, "throw"),					5,	TT_CppKW_throw },
		{ LN_T(TChar, "true"),					4,	TT_CppKW_true },
		{ LN_T(TChar, "try"),					3,	TT_CppKW_try },
		{ LN_T(TChar, "typedef"),				7,	TT_CppKW_typedef },
		{ LN_T(TChar, "typeid"),				6,	TT_CppKW_typeid },
		{ LN_T(TChar, "typename"),				8,	TT_CppKW_typename },
		{ LN_T(TChar, "union"),					5,	TT_CppKW_union },
		{ LN_T(TChar, "unsigned"),				8,	TT_CppKW_unsigned },
		{ LN_T(TChar, "using"),					5,	TT_CppKW_using },
		{ LN_T(TChar, "virtual"),				7,	TT_CppKW_virtual },
		{ LN_T(TChar, "void"),					4,	TT_CppKW_void },
		{ LN_T(TChar, "volatile"),				8,	TT_CppKW_volatile },
		{ LN_T(TChar, "wchar_t"),				7,	TT_CppKW_wchar_t },
		{ LN_T(TChar, "while"),					5,	TT_CppKW_while },
	};

	const int len = LN_ARRAY_SIZE_OF(wordList);
	int keyLen = 0;
	int type = 0;
	for (int i = 0; i < len; ++i) 
	{
		if (wordList[i].word[0] == buffer.pos[0] &&		// �܂��͐擪�����𒲂ׂ�
			StringTraits::StrNCmp(wordList[i].word, buffer.pos, wordList[i].length) == 0)	// �擪����v������c��𒲂ׂ�
		{ 
			type = (int)wordList[i].type;
			keyLen = wordList[i].length;
			break;
		}
	}

	// �{���ɃL�[���[�h�����ʎq�Ƃ��Ċ������Ă���H
	if (keyLen > 0 && IsIdentifierLetter(Range(buffer.pos + keyLen, buffer.end)) == 0)
	{
		*langTokenType = type;
		return keyLen;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadKeyword(const Range& buffer, ReadResult* outResult)
{
	int lnagTokenType = 0;
	int len = IsKeyword(buffer, &lnagTokenType);
	if (len > 0) {
		outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::Keyword, buffer.pos, buffer.pos + len, lnagTokenType));
		return len;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadEnclosingTokenHelper(const Range& buffer, TokenCheckCallback startCallback, TokenCheckCallback endCallback, const TokenChar* chars, bool* outNotFoundEndToken)
{
	Range r = buffer;
	*outNotFoundEndToken = true;

	// ������̊J�n�`�F�b�N
	int startCount = startCallback(r);
	if (startCount == 0) {
		return 0;
	}
	r.pos += startCount;

	while (r.pos < r.end)
	{
		// �G�X�P�[�v�V�[�P���X�̃`�F�b�N
		int len = 0;
		if (chars != nullptr && r.pos[0] == '\\')
		{
			for (const TokenChar* c = chars; *c != '\0'; ++c)
			{
				if (r.pos[1] == *c)
				{
					len = 2;
					break;
				}
			}
		}
		if (len > 0)
		{
			r.pos += len;
			continue;	// ���̌�̉�͂ɂ͉񂳂Ȃ��Ŏ��̕�����
		}

		// �I�[�`�F�b�N
		len = endCallback(r);
		if (len > 0) {
			r.pos += len;
			*outNotFoundEndToken = false;
			break;		// �I��
		}

		r.pos++;
	}

	return r.pos - buffer.pos;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadCharLiteral(const Range& buffer, ReadResult* outResult)
{
	//const TokenChar chars[] = { '\'', '"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v', '\0' };
	const TokenChar chars[] = { '\'', '\0' };	// �������ړI�Ȃ̂� ' �����G�X�P�[�v��OK
	bool notFoundEndToken;
	int len = ReadEnclosingTokenHelper(buffer, IsCharLiteralStart, IsCharLiteralEnd, chars, &notFoundEndToken);

	if (len > 0)
	{
		int type = TT_NumericLitaralType_Char;
		if (buffer.pos[0] == 'L') {
			type = TT_NumericLitaralType_WideChar;
		}
		outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::ArithmeticLiteral, buffer.pos, buffer.pos + len, type));
	}
	return len;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsCharLiteralStart(const Range& buffer)
{
	if (buffer.pos[0] == '\'') {
		return 1;
	}
	if (buffer.pos[0] == 'L' && buffer.pos[1] == '\'') {
		return 2;	// L �v���t�B�b�N�X�t�������񃊃e����
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsCharLiteralEnd(const Range& buffer)
{
	if (buffer.pos[0] == '\'') {
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//int CppLexer::IsAnyChar(const Range& buffer, const TokenChar* chars, int len)
//{
//	for (int i = 0; i < len; ++i)
//	{
//		if (buffer.pos[0] == chars[i])
//		{
//			return 1;
//		}
//	}
//	return 0;
//}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadStringLiteral(const Range& buffer, ReadResult* outResult)
{
	bool notFoundEndToken;

	int len = 0;
	//if (m_seqPPDirective != PPDirectiveSeq::FoundInclude)
	//{
		//const TokenChar chars[] = { '\'', '"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v', '\0' };
		const TokenChar chars[] = { '"', '\0' };	// �������ړI�Ȃ̂� " �����G�X�P�[�v��OK
		len = ReadEnclosingTokenHelper(buffer, IsStringLiteralStart, IsStringLiteralEnd, chars, &notFoundEndToken);
	//}
	//else
	//{
	//	// #include ���̏ꍇ�͂�����B�G�X�P�[�v�͖���
	//	len = ReadEnclosingTokenHelper(buffer, IsStringLiteralStartInIncludeDirective, IsStringLiteralEndIncludeDirective, nullptr, &notFoundEndToken);
	//}

	if (len > 0)
	{
		int type = TT_NumericLitaralType_AsciiString;
		if (buffer.pos[0] == 'L') {
			type = TT_NumericLitaralType_WideString;
		}
		outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::StringLiteral, buffer.pos, buffer.pos + len, type));
	}
	return len;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsStringLiteralStart(const Range& buffer)
{
	if (buffer.pos[0] == '"') {
		return 1;
	}
	if (buffer.pos[0] == 'L' && buffer.pos[1] == '"') {
		return 2;	// L �v���t�B�b�N�X�t�������񃊃e����
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsStringLiteralEnd(const Range& buffer)
{
	if (buffer.pos[0] == '"') {
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadIdentifier(const Range& buffer, ReadResult* outResult)
{
	/*
		a b c d e f g h i j k l m
		n o p q r s t u v w x y z
		A B C D E F G H I J K L M
		N O P Q R S T U V W X Y Z _
		0 1 2 3 4 5 6 7 8 9
	*/
	Range r = buffer;

	// ���ʎq�̐擪�������H
	int len = IsIdentifierStart(buffer);
	if (len == 0) {
		return 0;	// ���ʎq�ł͂Ȃ�
	}
	r.pos += len;

	// ��ɑ��������͐��������H
	while (r.pos < r.end)
	{
		len = IsIdentifierLetter(r);
		if (len == 0) {
			break;
		}
		r.pos += len;
	}

	// �g�[�N���쐬
	outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::Identifier, buffer.pos, r.pos));
	return r.pos - buffer.pos;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsIdentifierStart(const Range& buffer)
{
	if (isalpha(buffer.pos[0]) || buffer.pos[0] == '_')
	{
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsIdentifierLetter(const Range& buffer)
{
	if (isalnum(buffer.pos[0]) || buffer.pos[0] == '_')
	{
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadNumericLiteral(const Range& buffer, ReadResult* outResult)
{
	/* 123	10�i�����e����
	* 012	8�i�����e����
	*
	*/

	// TODO: ���̓��e�� LangDef �Ɏ����Ă����ׂ������H

	bool	bExistPeriod = false;		/* �����_���o�ꂵ����TRUE */
	bool	bExistExp = false;		/* �w�����o�ꂵ����TRUE */
	bool	bExistSuffix = false;		/* �ڔ��ꂪ�o�ꂵ����TRUE */
	bool	bHexMode = false;		/* 16�i���ŗL��ꍇ��TRUE */

	// ���l���e�����̌^�B�T�t�B�b�N�X�Ƃ��Ȃ���΃f�t�H���g�� Int32
	int	litaralType = TT_NumericLitaralType_Int32;

	// 16�i���̐擪�`�F�b�N
	//const TokenChar* pPos = buffer.pos;
	Range r = buffer;
	int len = IsHexLiteralStart(r);
	if (len > 0) {
		r.pos += len;
		bHexMode = true;	// 16 �i���ł���
	}

	int nStep = 0;	// ��ԑJ�ڂ�\��

	bool bEnd = false;
	while (r.pos < r.end && !bEnd)
	{
		if (r.pos[0] > 255) {	// wchar_t �͈̔̓`�F�b�N
			break;
		}
		AlphaNumTypeFlags charType = GetAlphaNumType(r.pos[0]);

		// �}���`�o�C�g�R�[�h�܂��͐��䕶���܂��� . �ȊO�̉��Z�q�ł���ΏI��
		if (charType.TestFlag(AlphaNumTypeFlags::MBC) ||
			charType.TestFlag(AlphaNumTypeFlags::Control) ||
			(charType.TestFlag(AlphaNumTypeFlags::OpChar) && r.pos[0] != '.')){
			break;	// �I��
		}

		// �T�t�B�b�N�X�̃`�F�b�N (�T�t�B�b�N�X�͈�ԍŌゾ���A���낢��ȂƂ���ɏ�����B1f 1.0f .1f 1.0-e2f)
		if (charType.TestFlag(AlphaNumTypeFlags::Alphabet) /*|| charType.TestFlag(AlphaNumTypeFlags::HexAlpha)*/)
		{
			// �����^
			len = IsIntegerSuffix(r, &litaralType);
			if (len > 0) {
				r.pos += len;
				bExistSuffix = true;	// �T�t�B�b�N�X��������
				nStep = 10;
				continue;
			}
			// �����^
			if (!bHexMode)	// 16�V���� F �� float �� F ����ʂ�����
			{
				len = IsRealSuffix(r, &litaralType);
				if (len > 0) {
					r.pos += len;
					bExistSuffix = true;	// �T�t�B�b�N�X��������
					nStep = 10;
					continue;
				}
			}
			// �� 50LL �̂悤�ɏd���̉\�������邽�߁A�����������Ă��G���[�ɂ��Ȃ�
		}

		if (bExistSuffix) {
			return 0;	// �T�t�B�b�N�X�̌�ɁA�T�t�B�b�N�X�ł͂Ȃ������������B��������
		}

		switch (nStep)
		{
		case 0:		// ��������
		{
			if (bHexMode && charType.TestFlag(AlphaNumTypeFlags::HexAlpha)) {
				r.pos++;		// 16 �i���Ȃ� HexAlpha �������Čp��
			}
			else if (charType.TestFlag(AlphaNumTypeFlags::Number)) {
				r.pos++;		// ���ʂ̐��l�� OK
			}
			else if (charType.TestFlag(AlphaNumTypeFlags::OpChar) && r.pos[0] == '.') {
				nStep = 1;	// . �����������B���������ֈڍs
				r.pos++;
				litaralType = TT_NumericLitaralType_Double;	// �Ƃ肠���� Double �^�Ƃ���
			}
			else {
				len = IsExponentStart(r);
				if (len > 0)		// e E + -
				{
					nStep = 2;		// e- �Ƃ������������B�w�������ֈڍs
					r.pos += len;
					litaralType = TT_NumericLitaralType_Double;	// �w����������̂łƂ肠���� Double �^�Ƃ���
				}
				else {
					return 0;		// 10.5G ���A�ςȃT�t�B�b�N�X�Ƃ��A���̎��ʎq�Ƃ̊ԂɃX�y�[�X�������Ƃ�
				}
			}
			break;
		}
		case 1:		// ��������
		{
			if (bHexMode && charType.TestFlag(AlphaNumTypeFlags::HexAlpha)) {
				r.pos++;		// 16 �i���Ȃ� HexAlpha �������Čp��
			}
			else if (charType.TestFlag(AlphaNumTypeFlags::Number)) {
				r.pos++;		// ���ʂ̐��l�� OK
			}
			else
			{
				len = IsExponentStart(r);
				if (len > 0)		// e E + -
				{
					nStep = 2;		// e- �Ƃ������������B�w�������ֈڍs
					r.pos += len;
					//litaralType = TT_NumericLitaralType_Double;	// �w����������̂łƂ肠���� Double �^�Ƃ���
				}
				else {
					return 0;		// 10.5G ���A�ςȃT�t�B�b�N�X�Ƃ��A���̎��ʎq�Ƃ̊ԂɃX�y�[�X�������Ƃ�
				}
			}
			break;
		}
		case 2:		// �w������
		{
			if (charType.TestFlag(AlphaNumTypeFlags::Number)) {
				r.pos++;		// ���ʂ̐��l�� OK
			}
			else {
				return 0;	// ����ȊO�͎��s
			}
			break;
		}
		}
	}

	// �����܂ŗ������͐���
	outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::ArithmeticLiteral, buffer.pos, r.pos, litaralType));
	return r.pos - buffer.pos;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsHexLiteralStart(const Range& buffer)
{
	if (buffer.pos[0] == '0' && (buffer.pos[1] == 'x' || buffer.pos[1] == 'X'))
		return 2;
	return 0;
	// VB �͂����� &H
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsIntegerSuffix(const Range& buffer, int* outLiteralType)
{
	/*
		u U
		l L
		ul Ul uL UL
		ll LL
		ull Ull uLL uLL
	*/
	if (buffer.pos[0] == 'u' || buffer.pos[0] == 'U')
	{
		if (buffer.pos[1] == 'l' || buffer.pos[1] == 'L')
		{
			if (buffer.pos[2] == 'l' || buffer.pos[2] == 'L')
			{
				*outLiteralType = TT_NumericLitaralType_UInt64;
				return 3;
			}
			else
			{
				*outLiteralType = TT_NumericLitaralType_UInt32;
				return 2;
			}
		}
		else
		{
			*outLiteralType = TT_NumericLitaralType_UInt32;
			return 1;
		}
	}

	if (buffer.pos[0] == 'l' || buffer.pos[0] == 'L')
	{
		if (buffer.pos[1] == 'l' || buffer.pos[1] == 'L')
		{
			*outLiteralType = TT_NumericLitaralType_Int64;
			return 2;
		}
		else
		{
			*outLiteralType = TT_NumericLitaralType_Int32;
			return 1;
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsRealSuffix(const Range& buffer, int* outLiteralType)
{
	if (buffer.pos[0] == 'f' || buffer.pos[0] == 'F')
	{
		*outLiteralType = TT_NumericLitaralType_Float;
		return 1;
	}
	if (buffer.pos[0] == 'd' || buffer.pos[0] == 'D')
	{
		*outLiteralType = TT_NumericLitaralType_Double;
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsExponentStart(const Range& buffer)
{
	if (buffer.pos[0] == 'e' ||
		buffer.pos[0] == 'E')
	{
		if (buffer.pos[1] == '+' ||
			buffer.pos[1] == '-') {
			return 2;
		}
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadBlockComment(const Range& buffer, ReadResult* outResult)
{
	bool notFoundEndToken;
	int len = ReadEnclosingTokenHelper(buffer, IsBlockCommentStart, IsBlockCommentEnd, nullptr, &notFoundEndToken);
	if (len > 0)
	{
		if (notFoundEndToken)
		{
			m_diag->Report(DiagnosticsCode::UnexpectedEOFInBlockComment);
			return len;
		}
		outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::Comment, buffer.pos, buffer.pos + len));
	}
	return len;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsBlockCommentStart(const Range& buffer)
{
	if (buffer.pos[0] == '/' &&
		buffer.pos[1] == '*')
		return 2;
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsBlockCommentEnd(const Range& buffer)
{
	if (buffer.pos[0] == '*' &&
		buffer.pos[1] == '/')
		return 2;
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadLineComment(const Range& buffer, ReadResult* outResult)
{
	// �s�R�����g�̊J�n�`�F�b�N
	int len = IsLineCommentStart(buffer);
	if (len == 0) {
		return 0;
	}

	Range r = buffer;
	while (r.pos < r.end)
	{
		// �s�� \ �`�F�b�N
		len = IsEscapeNewLine(r);
		if (len > 0) {
			r.pos += len;
			continue;		// �p��
		}

		// �I���`�F�b�N (EOF �� end ���B�Ń��[�v������̂œ��ɋC�ɂ��Ȃ��ėǂ�)
		len = IsNewLine(r);
		if (len > 0) {
			// ���s�̓R�����g�ɂ͊܂܂Ȃ�
			break;			// �I��
		}

		++r.pos;
	}

	outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::Comment, buffer.pos, r.pos));
	return r.pos - buffer.pos;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsLineCommentStart(const Range& buffer)
{
	if (buffer.pos[0] == '/' &&
		buffer.pos[1] == '/')
		return 2;
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadOperator(const Range& buffer, ReadResult* outResult)
{
	int lnagTokenType = 0;
	int len = IsOperator(buffer, &lnagTokenType);
	if (len > 0) {
		outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::Operator, buffer.pos, buffer.pos + len, lnagTokenType));
		return len;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsOperator(const Range& buffer, int* langTokenType)
{
	if (GetAlphaNumType(buffer.pos[0]).TestFlag(AlphaNumTypeFlags::OpChar))
	{
		struct WordData
		{
			const TokenChar*	word;
			int					length;
			int					type;
		};
		// ++ �� + �̂悤�ɁA1�����ڂ��������͕̂������̑���������Ɍ��������悤�ɂ���
		static const WordData wordList[] =
		{
			{ LN_T(TChar, "##"),	2,	TT_CppOP_SharpSharp },
			{ LN_T(TChar, "#"),		1,	TT_CppOP_Sharp },
			{ LN_T(TChar, "->*"),	3,	TT_CppOP_ArrowAsterisk },
			{ LN_T(TChar, "->"),	2,	TT_CppOP_Arrow },
			{ LN_T(TChar, ","),		1,	TT_CppOP_Comma },
			{ LN_T(TChar, "++"),	2,	TT_CppOP_Increment },
			{ LN_T(TChar, "--"),	2,	TT_CppOP_Decrement },
			{ LN_T(TChar, "&&"),	2,	TT_CppOP_LogicalAnd },
			{ LN_T(TChar, "||"),	2,	TT_CppOP_LogicalOr },
			{ LN_T(TChar, "<="),	2,	TT_CppOP_LessThenEqual },
			{ LN_T(TChar, ">="),	2,	TT_CppOP_GreaterThenEqual },
			{ LN_T(TChar, "=="),	2,	TT_CppOP_CmpEqual },
			{ LN_T(TChar, "<<="),	3,	TT_CppOP_LeftShiftEqual },
			{ LN_T(TChar, ">>="),	3,	TT_CppOP_RightShiftEqual },
			{ LN_T(TChar, "+="),	2,	TT_CppOP_PlusEqual },
			{ LN_T(TChar, "-="),	2,	TT_CppOP_MinusEqual },
			{ LN_T(TChar, "*="),	2,	TT_CppOP_MulEqual },
			{ LN_T(TChar, "/="),	2,	TT_CppOP_DivEqual },
			{ LN_T(TChar, "%="),	2,	TT_CppOP_ModEqual },
			{ LN_T(TChar, "&="),	2,	TT_CppOP_AndEqual },
			{ LN_T(TChar, "|="),	2,	TT_CppOP_OrEqual },
			{ LN_T(TChar, "!="),	2,	TT_CppOP_NotEqual },
			{ LN_T(TChar, "="),		1,	TT_CppOP_Equal },
			{ LN_T(TChar, "<<"),	2,	TT_CppOP_LeftShift },
			{ LN_T(TChar, ">>"),	2,	TT_CppOP_RightShift },
			{ LN_T(TChar, "+"),		1,	TT_CppOP_Plus },
			{ LN_T(TChar, "-"),		1,	TT_CppOP_Minus },
			{ LN_T(TChar, "*"),		1,	TT_CppOP_Asterisk },
			{ LN_T(TChar, "/"),		1,	TT_CppOP_Slash },
			{ LN_T(TChar, "%"),		1,	TT_CppOP_Parseint },
			{ LN_T(TChar, "&"),		1,	TT_CppOP_Ampersand },
			{ LN_T(TChar, "|"),		1,	TT_CppOP_Pipe },
			{ LN_T(TChar, "~"),		1,	TT_CppOP_Tilde },
			{ LN_T(TChar, "^"),		1,	TT_CppOP_Caret },
			{ LN_T(TChar, "!"),		1,	TT_CppOP_Exclamation },
			{ LN_T(TChar, "..."),	3,	TT_CppOP_Ellipsis },
			{ LN_T(TChar, ".*"),	2,	TT_CppOP_DotAsterisk },
			{ LN_T(TChar, "."),		1,	TT_CppOP_Dot },
			{ LN_T(TChar, "::"),	2,	TT_CppOP_DoubleColon },
			{ LN_T(TChar, "?"),		1,	TT_CppOP_Question },
			{ LN_T(TChar, ":"),		1,	TT_CppOP_Colon },
			{ LN_T(TChar, ";"),		1,	TT_CppOP_Semicolon },
			{ LN_T(TChar, "{"),		1,	TT_CppOP_LeftBrace },
			{ LN_T(TChar, "}"),		1,	TT_CppOP_RightBrace },
			{ LN_T(TChar, "["),		1,	TT_CppOP_LeftBracket },
			{ LN_T(TChar, "]"),		1,	TT_CppOP_RightBracket },
			{ LN_T(TChar, "("),		1,	TT_CppOP_LeftParen },
			{ LN_T(TChar, ")"),		1,	TT_CppOP_RightParen },
			{ LN_T(TChar, "<"),		1,	TT_CppOP_LeftAngle },
			{ LN_T(TChar, ">"),		1,	TT_CppOP_RightAngle },
		};
		const int count = LN_ARRAY_SIZE_OF(wordList);
		for (int i = 0; i < count; ++i)
		{
			if (wordList[i].word[0] == buffer.pos[0] &&		// �܂��͐擪�����𒲂ׂ�
				StringTraits::StrNCmp(wordList[i].word, buffer.pos, wordList[i].length) == 0)	// �擪����v������c��𒲂ׂ�
			{
				*langTokenType = (int)wordList[i].type;
				return wordList[i].length;
			}
		}
		return 0;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadEscapeNewLine(const Range& buffer, ReadResult* outResult)
{
	int len = IsEscapeNewLine(buffer);
	if (len > 0) {
		outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::SpaceSequence, buffer.pos, buffer.pos + len, TT_EscapeNewLine));
		return len;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsEscapeNewLine(const Range& buffer)
{
	if (buffer.pos[0] == '\\')
	{
		if (buffer.pos[1] == '\r' &&
			buffer.pos[2] == '\n')
		{
			return 3;
		}
		if (buffer.pos[1] == '\r' ||
			buffer.pos[1] == '\n')
		{
			return 2;
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::ReadPPHeaderName(const Range& buffer, ReadResult* outResult)
{
	// �G�X�P�[�v�͖���
	bool notFoundEndToken;
	int len = ReadEnclosingTokenHelper(buffer, IsStringLiteralStartInIncludeDirective, IsStringLiteralEndIncludeDirective, nullptr, &notFoundEndToken);
	if (len > 0)
	{
		outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::StringLiteral, buffer.pos, buffer.pos + len, TT_HeaderName));
	}
	return len;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsStringLiteralStartInIncludeDirective(const Range& buffer)
{
	if (buffer.pos[0] == '"' || buffer.pos[0] == '<') {
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int CppLexer::IsStringLiteralEndIncludeDirective(const Range& buffer)
{
	if (buffer.pos[0] == '"' || buffer.pos[0] == '>') {
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// �v���v���Z�b�T�g�[�N����(1�s)�̓ǂݎ��B
// ���s�͊܂܂Ȃ��B
// #define AAA �� �u#, define, space, AAA, NewLine�v�ŏo�Ă���B
// EscapeNewLine �����l�B
//		#error AAA\
//		BBB
// �́A�u#, error, space, PPTokens, EscapeNewLine, PPTokens�v
//-----------------------------------------------------------------------------
int CppLexer::ReadPPTokens(const Range& buffer, ReadResult* outResult)
{
	Range r = buffer;
	while (r.pos < r.end)
	{
		// ���s�܂œǂ�
		int len = IsEscapeNewLine(r);
		if (len > 0) {
			break;		// �I��
		}
		len = IsNewLine(r);
		if (len > 0) {
			break;		// �I��
		}
		++r.pos;
	}

	outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::TextTokens, buffer.pos, r.pos, TT_PPTokens));
	return r.pos - buffer.pos;
}

} // namespace Parser
LN_NAMESPACE_END

