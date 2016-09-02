
#pragma once
#include "../Common.h"
#include "InputFile.h"

LN_NAMESPACE_BEGIN
namespace parser
{
	
/**
	@brief	��͑Ώۃt�@�C���̏W��
	@note	�����������炢��Ȃ���������Ȃ����ǁA���̐����̂��߂ɗp�ӁB
*/
class Project
{
public:
	Array<InputFilePtr>		inputFileList;

	Project() = default;
	~Project() = default;

	void AddInputFile(const PathName& filePath);
};

} // namespace Parser
LN_NAMESPACE_END

