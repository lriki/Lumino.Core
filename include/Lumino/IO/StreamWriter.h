
#pragma once
#include "Stream.h"
#include "PathName.h"
#include "TextWriter.h"

namespace Lumino
{

/**
	@brief	����������̃G���R�[�f�B���O�ŃX�g���[���ɏ������ނ��߂� TextWriter �̎����ł��B
*/
class StreamWriter
	: public TextWriter
{
public:

	/**
		@brief		�p�X���Ŏw�肵���t�@�C���p�� StreamWriter �����������܂��B
		@param[in]	filePath	: �������ݐ�t�@�C���̃p�X
		@param[in]	encoding	: �������ݎ��Ɏg�p����G���R�[�f�B���O (NULL �̏ꍇ�A�����R�[�h�ϊ��͍s��Ȃ�)
		@param[in]	mode		: �t�@�C���̃I�[�v�����[�h
	*/
	StreamWriter(const PathName& filePath, Text::Encoding* encoding = NULL, FileWriteMode mode = FileWriteMode_Truncate);

	virtual ~StreamWriter();
	
private:
	virtual void Flash();
	virtual void WriteOverride(const void* data, size_t byteCount);

private:
	RefPtr<Stream>	m_stream;
};

} // namespace Lumino
