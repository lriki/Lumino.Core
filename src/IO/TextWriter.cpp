
#include "../Internal.h"
#include <Lumino/Base/StringTraits.h>
#include <Lumino/IO/TextWriter.h>

namespace Lumino
{

//=============================================================================
// TextWriter
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TextWriter::TextWriter()
	: m_newLine(String::GetNewLine())
	, m_locale()
	//, m_utf16Buffer(BufferSize, false)
	, m_writtenPreamble(true)
{
	// String �𒆊ԕ����R�[�h (UTF16) �ɕϊ����邽�߂̃f�R�[�_
	//m_decoder.Attach(Text::Encoding::GetTCharEncoding()->CreateDecoder());

	// �ϊ����� TCHAR
	m_converter.SetSourceEncoding(Text::Encoding::GetTCharEncoding());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TextWriter::~TextWriter()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TextWriter::SetEncoding(Text::Encoding* encoding)
{
	m_converter.SetDestinationEncoding(encoding);
#if 0
	m_encoding = encoding;

	if (m_encoding != NULL)
	{
		m_encoder.Attach(m_encoding->CreateEncoder());

		// ���ԃo�b�t�@���w��G���R�[�f�B���O�ɑS�ĕϊ������Ƃ��ɕK�v�ɂȂ�ő�o�b�t�@�T�C�Y���v�Z���A�������m��
		size_t maxSize = Text::Encoding::GetConversionRequiredByteCount(Text::Encoding::GetUTF16Encoding(), m_encoding, m_utf16Buffer.GetSize());
		m_outputBuffer.Resize(maxSize, false);

		// TCHAR �� ���ԃo�b�t�@ (UTF16) ���A���ԃo�b�t�@�ɔ[�߂��� TCHAR ������
		m_safeTCharCount = m_utf16Buffer.GetSize() / Text::Encoding::GetTCharEncoding()->GetMaxByteCount();

		// BOM ���K�v��
		if (m_encoding->GetPreamble() != NULL) {
			m_writtenPreamble = false;
		}
		else {
			m_writtenPreamble = true;	// ���� BOM ���������񂾂��Ƃɂ���
		}
		
	}
#endif
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TextWriter::SetNewLine(const String& newLine)
{
	m_newLine = newLine;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TextWriter::SetFormatLocale(const Locale& locale)
{
	m_locale = locale;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TextWriter::Write(const TCHAR* str, int len)
{
	WriteInternal(str, (len < 0) ? StringTraits::StrLen(str) : len);
}

void TextWriter::WriteFormat(const TCHAR* format, ...)
{
	LN_THROW(0, NotImplementedException);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TextWriter::WriteChar(TCHAR ch)
{
	WriteInternal(&ch, 1);
}
void TextWriter::WriteInt16(int16_t value)
{
	TCHAR buf[64];
	int len = StringTraits::SPrintf(buf, 64, _T("%d"), value);
	WriteInternal(buf, len);
}
void TextWriter::WriteInt32(int32_t value)
{
	TCHAR buf[64];
	int len = StringTraits::SPrintf(buf, 64, _T("%d"), value);
	WriteInternal(buf, len);
}
void TextWriter::WriteInt64(int64_t value)
{
	TCHAR buf[64];
	int len = StringTraits::SPrintf(buf, 64, _T("%lld"), value);
	WriteInternal(buf, len);
}
void TextWriter::WriteByte(byte_t value)
{
	TCHAR buf[64];
	int len = StringTraits::SPrintf(buf, 64, _T("%u"), value);
	WriteInternal(buf, len);
}
void TextWriter::WriteUInt16(uint16_t value)
{
	TCHAR buf[64];
	int len = StringTraits::SPrintf(buf, 64, _T("%u"), value);
	WriteInternal(buf, len);
}
void TextWriter::WriteUInt32(uint32_t value)
{
	TCHAR buf[64];
	int len = StringTraits::SPrintf(buf, 64, _T("%u"), value);
	WriteInternal(buf, len);
}
void TextWriter::WriteUInt64(uint64_t value)
{
	TCHAR buf[64];
	int len = StringTraits::SPrintf(buf, 64, _T("%llu"), value);
	WriteInternal(buf, len);
}
void TextWriter::WriteFloat(float value)
{
	TCHAR buf[64];
	int len = StringTraits::tsnprintf_l(buf, 64, _T("%f"), m_locale.GetNativeLocale(), value);
	WriteInternal(buf, len);
}
void TextWriter::WriteDouble(double value)
{
	TCHAR buf[64];
	int len = StringTraits::tsnprintf_l(buf, 64, _T("%lf"), m_locale.GetNativeLocale(), value);
	WriteInternal(buf, len);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TextWriter::WriteLine()
{
	WriteInternal(m_newLine.GetCStr(), m_newLine.GetLength());
}
void TextWriter::WriteLine(const TCHAR* str, int len)
{
	Write(str, len);
	WriteLine();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TextWriter::WriteLineFormat(const TCHAR* format, ...)
{
	LN_THROW(0, NotImplementedException);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TextWriter::WriteLineChar(TCHAR value)
{
	WriteChar(value);
	WriteLine();
}
void TextWriter::WriteLineInt16(int16_t value)
{
	WriteInt16(value);
	WriteLine();
}
void TextWriter::WriteLineInt32(int32_t value)
{
	WriteInt32(value);
	WriteLine();
}
void TextWriter::WriteLineInt64(int64_t value)
{
	WriteInt64(value);
	WriteLine();
}
void TextWriter::WriteLineByte(uint8_t value)
{
	WriteByte(value);
	WriteLine();
}
void TextWriter::WriteLineUInt16(uint16_t value)
{
	WriteUInt16(value);
	WriteLine();
}
void TextWriter::WriteLineUInt32(uint32_t value)
{
	WriteUInt32(value);
	WriteLine();
}
void TextWriter::WriteLineUInt64(uint64_t value)
{
	WriteUInt64(value);
	WriteLine();
}
void TextWriter::WriteLineFloat(float value)
{
	WriteFloat(value);
	WriteLine();
}
void TextWriter::WriteLineDouble(double value)
{
	WriteDouble(value);
	WriteLine();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TextWriter::Flash()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TextWriter::WriteInternal(const TCHAR* str, int len)
{
	// BOM �̏������݂��K�v�ł���΂����ŏ�������
	if (!m_writtenPreamble)
	{
		const byte_t* bom = m_converter.GetDestinationEncoding()->GetPreamble();
		size_t len = strlen((char*)bom);
		WriteOverride(bom, len);
		m_writtenPreamble = true;
	}

	// ���͂���Ȃ���ɂ��邱�Ƃ͖���
	if (str == NULL || len == 0) {
		return;
	}

	const ByteBuffer buf = m_converter.Convert(str, len * sizeof(TCHAR));

	WriteOverride(buf.GetConstData(), buf.GetSize());

#if 0

	if (m_decoder != NULL && m_encoder != NULL)
	{
		// �ϊ���Ԃ�ێ��ł��� Encoding �ł���Η]���Ƀ��������m�ۂ��Ȃ��ŕϊ��ł���B
		if (m_decoder->CanRemain()/* && m_encoder->CanRemain()*/)	// encoder ���͏�ԕۑ��ł��Ȃ��Ă��ǂ�
		{
			// ��̃R�[�h���L���X�g���炯�ɂȂ�Ȃ��悤��
			Text::UTF16* utf16Buf = (Text::UTF16*)m_utf16Buffer.GetData();
			int utf16ElementCount = m_utf16Buffer.GetSize() / sizeof(Text::UTF16);

			int convCount = 0;
			while (convCount < len)
			{
				int charCount = std::min(len - convCount, m_safeTCharCount);

				// TCHAR �𒆊ԃR�[�h��
				size_t outBytesUsed, outCharsUsed;
				m_decoder->ConvertToUTF16((byte_t*)&str[convCount], charCount * sizeof(TCHAR), utf16Buf, utf16ElementCount, &outBytesUsed, &outCharsUsed);

				// ���ԃR�[�h���o�̓R�[�h��
				m_encoder->ConvertFromUTF16(utf16Buf, outCharsUsed, m_outputBuffer.GetData(), m_outputBuffer.GetSize(), &outBytesUsed, &outCharsUsed);

				// �o��
				WriteOverride(m_outputBuffer.GetData(), outBytesUsed);

				convCount += charCount;
			}
		}
		// �f�R�[�_���ϊ���Ԃ�ێ��ł��Ȃ��ꍇ�͂�ނ𓾂Ȃ��̂ňꎞ���������m�ۂ��A�\�[�X�o�b�t�@�S�̂���x�ɕϊ�����B
		else
		{
			// �o��
			WriteOverride(m_outputBuffer.GetData(), outBytesUsed);
		}
	}
	// �G���R�[�_������ (SetEncoding() ����Ă��Ȃ�) �ꍇ�́A�����R�[�h�ϊ����s�킸 TCHAR �����̂܂܏o�͂���
	else if (m_encoder == NULL)
	{
		WriteOverride(str, len * sizeof(TCHAR));
	}
	else {
		LN_THROW(0, NotImplementedException);
	}
#endif
}


} // namespace Lumino
