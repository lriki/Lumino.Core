
#pragma once

namespace Lumino
{
namespace Json
{

template<typename TChar>
class InputStream
{
public:
	InputStream(Stream* stream, Text::Encoding* encoding)
		: m_stream(stream)
		, m_bufferSize(1024)	// TODO: バッファリングサイズ (最低でも BOM が読める分は必要)
		, m_readCharCount(0)
		, m_eof(false)
	{
		m_buffer = LN_NEW TChar[m_bufferSize];
		m_bufferCurrent = m_buffer;
		m_bufferLast = m_buffer + m_bufferSrc.GetSize();
	}

	~InputStream()
	{
		LN_SAFE_DELETE_ARRAY(m_buffer);
	}

	TChar Peek() const { return *m_bufferCurrent; }

	/// 1文字読み進める (進める前に指していた文字を返す)
	TChar Take()
	{
		TChar ch = *m_bufferCurrent;
		Read();
		return ch;
	}

	/// 空白並びを読み飛ばす (抜けた後は Peek() で空白の次の文字が取れる。EOF に到達した場合は false を返す)
	bool SkipWhitespace()
	{
		while (Peek() == ' ' || Peek() == '\n' || Peek() == '\r' || Peek() == '\t') {
			Take();
		}
		return IsEOF();
	}

	/// EOF に到達しているかを確認する
	bool IsEOF() const
	{
		// stream を最後まで読んでいて、バッファリングしたデータも最後まで読みきっている場合
		return m_eof && (m_bufferCurrent == m_bufferLast);
	}

	/// 読み取った文字数の取得
	int GetReadCharCount() const
	{
		return m_readCharCount;
	}

private:
	void Read()
	{
		if (m_bufferCurrent < m_bufferLast) {
			++m_bufferCurrent;
		}
		else if (!m_eof)
		{
			bool isHead = (m_stream->GetPosition() == 0);

			size_t readCount = m_stream->Read(m_buffer, m_bufferSize * sizeof(TChar));
			m_bufferLast = m_buffer + (readCount / sizeof(TChar));
			if (readCount < m_bufferSize * sizeof(TChar))
			{
				*(m_bufferLast - 1) = 0x00;
				m_eof = true;
			}
			else if (isHead)
			{
				// ストリーム先頭の場合は BOM チェックを行う
				size_t bomLen = m_encoding->CheckPreamble(m_buffer, readCount);
				if (bomLen > 0) {
					m_bufferCurrent += bomLen / sizeof(TChar);
				}
			}

			// 読み取った文字数をカウント (エラー表示用)
			m_readCharCount += readCount / sizeof(TChar);
		}
	}

private:
	Stream*			m_stream;
	Text::Encoding*	m_encoding;
	TChar*			m_buffer;
	size_t			m_bufferSize;
	TChar*			m_bufferLast;		///< 最後の有効文字の次を指す
	TChar*			m_bufferCurrent;
	int				m_readCharCount;	//< 読み取った文字数 (エラー表示用)
	bool			m_eof;
};

} // namespace Json
} // namespace Lumino
