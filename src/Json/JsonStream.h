
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
		, m_bufferSize(1024)	// TODO: �o�b�t�@�����O�T�C�Y (�Œ�ł� BOM ���ǂ߂镪�͕K�v)
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

	/// 1�����ǂݐi�߂� (�i�߂�O�Ɏw���Ă���������Ԃ�)
	TChar Take()
	{
		TChar ch = *m_bufferCurrent;
		Read();
		return ch;
	}

	/// �󔒕��т�ǂݔ�΂� (��������� Peek() �ŋ󔒂̎��̕���������BEOF �ɓ��B�����ꍇ�� false ��Ԃ�)
	bool SkipWhitespace()
	{
		while (Peek() == ' ' || Peek() == '\n' || Peek() == '\r' || Peek() == '\t') {
			Take();
		}
		return IsEOF();
	}

	/// EOF �ɓ��B���Ă��邩���m�F����
	bool IsEOF() const
	{
		// stream ���Ō�܂œǂ�ł��āA�o�b�t�@�����O�����f�[�^���Ō�܂œǂ݂����Ă���ꍇ
		return m_eof && (m_bufferCurrent == m_bufferLast);
	}

	/// �ǂݎ�����������̎擾
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
				// �X�g���[���擪�̏ꍇ�� BOM �`�F�b�N���s��
				size_t bomLen = m_encoding->CheckPreamble(m_buffer, readCount);
				if (bomLen > 0) {
					m_bufferCurrent += bomLen / sizeof(TChar);
				}
			}

			// �ǂݎ�������������J�E���g (�G���[�\���p)
			m_readCharCount += readCount / sizeof(TChar);
		}
	}

private:
	Stream*			m_stream;
	Text::Encoding*	m_encoding;
	TChar*			m_buffer;
	size_t			m_bufferSize;
	TChar*			m_bufferLast;		///< �Ō�̗L�������̎����w��
	TChar*			m_bufferCurrent;
	int				m_readCharCount;	//< �ǂݎ���������� (�G���[�\���p)
	bool			m_eof;
};

} // namespace Json
} // namespace Lumino
