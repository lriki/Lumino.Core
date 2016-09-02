
#include "Internal.h"
#include "Frontend\Cpp\CppLexer.h"
#include "DiagnosticsManager.h"
#include "UnitFile.h"
#include "Context.h"

LN_NAMESPACE_BEGIN
namespace parser
{

//=============================================================================
// Context
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Context::Clear()
{
	m_codeFileMap.clear();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ResultState Context::LookupPreprocessedIncludeFile(
	CompileUnitFile* rootCompileUnitFile,
	const TokenPathName& basePath,
	const TokenPathName& filePath,
	const Array<TokenPathName>* additionalIncludePaths,
	const MacroMapContainer& parentMacroMap,
	ReferenceTracker* referenceTracker,
	DiagnosticsItemSet* parentDiag,
	IncludeFile** outFile)
{
	if (additionalIncludePaths == nullptr) return ResultState::Error;

	//------------------------------------------------------------
	// include �t�@�C������������
	PathName includeFilePath;
	// ��΃p�X�Ȃ炻�̂܂܎g��
	if (filePath.IsAbsolute())
	{
		includeFilePath = filePath;
	}
	// �e�R�[�h�t�@�C������̑��΃p�X�ɑ��݂��邩�H
	if (basePath.ExistsFileInDirectory(filePath))
	{
		includeFilePath = PathName(basePath, filePath);
	}
	// -I �Ŏw�肳�ꂽ�t�H���_�ɑ��݂��邩�H
	else
	{
		for (auto& path : *additionalIncludePaths)
		{
			if (path.ExistsFileInDirectory(filePath))
			{
				includeFilePath = PathName(path, filePath);
				break;
			}
		}
	}

	// Error: include �t�@�C����������Ȃ�����
	if (includeFilePath.IsEmpty())
	{
		parentDiag->Report(DiagnosticsCode::Preprocessor_IncludeFileNotFount, includeFilePath.GetString());
		return ResultState::Error;
	}

	// ��΃p�X�ɂ���
	includeFilePath = includeFilePath.CanonicalizePath();


	//------------------------------------------------------------

	//if (rootCompileUnitFile->includedFiles.Contains([&](IncludeFile* item){ return item->GetAbsolutePath().Equals(includeFilePath); }))
	//{

	//}




	uint64_t key = MakeCacheFileKey(includeFilePath, additionalIncludePaths, parentMacroMap.GetConst());

	auto itr = m_codeFileMap.find(key);
	if (itr != m_codeFileMap.end())
	{
		*outFile = itr->second;
		return ResultState::Success;	// TODO: CodeFile �ɉ�͌��ʂ�����������
	}
	else
	{
		// �t�@�C�����e����x�S�ēǂݍ���
		// TODO: �����R�[�h
		FileStreamPtr file = FileStream::Create(includeFilePath, FileOpenMode::Read);
		ByteBuffer buffer(file->GetLength(), false);
		file->Read(buffer.GetData(), buffer.GetSize());

		auto codeFile = RefPtr<IncludeFile>::MakeRef();
		codeFile->Initialize(includeFilePath);

		// �g�[�N������
		CppLexer lex;
		TokenListPtr tokens = lex.Tokenize(buffer, parentDiag);	// TODO: �G���[
		codeFile->SetTokenList(tokens);				// �g�[�N���X�g�͑S���o���Ă��� (define �̒u���v�f�ƃG���e�B�e�B�Q�Ƃ����o����������Ă��ǂ�)


		//printf("%d\n", parentMacroMap.GetConst()->IsDefined("AAA"));

		// �v���v���Z�X
		Preprocessor preprocessor;
		LN_RESULT_CALL(preprocessor.BuildPreprocessedTokenList(this, rootCompileUnitFile, tokens, codeFile, additionalIncludePaths, parentMacroMap, referenceTracker, parentDiag));
		//codeFile->GetMacroMap()->SetFreeze(true);	// �m��B���Ƃ͕ω����Ȃ�

		printf("%d\n", codeFile->GetMacroMapPtr()->GetConst()->IsDefined("BBB"));

		m_codeFileMap.insert({ key, codeFile });
		*outFile = codeFile;
		return ResultState::Success;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uint64_t Context::MakeCacheFileKey(const PathName& includeFilePath, const Array<TokenPathName>* additionalIncludePaths, const MacroMap* macroMap)
{
	uint64_t key = Hash::CalcHash(includeFilePath.c_str(), includeFilePath.GetLength());
	for (auto& path : *additionalIncludePaths)
	{
		key += Hash::CalcHash(path.c_str(), path.GetLength());
	}
	key += macroMap->GetHashCode();
	return key;
}

} // namespace Parser
LN_NAMESPACE_END

