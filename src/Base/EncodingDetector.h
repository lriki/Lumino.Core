#pragma once

namespace Lumino
{

enum EncodingType
{
	EncodingType_Unknown = 0,	///< �s���ȕ����R�[�h (���ʎ��s�B�܂��̓o�C�i���t�@�C��)
	EncodingType_UTF8,
	EncodingType_UTF8N,
	EncodingType_UTF16L,
	EncodingType_UTF16B,
	EncodingType_UTF16LN,
	EncodingType_UTF16BN,
	EncodingType_UTF32L,
	EncodingType_UTF32B,
	EncodingType_UTF32LN,
	EncodingType_UTF32BN,

	EncodingType_SJIS,
};

class IMBSCodeDetector
{
public:

	virtual ~IMBSCodeDetector() {}

	/// ���
	virtual void Detect(const void* bytes, size_t bytesSize) = 0;

	/// EncodingType
	virtual EncodingType GetEncodingType() = 0;

	/// �s���ȃo�C�g�V�[�P���X������������
	virtual bool IsUnMatch() = 0;

	/// �s���ȃo�C�g�V�[�P���X���������Ƃ��̍s�ԍ�
	virtual int GetUnMatchLine() = 0;

	/// �}�b�`�|�C���g
	virtual int GetScore() = 0;
};

/**
	@brief	UTF8N �]���N���X
*/
class UTF8NDetector : public IMBSCodeDetector
{
public:
	virtual ~UTF8NDetector() {}
	virtual EncodingType GetEncodingType() { return EncodingType_UTF8N; }
	virtual void Detect(const void* bytes, size_t bytesSize);
	virtual bool IsUnMatch() { return m_unmatch; }
	virtual int GetUnMatchLine() { return m_lineNum; }
	virtual int GetScore() { return m_score; }

private:
	int		m_lineNum;
	int		m_score;
	bool	m_unmatch;
};

/**
	@brief	SJIS �]���N���X
*/
class SJISDetector : public IMBSCodeDetector
{
public:
	virtual ~SJISDetector() {}
	virtual EncodingType GetEncodingType() { return EncodingType_SJIS; }
	virtual void Detect(const void* bytes, size_t bytesSize);
	virtual bool IsUnMatch() { return m_unmatch; }
	virtual int GetUnMatchLine() { return m_lineNum; }
	virtual int GetScore() { return m_score; }

private:
	int		m_lineNum;
	int		m_score;
	bool	m_unmatch;
};


class EncodingDetector
{
public:
	typedef unsigned char Byte;

public:
	EncodingType Detect(const void* bytes, size_t bytesSize);

private:
	EncodingType CheckUTFBom();

private:
	const Byte*		m_buffer;
	size_t			m_bufferSize;
};



}

