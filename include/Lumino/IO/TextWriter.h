
#pragma once
#include "../Base/RefObject.h"
#include "../Base/ByteBuffer.h"
#include "../Base/Locale.h"
#include "../Base/String.h"
#include "../Text/Encoding.h"
#include "../Text/EncodingConverter.h"

LN_NAMESPACE_BEGIN

/**
	@brief	�e��f�[�^�𕶎���Ƃ��ď������݂��s���@�\�̃x�[�X�N���X�ł��B�@�\���g�p����ɂ� StreamWriter �� StringWriter ���g�p���܂��B
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
	void SetEncoding(Encoding* encoding);

	/**
		@brief		���� TextWriter ���o�͂��镶����̃G���R�[�f�B���O���擾���܂��B
	*/
	Encoding* GetEncoding() const;

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
	void Write(const String& str);

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

	/**
		@brief		�f�[�^�̏������ݐ���������܂��B
	*/
	virtual void WriteOverride(const void* data, size_t byteCount) = 0;

private:
	void WriteInternal(const TCHAR* str, int len);

private:
	static const int	BufferSize = 2048;
	EncodingConverter	m_converter;
	String				m_newLine;
	Locale				m_locale;
	bool				m_writtenPreamble;
};

LN_NAMESPACE_END
