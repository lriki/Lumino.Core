
#pragma once
#include "Common.h"
//#include <memory>
//#include "../../../external/Lumino.Core/include/Lumino/Base/ByteBuffer.h"
//#include "../../../external/Lumino.Core/include/Lumino/Base/Array.h"
//#include "../../../external/Lumino.Core/include/Lumino/Base/String.h"
//#include "../../../external/Lumino.Core/include/Lumino/Base/StringTraits.h"

LN_NAMESPACE_BEGIN
namespace parser
{
	
/**
	@brief	
*/
class Token
{
public:
	static const Token EofToken;


private:
	Token(CommonTokenType commonType/*, const TokenChar* begin, const TokenChar* end*/)
	{
		m_commonType = commonType;
	}
	//Token(CommonTokenType commonType, const TokenChar* begin, const TokenChar* end, int langTokenType)
	//	: Token(commonType, begin, end)
	//{
	//	m_langTokenType = langTokenType;
	//}

public:
	Token() = default;
	Token(const Token& src) = default;
	Token& operator = (const Token& src) = default;
	~Token() = default;

	CommonTokenType GetCommonType() const { return m_commonType; }
	int GetLangTokenType() const { return m_langTokenType; }
	const TokenChar* GetBegin() const;	// TODO: inline �ɂ�����
	const TokenChar* GetEnd() const;
	int GetLength() const { return m_locEnd - m_locBegin; }

	bool IsSpaceOrComment() const { return m_commonType == CommonTokenType::SpaceSequence || m_commonType == CommonTokenType::Comment; }
	bool IsEof() const { return m_commonType == CommonTokenType::Eof; }

	// �����񂪈�v���邩
	bool EqualString(const TokenChar* str, int len) const
	{
		if (GetLength() != len) return false;
		return StringTraits::StrNCmp(GetBegin(), str, len) == 0;	// TODO: Case
	}

	// ��������v���邩
	bool EqualChar(TokenChar ch) const
	{
		if (GetLength() != 1) return false;
		return *GetBegin() == ch;	// TODO: Case
	}

	GenericString<TokenChar> ToString() const { return GenericString<TokenChar>(GetBegin(), GetLength()); }

	TokenStringRef GetStringRef() const { return TokenStringRef(GetBegin(), GetLength()); }

	// �g�[�N�����}�N���̏ꍇ�A���̃}�N���̎��̂ւ̎Q��
	// ���ۂɎg���Ƃ��Ă��A�}�N����`���l�X�g���Ă���ꍇ�� NULL�B
	// �}�N����W�J�����Ƃ��A���̒��ɂ���}�N���̒u���͒��߂̒�`�ɏ]���B�E�E�E���Č����ƁA1������������@�ɓ��ꂵ���ق����ǂ����E�E�E�H
	void SetMacroEntity(MacroDefine* macro) { m_macroEntity = macro; }
	MacroDefine* GetMacroEntity() const { return m_macroEntity; }

	void SetValid(bool valid) { m_valid = valid; }
	bool IsValid() const { return m_valid; }

	void SetFirstLineNumber(int lineNumber) { m_firstLineNumber = lineNumber; }
	void SetFirstColumn(int column) { m_firstColumn = column; }
	void SetLastLineNumber(int lineNumber) { m_lastLineNumber = lineNumber; }
	void SetLastColumn(int column) { m_lastColumn = column; }

	int GetFirstLineNumber() const { return m_firstLineNumber; }
	int GetFirstColumn() const { return m_firstColumn; }
	int GetLastLineNumber() const { return m_lastLineNumber; }
	int GetLastColumn() const { return m_lastColumn; }

private:
	friend class TokenBuffer;
	CommonTokenType		m_commonType = CommonTokenType::Unknown;
	int					m_langTokenType = 0;
	int					m_firstLineNumber = -1;		// 0�X�^�[�g
	int					m_firstColumn = -1;			// 0�X�^�[�g
	int					m_lastLineNumber = -1;		// 0�X�^�[�g
	int					m_lastColumn = -1;			// 0�X�^�[�g


	TokenBuffer*		m_ownerBuffer = nullptr;
	DataLocation		m_locBegin = 0;				// m_ownerBuffer ���̕�����̂���擪�C���f�b�N�X
	DataLocation		m_locEnd = 0;				// m_ownerBuffer ���̕�����̂���I�[�C���f�b�N�X

