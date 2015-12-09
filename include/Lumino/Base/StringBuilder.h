
#pragma once
#include "ByteBuffer.h"
#include "String.h"
#include "Formatter.h"

LN_NAMESPACE_BEGIN

/**
	@brief
*/
template<typename TChar>
class GenericStringBuilderCore
{
public:
	GenericStringBuilderCore();
	~GenericStringBuilderCore();

public:
	void Clear();
	void Append(const TChar ch);
	void Append(const TChar ch, int count);
	void Append(const TChar* str, int length);
	void Append(const byte_t* buffer, int byteCount);
	void Append(const ByteBuffer& buffer);

	void Replace(int start, int length, const TChar* str, int strLength);

	const TChar* c_str() const { return (const TChar*)m_buffer.GetConstData(); }
	int GetLength() const { return m_bufferUsed / sizeof(TChar); }


private:
	void WriteInternal(const TChar* str, int length);

protected:
	ByteBuffer	m_buffer;
	size_t		m_bufferUsed;	///< m_buffer ���̎g�p���o�C�g��
};


template<typename TChar>
class GenericStringBuilder
	: public GenericStringBuilderCore<TChar>
{
public:
	GenericString<TChar> ToString() const
	{
		return GenericString<TChar>(
			(const TChar*)GenericStringBuilderCore<TChar>::m_buffer.GetConstData(),
			GenericStringBuilderCore<TChar>::m_bufferUsed / sizeof(TChar));
	}

	//void Append(const GenericString<TChar>& str)
	//{
	//	GenericStringBuilderCore<TChar>::Append(str.c_str(), str.GetLength());
	//}


	template<typename... TArgs>
	void AppendFormat(const GenericStringRef<TChar>& format, const TArgs&... args);

	void AppendFormatInternal(const GenericStringRef<TChar>& format, typename Formatter<TChar>::FormatList* args);
};






template<typename TChar>
template<typename... TArgs>
void GenericStringBuilder<TChar>::AppendFormat(const GenericStringRef<TChar>& format, const TArgs&... args)
{
	auto list = Formatter<TChar>::MakeArgList(args...);
	AppendFormatInternal(format, &list);
}

template<typename TChar>
void GenericStringBuilder<TChar>::AppendFormatInternal(const GenericStringRef<TChar>& format, typename Formatter<TChar>::FormatList* args)
{
	const TChar* pos = format.GetBegin();
	const TChar* end = format.GetEnd();
	TChar ch;
	while (pos < end)
	{
		// { ��������܂ŉ�郋�[�v
		while (pos < end)
		{
			ch = *pos;
			++pos;

			if (ch == '}')
			{
				if (pos < end && *pos == '}') {	// } �̃G�X�P�[�v "}}"
					++pos;
				}
				else {
					LN_THROW(0, InvalidFormatException);	// �P���� } ������Ă͂Ȃ�Ȃ�
				}
			}
			if (ch == '{')
			{
				if (pos < end && *pos == '{') { 	// { �̃G�X�P�[�v "{{"
					++pos;
				}
				else if (pos >= end) {
					LN_THROW(0, InvalidFormatException);	// { �ŏI�����
				}
				else {
					break;
				}
			}

			Append(ch);
		}
		// ���̎��_�� pos �� { �̎����w���Ă���

		if (pos >= end) { break; }

		// ���̕����͕K�������łȂ���΂Ȃ�Ȃ�
		if ('0' <= *pos && *pos <= '9') {
		}
		else {
			LN_THROW(0, InvalidFormatException);
		}

		//-----------------------------------------------------------
		// Index �R���|�[�l���g (�^�����̃C���f�b�N�X�����o��)
		int index = 0;
		do
		{
			index = (index * 10) + ((*pos) - '0');
			++pos;
			LN_THROW(pos < end, InvalidFormatException);	// �C���f�b�N�X��͒��� \0 �ɂȂ���

		} while ((*pos) >= '0' && (*pos) <= '9');
		LN_THROW(index < args->GetCount(), InvalidFormatException);

		//-----------------------------------------------------------
		// Alignment �R���|�[�l���g
		while (pos < end && *pos == ' ') { pos++; }	// �擪�̋󔒂𖳎�
		bool leftJustify = false;					// ���l�߂ɂ��邩�H
		int width = 0;
		if (*pos == ',')	// , �ł���Ή�͊J�n�B������Ώȗ�����Ă���
		{
			pos++;
			while (pos < end && *pos == ' ') { pos++; }
			LN_THROW(pos < end, InvalidFormatException);

			if (*pos == '-')	// ������ - �Ȃ獶�l��
			{
				leftJustify = true;
				++pos;
				LN_THROW(pos < end, InvalidFormatException);
			}

			// ���͐�ΐ���
			LN_THROW('0' <= *pos && *pos <= '9', InvalidFormatException);
			do
			{
				width = (width * 10) + ((*pos) - '0');
				++pos;
				LN_THROW(pos < end, InvalidFormatException);

			} while ((*pos) >= '0' && (*pos) <= '9');
		}

		//-----------------------------------------------------------
		// FormatString �R���|�[�l���g
		while (pos < end && *pos == ' ') { pos++; }	// �擪�̋󔒂𖳎�
		const TChar* fmtBegin = nullptr;
		const TChar* fmtEnd = nullptr;
		const TChar* fmtParamEnd = nullptr;
		if (*pos == ':')
		{
			pos++;
			fmtBegin = pos;
			while (true)
			{
				LN_THROW(pos < end, InvalidFormatException);
				if (*pos == '}') {
					break;
				}
				++pos;
				fmtEnd = pos;
			}

			// "D4" �̂悤�Ɍ�낪�����̃p�����[�^�ɂȂ��Ă���Ƃ��A�����Ő������������o���Ă��܂� (�ϊ������y�ł���悤��)
			ch = *(fmtEnd - 1);
			if (ch >= '0' && ch <= '9')
			{
				fmtParamEnd = fmtEnd;
				do
				{
					--fmtEnd;
					ch = *(fmtEnd - 1);

				} while (ch >= '0' && ch <= '9');
			}
		}

		// �Ō�� } �łȂ���΂Ȃ�Ȃ�
		LN_THROW(*pos == '}', InvalidFormatException);

		GenericString<TChar> str = args->GetArg(index).DoFormat(GenericStringRef<TChar>(fmtBegin, fmtEnd), GenericStringRef<TChar>(fmtEnd, fmtParamEnd));

		int pad = width - str.GetLength();
		if (!leftJustify && pad > 0) Append(' ', pad);
		Append(str.c_str(), str.GetLength());
		if (leftJustify && pad > 0) Append(' ', pad);

		++pos;
	}
}

typedef GenericStringBuilder<TCHAR>		StringBuilder;
typedef GenericStringBuilder<char>		StringBuilderA;
typedef GenericStringBuilder<wchar_t>	StringBuilderW;



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TChar>
template<typename... TArgs>
GenericString<TChar> GenericString<TChar>::Format(const GenericStringRef<TChar>& format, const TArgs&... args)
{
	GenericStringBuilder<TChar> sb;
	sb.AppendFormat(format, args...);
	return sb.ToString();
}

LN_NAMESPACE_END
