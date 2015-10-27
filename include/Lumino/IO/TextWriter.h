
#pragma once
#include "../Base/RefObject.h"
#include "../Base/ByteBuffer.h"
#include "../Base/Locale.h"
#include "../Base/String.h"
#include "../Text/Encoding.h"
#include "../Text/EncodingConverter.h"

LN_NAMESPACE_BEGIN

/**
	@brief	各種データを文字列として書き込みを行う機能のベースクラスです。機能を使用するには StreamWriter や StringWriter を使用します。
*/
class TextWriter
	: public RefObject
{
public:
	TextWriter();
	virtual ~TextWriter();
	
public:
	
	/**
		@brief		この TextWriter が出力する文字列のエンコーディングを指定します。
		@details	設定する場合、初回の書き込みの前に設定する必要があります。途中から変更はできません。
	*/
	void SetEncoding(Encoding* encoding);

	/**
		@brief		この TextWriter が出力する文字列のエンコーディングを取得します。
	*/
	Encoding* GetEncoding() const;

	/**
		@brief		この TextWriter で使用する改行文字列を設定します。
		@details	規定値は String::GetNewLine() で取得できる値です。
	*/
	void SetNewLine(const String& newLine);

	/**
		@brief		文字列の書き込み時に使用する書式を表すロケールを指定します。
	*/
	void SetFormatLocale(const Locale& locale);

	/**
		@brief		文字列を書き込みます。
		@param[in]	str		: 書き込む文字列
		@param[in]	len		: 書き込む文字数 (-1 の場合は \0 まで)
	*/
	void Write(const TCHAR* str, int len = -1);
	void Write(const String& str);

	/**
		@brief		書式を指定して文字列を書き込みます。
		@param[in]	str		: 書式指定文字列
		@param[in]	...		: 引数リスト
		@details	注意点等は String::Format() を参照してください。
	*/
	void WriteFormat(const TCHAR* format, ...);

	/**
		@brief		文字を書き込みます。
		@param[in]	ch		: 書き込む文字
	*/
	void WriteChar(TCHAR ch);

	/**
		@brief		整数値を文字列に変換して書き込みます。
		@param[in]	value	: 書き込む値
	*/
	void WriteInt16(int16_t value);			///< @copydoc WriteInt16
	void WriteInt32(int32_t value);			///< @copydoc WriteInt16
	void WriteInt64(int64_t value);			///< @copydoc WriteInt16
	void WriteByte(byte_t value);			///< @copydoc WriteInt16
	void WriteUInt16(uint16_t value);		///< @copydoc WriteInt16
	void WriteUInt32(uint32_t value);		///< @copydoc WriteInt16
	void WriteUInt64(uint64_t value);		///< @copydoc WriteInt16

	/**
		@brief		実数値を文字列に変換して書き込みます。
		@param[in]	value	: 書き込む値
		@attention	小数点文字はロケールに依存します。(例えばフランス語では ',' になります)
					必ず '.' で書き込まれるようにするには、SetFormatLocale() で ANSI ロケールを指定してください。
	*/
	void WriteFloat(float value);
	void WriteDouble(double value);			///< @copydoc WriteFloat
	
	/**
		@brief		改行を書き込みます。
	*/
	void WriteLine();

	/**
		@brief		文字列を書き込み、続けて改行を書き込みます。
		@param[in]	str		: 書き込む文字列
		@param[in]	len		: 書き込む文字数 (-1 の場合は \0 まで)
	*/
	void WriteLine(const TCHAR* str, int len = -1);

	/**
		@brief		書式を指定して文字列を書き込み、続けて改行を書き込みます。
		@param[in]	str		: 書式指定文字列
		@param[in]	...		: 引数リスト
		@details	注意点等は String::Format() を参照してください。
	*/
	void WriteLineFormat(const TCHAR* format, ...);
	
	/**
		@brief		文字を書き込みます。
		@param[in]	ch		: 書き込む文字
	*/
	void WriteLineChar(TCHAR ch);

	/**
		@brief		整数値を文字列に変換して書き込み、続けて改行を書き込みます。
		@param[in]	value	: 書き込む値
	*/
	void WriteLineInt16(int16_t value);		///< @copydoc WriteLineInt16
	void WriteLineInt32(int32_t value);		///< @copydoc WriteLineInt16
	void WriteLineInt64(int64_t value);		///< @copydoc WriteLineInt16
	void WriteLineByte(byte_t value);		///< @copydoc WriteLineInt16
	void WriteLineUInt16(uint16_t value);	///< @copydoc WriteLineInt16
	void WriteLineUInt32(uint32_t value);	///< @copydoc WriteLineInt16
	void WriteLineUInt64(uint64_t value);	///< @copydoc WriteLineInt16

	/**
		@brief		実数値を文字列に変換して書き込み、続けて改行を書き込みます。
		@param[in]	value		: 書き込む値
		@attention	小数点文字はロケールに依存します。(例えばフランス語では ',' になります)
					必ず '.' で書き込まれるようにするには、SetFormatLocale() で ANSI ロケールを指定してください。
	*/
	void WriteLineFloat(float value);
	void WriteLineDouble(double value);		///< @copydoc WriteLineFloat

	/**
		@brief		バッファリングデータを全てストリームに書き出します。
	*/
	virtual void Flash();

protected:

	/**
		@brief		データの書き込み先を実装します。
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
