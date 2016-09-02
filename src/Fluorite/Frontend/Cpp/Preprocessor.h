
#pragma once
#include <unordered_map>
#include "../../Common.h"
#include "../../TokenList.h"
#include "../../TokenBuffer.h"
#include "../../IdentifierMap.h"
#include "../RpnParser.h"

LN_NAMESPACE_BEGIN
namespace parser
{

class SourceLocation
{
public:
	uint32_t	loc;
};

class SourceRange
{
public:
	SourceLocation	begin;
	SourceLocation	end;
};

// �}�N����`�̎��́B�����}�N���͂��肦��(undef ��̍Ē�`���Ă��O�͕̂ێ�����)�B���̏ꍇ�͒ʏ�A��`�s���Ⴄ�B
// 
class MacroDefine
	: public RefObject
{
public:
	TokenString		name;
	SourceRange		replacementRange;
	//TokenString		replacementContentString;	// TODO: ����Ȃ������B�u���v�f�𕶎���ŕ��ׂ����� (���S��v�̊m�F�Ŏg���B�O��̋󔒂͏����Ă���)

	bool		undef = false;

	//void AppendReplacementToTokenList(TokenList* tokenList);	// TODO: �������X�g

	MacroDefine(){}

private:
	LN_DISALLOW_COPY_AND_ASSIGN(MacroDefine);
};

class MacroMap
	: public RefObject
{
public:
	MacroMap();

	MacroDefine* Insert(const Token& name, const SourceRange& replacementRange);

	MacroDefine* Find(const TokenChar* nameBegin, const TokenChar* nameEnd = nullptr) const;
	MacroDefine* Find(const Token& name) const;

	bool IsDefined(const Token& name, MacroDefine** outDefinedMacro = nullptr) const;
	bool IsDefined(const TokenChar* name, MacroDefine** outDefinedMacro = nullptr) const;

	uint64_t GetHashCode() const;

	void Copy(const MacroMap* srcMacroMap);

	//void SetFreeze(bool enabled) { m_freeze = enabled; }
	//bool IsFreeze() const { return m_freeze; }

private:
	Array<RefPtr<MacroDefine>>			m_allMacroList;	// �ߋ��ɒ�`���ꂽ�S�Ẵ}�N��
	IdentifierMap<MacroDefine*>	m_macroMap;		// �Ē�`���ꂽ�肵�����͈̂�ԐV�����}�N�����i�[�����
	//bool						m_freeze;		// �ύX�֎~�t���O
};

// COW ���L�̎x��
//		# ��1������ include �t�@�C���̂��߂� MacroMap ��������肷��ƃ����������������Ȃ�B
//		include �t�@�C���̉�͊J�n�� MAcroMap ���R�s�[���邩�I�����ɂ��邩�Ƃ��l����ƕ��G�ɂȂ����肷��B
//		�{���ɕK�v�ȃ^�C�~���O�ŃR�s�[����邽�߂ɂ��̃N���X��p�ӂ����B
class MacroMapContainer
{
public:

	MacroMapContainer()
	{
		m_core = &m_sharedEmpty;
	}

	MacroMap* Get()
	{
		if (m_core.Get() == &m_sharedEmpty || m_core->GetReferenceCount() != 1)
		{
			auto newCore = RefPtr<MacroMap>::MakeRef();
			if (!m_core.IsNull())
			{
				newCore->Copy(m_core);
				m_core = newCore;
			}
		}
		return m_core;
	}


	const MacroMap* GetConst() const
	{
		return m_core;
	}



private:
	RefPtr<MacroMap>	m_core;
	static MacroMap		m_sharedEmpty;
};


class RawReferenceMap
{

};




// �v���v���Z�X�����t�@�C�����B
// .c �� .h ���͖��Ȃ��B
// �~�g�[�N�����X�g�͕ێ����Ȃ��̂Œ��ӁB
/*
	�g�[�N�����X�g���ێ�����BUIColors.h �Ƃ��B


	�����t�@�C���ł��A���̓}�N���}�b�v�̓��e���Ⴆ�ΈႤ�C���X�^���X�����B
*/
class PreprocessedFileCacheItem
{
public:
	PathNameA		filePath;
	MacroMap		inputMacroMap;

