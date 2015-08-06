
#pragma once
#include "../Base/RefObject.h"
#include "../Base/ByteBuffer.h"
#include "../Base/Locale.h"
#include "../Base/String.h"
#include "../Text/Encoding.h"
#include "../Text/EncodingConverter.h"

namespace Lumino
{

/**
	@brief	�e��f�[�^�𕶎���Ƃ��ď������݂��s���N���X�ł��B
*/
class TextWriter
	: public RefObject
{
public:
	TextWriter();
	virtual ~TextWriter();
	
public:
	
	/**
		@brief		���� TextWriter ���o�͂��镶����̃G���R�[�f�B���O���w�肵�܂��B
		@details	�ݒ肷��ꍇ�A����̏������݂̑O�ɐݒ肷��K�v������܂��B�r������ύX�͂ł��܂���B
	*/
	void SetEncoding(Text::Encoding* encoding);

	/**
		@brief		���� TextWriter �Ŏg�p������s�������ݒ肵�܂��B
		@details	�K��l�� String::GetNewLine() �Ŏ擾�ł���l�ł��B
	*/
	void SetNewLine(const String& newLine);

	/**
		@brief		������̏������ݎ��Ɏg�p���鏑����\�����P�[�����w�肵�܂��B
	*/
	void SetFormatLocale(const Locale& locale);

	/**
		@brief		��������������݂܂��B
		@param[in]	str		: �������ޕ�����
		@param[in]	len		: �������ޕ����� (-1 �̏ꍇ�� \0 �܂�)
	*/
	void Write(const TCHAR* str, int len = -1);

	/**
		@brief		�������w�肵�ĕ�������������݂܂��B
		@param[in]	str		: �����w�蕶����
		@param[in]	...		: �������X�g
		@details	���ӓ_���� String::Format() ���Q�Ƃ��Ă��������B
	*/
	void WriteFormat(const TCHAR* format, ...);

	/**
		@brief		�������������݂܂��B
		@param[in]	ch		: �������ޕ���
	*/
	void WriteChar(TCHAR ch);

	/**
		@brief		�����l�𕶎���ɕϊ����ď������݂܂��B
		@param[in]	value	: �������ޒl
	*/
	void WriteInt16(int16_t value);			///< @copydoc WriteInt16
	void WriteInt32(int32_t value);			///< @copydoc WriteInt16
	void WriteInt64(int64_t value);			///< @copydoc WriteInt16
	void WriteByte(byte_t value);			///< @copydoc WriteInt16
	void WriteUInt16(uint16_t value);		///< @copydoc WriteInt16
	void WriteUInt32(uint32_t value);		///< @copydoc WriteInt16
	void WriteUInt64(uint64_t value);		///< @copydoc WriteInt16

	/**
		@brief		�����l�𕶎���ɕϊ����ď������݂܂��B
		@param[in]	value	: �������ޒl
		@attention	�����_�����̓��P�[���Ɉˑ����܂��B(�Ⴆ�΃t�����X��ł� ',' �ɂȂ�܂�)
					�K�� '.' �ŏ������܂��悤�ɂ���ɂ́ASetFormatLocale() �� ANSI ���P�[�����w�肵�Ă��������B
	*/
	void WriteFloat(float value);
	void WriteDouble(double value);			///< @copydoc WriteFloat
	
	/**
		@brief		���s���������݂܂��B
	*/
	void WriteLine();

	/**
		@brief		��������������݁A�����ĉ��s���������݂܂��B
		@param[in]	str		: �������ޕ�����
		@param[in]	len		: �������ޕ����� (-1 �̏ꍇ�� \0 �܂�)
	*/
	void WriteLine(const TCHAR* str, int len = -1);

	/**
		@brief		�������w�肵�ĕ�������������݁A�����ĉ��s���������݂܂��B
		@param[in]	str		: �����w�蕶����
		@param[in]	...		: �������X�g
		@details	���ӓ_���� String::Format() ���Q�Ƃ��Ă��������B
	*/
	void WriteLineFormat(const TCHAR* format, ...);
	
	/**
		@brief		�������������݂܂��B
		@param[in]	ch		: �������ޕ���
	*/
	void WriteLineChar(TCHAR ch);

	/**
		@brief		�����l�𕶎���ɕϊ����ď������݁A�����ĉ��s���������݂܂��B
		@param[in]	value	: �������ޒl
	*/
	void WriteLineInt16(int16_t value);		///< @copydoc WriteLineInt16
	void WriteLineInt32(int32_t value);		///< @copydoc WriteLineInt16
	void WriteLineInt64(int64_t value);		///< @copydoc WriteLineInt16
	void WriteLineByte(byte_t value);		///< @copydoc WriteLineInt16
	void WriteLineUInt16(uint16_t value);	///< @copydoc WriteLineInt16
	void WriteLineUInt32(uint32_t value);	///< @copydoc WriteLineInt16
	void WriteLineUInt64(uint64_t value);	///< @copydoc WriteLineInt16

	/**
		@brief		�����l�𕶎���ɕϊ����ď������݁A�����ĉ��s���������݂܂��B
		@param[in]	value		: �������ޒl
		@attention	�����_�����̓��P�[���Ɉˑ����܂��B(�Ⴆ�΃t�����X��ł� ',' �ɂȂ�܂�)
					�K�� '.' �ŏ������܂��悤�ɂ���ɂ́ASetFormatLocale() �� ANSI ���P�[�����w�肵�Ă��������B
	*/
	void WriteLineFloat(float value);
	void WriteLineDouble(double value);		///< @copydoc WriteLineFloat

	/**
		@brief		�o�b�t�@�����O�f�[�^��S�ăX�g���[���ɏ����o���܂��B
	*/
	virtual void Flash();

protected:
	virtual void WriteOverride(const void* data, size_t byteCount) = 0;

private:
	void WriteInternal(const TCHAR* str, int len);

private:
	static const int BufferSize = 2048;

	Text::EncodingConverter	m_converter;

	//RefPtr<Text::Encoding>	m_encoding;
	//RefPtr<Text::Decoder>	m_decoder;		///< String �𒆊ԕ����R�[�h (UTF16) �ɕϊ����邽�߂̃f�R�[�_
	//RefPtr<Text::Encoder>	m_encoder;		///< ���ԕ����R�[�h (UTF16) ���o�͕����R�[�h (m_encoding) �ɕϊ����邽�߂̃G���R�[�_
	String					m_newLine;
	Locale					m_locale;
	//ByteBuffer				m_utf16Buffer;		///< ���ԕ����R�[�h�o�b�t�@
	//ByteBuffer				m_outputBuffer;		///< �ϊ���o�b�t�@
	//int						m_safeTCharCount;	///< TCHAR �� ���ԃo�b�t�@ (UTF16) ���A���ԃo�b�t�@�ɔ[�߂��� TCHAR ������
	bool					m_writtenPreamble;	///< BOM ���������񂾂� (BOM �̖����G���R�[�f�B���O�ł���΍ŏ����� true)
};

} // namespace Lumino