	MacroDefine*		m_macroEntity = nullptr;

	bool				m_valid = true;
};

#if 0
// ��{�I�ȃg�[�N����� (����ԋ���)
enum TokenType
{
	TokenType_Unknown = 0,
	TokenType_SpaceSequence,
	TokenType_NewLine,
	TokenType_Identifier,
	TokenType_Keyword,
	TokenType_NumericLiteral,

	/*
	 *	C/C++ �́ustring-literal:�v�ɂ�����AL �̂悤�ȃv���t�B�b�N�X���܂ޓ_�ɒ��ӁB
	 *	���@��A�v���t�B�b�N�X�ƃN�H�[�e�[�V�����̊Ԃɋ󔒂��܂߂邱�Ƃ͂ł��Ȃ��B
	 *	C# �� @ ���܂܂��B
	 */
	 TokenType_StringLiteral,
	 TokenType_CharLiteral,			///< RPNParser �Ő��l�Ƃ��ĕ]���ł��邽�� StringLiteral �Ƃ͕����Ă���
	//TokenType_CharOrStringLiteral,

	TokenType_Operator,
	TokenType_Comment,				///< �s��\ ������ꍇ�͊܂߂�
	TokenType_EscNewLine,			///< �s�� \ (TokenType_NewLine �Ƃ͋�ʂ���B"\n" = NewLine, "\\\n" = EscNewLine)
	TokenType_MBSSequence,			///< �}���`�o�C�g��������
	TokenType_EOF,					///< EOF
};

template<typename TChar>
class Token
{
public:
	typename typedef GenericString<TChar> StringT;

public:
	Token()
	{
		Init();
	}

	Token(TokenType type, const TChar* pBegin, const TChar* pEnd)
	{
		Init();
		m_type = type;
		m_begin = pBegin;
		m_end = pEnd;
	}

	Token(TokenType type, int lnagTokenType, const TChar* pBegin, const TChar* pEnd)
		: m_type(type)
		, m_langTokenType(lnagTokenType)
		, m_begin(pBegin)
		, m_end(pEnd)
	{
		Init();
		m_type = type;
		m_langTokenType = lnagTokenType;
		m_begin = pBegin;
		m_end = pEnd;
	}

	~Token()
	{
		//LN_SAFE_DELETE_ARRAY(m_dynamicBuffer);
	}

	void Init()
	{
		m_type = TokenType_Unknown;
		m_langTokenType = 0;
		m_begin = NULL;
		m_end = NULL;
		m_beginStringValue = NULL;
		m_endStringValue = NULL;
		m_staticBuffer[0] = 0x00;
		m_dynamicBuffer = NULL;
	}


	Token(const Token& src) { Copy(src); }
	Token& operator = (const Token& src) { Copy(src); return *this; }


	void Copy(const Token& src)
	{
		m_type				= src.m_type;
		m_langTokenType		= src.m_langTokenType;
		m_begin				= src.m_begin;
		m_end				= src.m_end;
		m_beginStringValue	= src.m_beginStringValue;
		m_endStringValue	= src.m_endStringValue;
		m_dynamicBuffer		= src.m_dynamicBuffer;

		// src �� static �o�b�t�@���g���Ă���ꍇ�̓R�s�[������ł�����̃|�C���^���t���ւ���
		if (src.m_begin == src.m_staticBuffer)
		{
			memcpy_s(m_staticBuffer, sizeof(m_staticBuffer), src.m_staticBuffer, sizeof(m_staticBuffer));
			m_begin = m_staticBuffer;
			m_end = m_begin + src.GetLength();
			m_beginStringValue = m_begin + (src.m_beginStringValue - src.m_begin);
			m_endStringValue = m_end - (src.m_end - src.m_endStringValue);
		}
	}

public:
	TokenType		GetTokenType() const { return m_type; }
	int				GetLangTokenType() const { return m_langTokenType; }
	const TChar*	GetTokenBegin() const { return m_begin; }
	const TChar*	GetTokenEnd() const { return m_end; }
	int				GetLength() const { return m_end - m_begin; }

	// ��������Ɩ�肠��B�v���v���Z�b�T�ł� \n �͋󔒈������Ă̓_��
	bool IsGenericSpace() const
	{
		return (
			m_type == TokenType_SpaceSequence ||
			m_type == TokenType_Comment ||
			m_type == TokenType_EscNewLine);
		/* TokenType_EscNewLine ���󔒂ł���B
		* # \
		* include "hoge.h"
		* �̓R���p�C���\�B
		*/
	}

