
#include "../Internal.h"
#include "../UnitFile.h"
#include "Lexer.h"

LN_NAMESPACE_BEGIN
namespace parser
{
	
static int g_alphaNumTypeTable[256] =
{
	0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
	0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
	0x0001, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
	0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0004, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
	0x0002, 0x0108, 0x0108, 0x0108, 0x0108, 0x0108, 0x0108, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
	0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0002, 0x0002, 0x0002, 0x0002, 0x0002,
	0x0002, 0x0108, 0x0108, 0x0108, 0x0108, 0x0108, 0x0108, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
	0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0002, 0x0002, 0x0002, 0x0002, 0x0001,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

//=============================================================================
// Lexer
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ResultState Lexer::Tokenize(UnitFile* file, DiagnosticsItemSet* diag)
{
	ByteBuffer buffer = FileSystem::ReadAllBytes(file->GetAbsolutePath());
	file->m_tokenList = Tokenize(buffer, diag);
	return ResultState::Success;	// TODO
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TokenListPtr Lexer::Tokenize(const char* str, DiagnosticsItemSet* diag)
{
	ByteBuffer buf(str, strlen(str));
	return Tokenize(buf, diag);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TokenListPtr Lexer::Tokenize(const ByteBuffer& buffer, DiagnosticsItemSet* diag)
{
	m_diag = diag;
	m_currentLineNumber = 0;
	m_currentColumn = 0;

	TokenListPtr list(LN_NEW TokenList(), false);
	m_tokenBuffer = RefPtr<TokenBuffer>::MakeRef();	// TODO: TokenList �Ɏ������������g�����肢��

	// �ň��̃p�^�[���ŗe�ʊm��
	list->Reserve(buffer.GetSize());

	OnStart();

	Range r;
	r.pos = (const TokenChar*)buffer.GetConstData();
	r.end = r.pos + (buffer.GetSize() / sizeof(TokenChar));
	while (r.pos < r.end)
	{
		int len = ReadToken(r, list);
		if (len == 0) {
			break;
		}
		PollingToken(list->GetLast());

		r.pos += len;
		m_currentColumn += len;

		// ���s������������s�ԍ��𑝂₷
		if (list->GetAt(list->GetCount() - 1).GetCommonType() == CommonTokenType::NewLine)
		{
			++m_currentLineNumber;
			m_currentColumn = 0;
		}
	}

	// �Ō�� EOF �����Ă���
	list->Add(m_tokenBuffer->CreateToken(CommonTokenType::Eof, nullptr, nullptr));

	return list;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Lexer::PollingToken(const Token& newToken)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
AlphaNumTypeFlags Lexer::GetAlphaNumType(TokenChar ch)
{
	uint8_t uc = (uint8_t)ch;
	if (uc > 255) {
		return AlphaNumTypeFlags::MBC;	// wchar_t �^�̑��o�C�g�R�[�h
	}
	return (AlphaNumTypeFlags::enum_type)g_alphaNumTypeTable[uc];
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Lexer::ReadNewLine(const Range& buffer, ReadResult* outResult)
{
	int len = IsNewLine(buffer);
	if (len > 0) {
		outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::NewLine, buffer.pos, buffer.pos + len));
		return len;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Lexer::IsNewLine(const Range& buffer)
{
	if (buffer.pos[0] == '\r' &&
		buffer.pos[1] == '\n')
	{
		return 2;
	}
	if (buffer.pos[0] == '\r' ||
		buffer.pos[0] == '\n')
	{
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int Lexer::ReadMBSSequence(const Range& buffer, ReadResult* outResult)
{
	Range r = buffer;
	while (r.pos < r.end)
	{
		int extraCount = 0;
		UnicodeUtils::CheckUTF8TrailingBytes((const UnicodeUtils::UTF8*)r.pos, (const UnicodeUtils::UTF8*)r.end, false, &extraCount);

		// �ǉ��o�C�g������ = �V���O���o�C�g�����������ꍇ�͏I��
		if (extraCount == 0) {
			break;
		}

		// ���񒲂ׂ������ƁA�ǉ��������i�߂�
		r.pos += (1 + extraCount);
	};

	int len = r.pos - buffer.pos;
	if (len > 0) {
		outResult->Set(m_tokenBuffer->CreateToken(CommonTokenType::MbsSequence, buffer.pos, r.pos));
	}
	return len;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Lexer::OnStart()
{
}

} // namespace Parser
LN_NAMESPACE_END

