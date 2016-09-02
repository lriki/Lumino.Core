
#pragma once
#include "Common.h"
#include "TokenList.h"
#include "Frontend\Cpp\Preprocessor.h"

LN_NAMESPACE_BEGIN
namespace parser
{

// .cpp �� .h�A1�̉�͒P�ʂƂȂ�t�@�C���B�p�X���� LocationStateKey �ɂ���Ď����Ǘ��B
// TODO: CodeFile �Ƃ��̖��O�̂ق����������ȁH
class UnitFile
	: public RefObject
{
public:

	void Initialize(const TokenPathName& fileAbsPath);

	const TokenPathName& GetAbsolutePath() const { return m_fileAbsPath; }
	const TokenPathName& GetDirectoryPath() const { return m_directoryAbsPath; }

	void SetTokenList(TokenListPtr tokenList) { m_tokenList = tokenList; }
	TokenListPtr GetTokenList() const { m_tokenList; }

	//void CopyMacroMap(const MacroMap* macroMap);

	void SetMacroMap(const MacroMapContainer& macroMap) { m_macroMap = macroMap; }
	MacroMapContainer* GetMacroMapPtr() { return &m_macroMap; }

public:
	SourceRange SaveMacroReplacementTokens(const Token* begin, const Token* end);	// �L���b�V���ɕۑ�����ƍĔz�u�̉\��������̂ŁA�i�[���ꂽ�ꏊ�̓|�C���^�ł͂Ȃ��C���f�b�N�X�ŕԂ�
	void GetMacroReplacementTokens(const SourceRange& range, const Token** outBegin, const Token** outEnd) const;


LN_INTERNAL_ACCESS:
	TokenPathName		m_fileAbsPath;
	TokenPathName		m_directoryAbsPath;	// m_fileAbsPath �� parent�Bincldue �t�@�C���̌����Ƃ��ŕp�ɂɎg���̂ŁA�������������l�����Ă��炩���ߎ��o���Ă���
	TokenListPtr		m_tokenList;

	//RefPtr<MacroMap>	m_macroMap;				// ���̃t�@�C���̒��Œ�`���ꂽ�}�N���Bundef �� undef �Ƃ��ċL�^����
	MacroMapContainer	m_macroMap;
	uint64_t			m_inputMacroMapHash;
	Array<Token>		m_tokensCache;	// TODO: TokenList �ł������H
	TokenBuffer			m_tokenBuffer;

	// �}�N���̒u���v�f�͂����ƕۑ����Ă����Ȃ���΂Ȃ�Ȃ��B�u���ꏊ�͂��̃N���X����ԃC���[�W���₷���Ǝv���B
	// ����A���̃g�[�N�����X�g�͕ۑ�����K�v�͖����B�ǂ�Ȓ�`������̂�����������΂����B(�֐����͕K�v������ () �Ȃ񂩂͂���Ȃ�)
	// [2016/2/19] �g�[�N�����X�g�������Ă����Ȃ���΂Ȃ�Ȃ��B
	// UIColors.h �Ƃ��A�n�[�h�R�[�f�B���O�Ŗ��ߍ��񂾃��\�[�X�Ƃ��B
	// �S���������Ɏ����Ă����̂��c���C�̂ŁA�ꎞ�t�@�C���ɕۑ����邱�Ƃ���������K�v�����邩���B
};


// 
class IncludeFile
	: public UnitFile
{
public:
};

// �R���p�C���P�ʃt�@�C���B��͂ւ̓��́B (.cpp�ȂǁBinclude�t�@�C���ł͂Ȃ��BFileEntity �͏o�͂ƍl����ׂ��A����)
class CompileUnitFile
	: public UnitFile
{
public:
	Array<IncludeFile*>	includedFiles;
};


} // namespace Parser
LN_NAMESPACE_END

