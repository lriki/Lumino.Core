
#pragma once
#include <unordered_map>
#include "Common.h"
#include "Frontend\Cpp\Preprocessor.h"

LN_NAMESPACE_BEGIN
namespace parser
{

class CompileOptions
{
public:


	Array<TokenPathName>	m_additionalIncludePaths;	// �ǉ��̃C���N���[�h�f�B���N�g��
	RefPtr<MacroMap>		m_preprocessorDefinitions;	// ��`�ς݃}�N�� (��͊J�n���� MacroMap)
	

	//CompileOptions*	m_parent;
};

//struct CacheFileKey
//{
//	uint64_t	includePathsKey = 0;
//	uint64_t	definedMacrosKey = 0;
//};

class Context
	: public RefObject
{
public:
	void Clear();

	// TODO: �R���p�C���I�v�V�����̃C���N���[�h�t�@�C���̌����p�X�̕��т���v���Ă���K�v������B
	/*
		������͍ς݂̃t�@�C�����ق����Ƃ��͂��̊֐����g���B
		.c �Ȃǂ̃R���p�C���P�ʂƂȂ�t�@�C���́A�R���p�C���I�v�V�����ŃL�[���w��B
		.h �Ȃǂ̃C���N���[�h�t�@�C����#include���_�̃}�N���ŃL�[���w��B
	*/
//	UnitFile* LookupCachedUnitFile(const TokenPathName& fileAbsPath, const CacheFileKey& key);

	ResultState LookupPreprocessedIncludeFile(
		CompileUnitFile* rootCompileUnitFile,
		const TokenPathName& basePath,
		const TokenPathName& filePath,
		const Array<TokenPathName>* additionalIncludePaths,
		const MacroMapContainer& parentMacroMap,
		ReferenceTracker* referenceTracker,
		DiagnosticsItemSet* parentDiag,
		IncludeFile** outFile);

	static uint64_t MakeCacheFileKey(const PathName& includeFilePath, const Array<TokenPathName>* additionalIncludePaths, const MacroMap* macroMap);

	// FontendContext/AnalayzerContext �������ق��������H

private:
	std::unordered_map<uint64_t, IncludeFilePtr>	m_codeFileMap;
};

} // namespace Parser
LN_NAMESPACE_END