	MacroMap		outputMacroMap;
	RawReferenceMap	outputRawReferenceMap;

public:
	SourceRange SaveMacroTokens(const Token* begin, const Token* end);	// �L���b�V���ɕۑ�����ƍĔz�u�̉\��������̂ŁA�i�[���ꂽ�ꏊ�̓|�C���^�ł͂Ȃ��C���f�b�N�X�ŕԂ�
	void GetMacroTokens(const SourceRange& range, const Token** outBegin, const Token** outEnd) const;

private:
	Array<Token>	m_tokensCache;
};

class HeaderFileManager
{
public:

};
	
/**
	@brief
*/
class Preprocessor
{
public:
	Preprocessor();

	// �w�肷�� MacroMap �͊J�n���_�̃}�N����`���
	ResultState BuildPreprocessedTokenList(
		Context* ownerContext,
		CompileUnitFile* compileUnitFile,
		TokenList* tokenList,
		UnitFile* unitFile,
		const Array<TokenPathName>* additionalIncludePaths,
		const MacroMapContainer& parentMacroMap,
		ReferenceTracker* referenceTracker,
		DiagnosticsItemSet* diag);

private:

	ResultState PollingDirectiveLine(Token* keyword, Token* lineEnd);
	ResultState AnalyzeIfElifDirective(Token* keyword, Token* lineEnd, bool isElse);
	ResultState AnalyzeIncludeDirective(Token* keyword, Token* lineEnd);
	ResultState AnalyzePragmaDirective(Token* keyword, Token* lineEnd);

	bool IsValidSection() const;

	//TokenList::iterator GetNextGenericToken(TokenList::iterator pos);





	enum class DirectiveSec
	{
		Idle,			// �������Ă��Ȃ�
		LineHead,		// �s���ł���B�܂��͏������
		FindIdent,		// ���ʎq��T���Ă���
		FindLineEnd,	// "#" ��������
	};

	// #if �` #endif �܂ł̔�����
	enum class ConditionalSectionState
	{
		None,			// ����O�A�Z�N�V�����O
		Valid,			// �L������O���[�v��
		Invalid,		// ��������O���[�v��
		Skip,			// �L����������I����A#endif�܂�skip�\
	};

	// #if �` #endif �܂ł̏��
	struct ConditionalSection
	{
		ConditionalSectionState	state = ConditionalSectionState::None;	// #if�`#endif �܂ł̌��݂̔�����
		bool					elseProcessed = false;					// #else��t���true(#else�`#else�h�~�̈�)
	};

	Context*					m_ownerContext;
	ConstantTokenBuffer			m_constTokenBuffer;
	TokenList*					m_tokenList;
	//PreprocessedFileCacheItem*	m_fileCache;
	CompileUnitFile*			m_compileUnitFile;
	UnitFile*					m_unitFile;
	MacroMapContainer			m_macroMap;
	const Array<TokenPathName>* m_additionalIncludePaths;
	ReferenceTracker*			m_referenceTracker;
	DiagnosticsItemSet*			m_diag;

	DirectiveSec				m_seqDirective;
	Stack<ConditionalSection>	m_conditionalSectionStack;	// #if �` #endif �܂ł̏��̃X�^�b�N
	Token*						m_preproLineHead;			// # �̎��̃g�[�N�����w���Ă���

	TokenList					m_preproExprTokenList;		// �O�����萔���̃g�[�N����W�J�����Ɨ̈�
	RpnParser					m_rpnParser;
	RpnEvaluator				m_rpnEvaluator;

	Array<Token*>				m_funcMacroParams;

	bool						m_foundPragmaOnce;
};

} // namespace Parser
LN_NAMESPACE_END

