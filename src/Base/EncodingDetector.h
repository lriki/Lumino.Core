#pragma once

namespace Lumino
{

enum EncodingType
{
	EncodingType_Unknown = 0,	///< 不明な文字コード (判別失敗。またはバイナリファイル)
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

	/// 解析
	virtual void Detect(const void* bytes, size_t bytesSize) = 0;

	/// EncodingType
	virtual EncodingType GetEncodingType() = 0;

	/// 不正なバイトシーケンスが見つかったか
	virtual bool IsUnMatch() = 0;

	/// 不正なバイトシーケンスを見つけたときの行番号
	virtual int GetUnMatchLine() = 0;

	/// マッチポイント
	virtual int GetScore() = 0;
};

/**
	@brief	UTF8N 評価クラス
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
	@brief	SJIS 評価クラス
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

