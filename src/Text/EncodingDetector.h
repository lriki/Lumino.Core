#pragma once

#include "../../include/Lumino/Text/Encoding.h"

namespace Lumino
{
namespace Text
{

class IMBSCodeDetector
{
public:

	virtual ~IMBSCodeDetector() {}

	/// ���
	/// untilUnmatch �� true �̏ꍇ�͕s��v�����������炻�̎��_�ŉ�͂��I������B
	/// ���̂Ƃ��A�s��v�����܂ł͉�͍ς݂Ƃ���B(UnMatch Count �� 1 �ł��邱��)
	/// �܂��ADetector �͏�Ԃ�ێ�����B�Ăяo������ Detect(true) �� Detect(false) ��2��
	/// �Ăяo�����s�����A2��ڂ�1��ڂɉ�͏I�������ʒu����J�n����B(���׌y���̂���)
	virtual void Detect(bool untilUnmatch) = 0;

	/// EncodingType
	virtual EncodingType GetEncodingType() = 0;

	/// �}�b�`�|�C���g
	virtual int GetScore() = 0;

	/// �s���ȃo�C�g�V�[�P���X������������
	virtual int GetUnMatchCount() = 0;

	/// �s���ȃo�C�g�V�[�P���X���������Ƃ��̍s�ԍ�
	virtual int GetUnMatchLine() = 0;
};

/**
	@brief	UTF8N �]���N���X
*/
class UTF8NDetector : public IMBSCodeDetector
{
public:
	UTF8NDetector(const void* bytes, size_t bytesSize);
	virtual ~UTF8NDetector() {}
	virtual EncodingType GetEncodingType() { return EncodingType_UTF8N; }
	virtual void Detect(bool untilUnmatch);
	virtual int GetScore() { return m_score; }
	virtual int GetUnMatchCount() { return m_unmatch; }
	virtual int GetUnMatchLine() { return m_lineNum; }

private:
	byte_t*	m_buffer;
	size_t	m_bufferSize;
	size_t	m_pos;
	int		m_lineNum;
	int		m_score;
	int		m_unmatch;
};

/**
	@brief	SJIS �]���N���X
*/
class SJISDetector : public IMBSCodeDetector
{
public:
	SJISDetector(const void* bytes, size_t bytesSize);
	virtual ~SJISDetector() {}
	virtual EncodingType GetEncodingType() { return EncodingType_SJIS; }
	virtual void Detect(bool untilUnmatch);
	virtual int GetScore() { return m_score; }
	virtual int GetUnMatchCount() { return m_unmatch; }
	virtual int GetUnMatchLine() { return m_lineNum; }

private:
	byte_t*	m_buffer;
	size_t	m_bufferSize;
	size_t	m_pos;
	int		m_lineNum;
	int		m_score;
	int		m_unmatch;
};

/**
	@bfief		�����R�[�h���������o����N���X�ł��B
*/
class EncodingDetector
{
public:
	typedef unsigned char Byte;

public:
	EncodingType Detect(const void* bytes, size_t bytesSize);

	/// �\���̈�ԍ��������R�[�h
	EncodingType GetEncodingType() const { return m_type; }

private:
	EncodingType CheckUTFBom();

private:
	const Byte*		m_buffer;
	size_t			m_bufferSize;
	EncodingType	m_type;
};


} // namespace Text
} // namespace Lumino