	bool IsGenericToken() const
	{
		return !IsGenericSpace();
	}

	bool IsEOF() const
	{
		return m_type == TokenType_EOF;
	}

	bool IsLineEnd() const
	{
		return (
			m_type == TokenType_NewLine ||
			m_type == TokenType_EOF);
	}

	// 1�����Ɉ�v���邩�B�g�[�N�����̂̒�����1�����ł���B
	bool EqualChar(TChar ch) const
	{
		if (m_begin[0] != ch) return false;
		if (GetLength() != 1) return false;
		return true;
	}

	// �����񂪈�v���邩 (strncmp �̂悤�ȕ�����v�ł͂Ȃ��B�������Ⴆ�΂��̎��_�� false)
	bool EqualString(const TChar* str, int len) const
	{
		if (GetLength() != len) return false;
		return StringUtils::StrNCmp(m_begin, str, len) == 0;
	}

	void SetStringValue(const TChar* beginStringValue, const TChar* endStringValue)
	{
		m_beginStringValue = beginStringValue;
		m_endStringValue = endStringValue;
	}

	StringT GetStringValue() const { return StringT(m_beginStringValue, m_endStringValue - m_beginStringValue); }

	void CloneTokenStrings()
	{
		if (m_begin == m_staticBuffer || m_dynamicBuffer != NULL) { return; }	// ���ł� Clone �ς�

		TChar* newBegin;
		size_t len = GetLength();
		// 32 �����Ɏ��܂�g�[�N�������ł���� static �ɃR�s�[���A������Q�Ƃ���
		if (len <= StaticStringLength)
		{
			memcpy_s(m_staticBuffer, sizeof(m_staticBuffer), m_begin, len);
			newBegin = m_staticBuffer;
		}
		// ���܂�Ȃ���� new
		else
		{
			if (m_dynamicBuffer == NULL) {
				m_dynamicBuffer = std::tr1::shared_ptr<TChar>(new TChar[len]);
				memcpy_s(m_dynamicBuffer.get(), sizeof(TChar) * len, m_begin, sizeof(TChar) * len);
			}
			newBegin = m_dynamicBuffer.get();
		}

		// �g�[�N���Q�Ɣ͈͂��Čv�Z����
		m_beginStringValue = newBegin + (m_beginStringValue - m_begin);
		m_endStringValue = (newBegin + len) - (m_end - m_endStringValue);
		m_begin = newBegin;
		m_end = newBegin + len;
	}

private:
	TokenType		m_type;
	int				m_langTokenType;
	const TChar*	m_begin;
	const TChar*	m_end;
	const TChar*	m_beginStringValue;
	const TChar*	m_endStringValue;


	static const int StaticStringLength = 32;
	TChar	m_staticBuffer[StaticStringLength];		// �I�[ \0 �ł͂Ȃ�
	//TChar*	m_dynamicBuffer;
	std::tr1::shared_ptr<TChar> m_dynamicBuffer;

	/*
		���ɂ��ŏI�I�ɕK�v�ɂȂ肻���ȏ��́c
		�E�Ӗ���̓��x���ł̎��(�N���X���H�֐����H�}�N���H)
		�E�錾���t�@�C�����ƍs�ԍ�
		�E���S�C����
		�E�����̈�̃g�[�N���ł��邩
		�c�Ǝv�������ǁA�Ӗ���̓��x���ł͑��� Varable �N���X�Ƃ�����āA���̃����o�Ƃ��� Token ����������������Ǝv���B
		��������ƕK�v�ɂȂ�͖̂����g�[�N�����ǂ������炢�H
		�E�}�N���ɂ���ēW�J���ꂽ�g�[�N���ł��邩��������A��ނ��߂ɕK�p
		�E�e�̈Ӗ��I�u�W�F�N�g

		�Ȃ��A���`���s���ꍇ�͊��S�Ɍ��ɖ߂��͕̂s�\�B
		�������Ԍ��ꂩ��̃f�R���p�C���ƍl��������������������Ȃ��B
		�����A�v���v�����͎c���Ă��������C������c�B
	*/


};
#endif

} // namespace Parser
LN_NAMESPACE_END

