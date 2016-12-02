﻿
#pragma once
#include "../Base/EnumExtension.h"
#include "../Base/String.h"

LN_NAMESPACE_BEGIN
namespace tr {

/** JSON 解析のエラーコード */
LN_ENUM(JsonParseError)
{
	NoError = 0,						///< エラーは発生していない
	DocumentEmpty,						///< 入力文字列が空だった
	DocumentRootNotSingular,			///< 複数のルート要素が見つかった
	ValueInvalid,						///< 無効な値
	StringEscapeInvalid,				///< 無効なエスケープシーケンス
	StringMissQuotationMark,			///< 文字列の開始と終端が一致しなかった (" が少ない)
	StringInvalidEncoding,				///< 変換できない文字が存在した
	ArrayMissCommaOrSquareBracket,		///< 配列の要素の後に , か ] が無かった
	ObjectMissKeyStart,					///< オブジェクトメンバの名前の開始 " が見つからなかった
	ObjectMissColon,					///< オブジェクトメンバの : が見つからなかった
	ObjectMissCommaOrCurlyBracket,		///< オブジェクトメンバの後に , か } が見つからなかった

	NumberInvalid,						///< 無効な数値
	NumberOverflow,						///< 数値の変換でオーバーフローが発生した

	Termination,						///< Hander で中断された
};
LN_ENUM_DECLARE(JsonParseError);

/**
	@brief	JSON 解析中のエラーを表します。
*/
class JsonError
{
public:
	JsonError()
		: ErrorCode(JsonParseError::NoError)
		, Offset(0)
		, Message()
	{}

public:
	void SetError(JsonParseError errorCode, int offset) { ErrorCode = errorCode; Offset = offset; }

public:
	JsonParseError	ErrorCode;
	int				Offset;
	String			Message;
};





/** JSON 解析のエラーコード */
LN_ENUM(JsonParseError2)
{
	NoError = 0,						/**< エラーは発生していない */

	UnterminatedString,		/**< 文字列の終端が見つかる前に EOF が見つかった。*/
	InvalidStringChar,		/**< 文字列内に JSON では使用できない文字が見つかった。(制御文字など)*/
	InvalidStringEscape,	/**< 文字列内に無効なエスケープシーケンスが見つかった。*/
	UnexpectedToken,		/**< 予期しないトークンが見つかった。*/
	InvalidNumber,			/**< 不正な数値形式。*/
	NumberOverflow,			/**< 数値の変換でオーバーフローが発生した。*/
	InvalidObjectClosing,	/**< オブジェクトが正しく閉じられていない。*/

	ArrayInvalidClosing,	/**< 配列が正しく閉じられていない。*/

	ValueInvalid,			/**< 無効な値 */
};
LN_ENUM_DECLARE(JsonParseError2);

/**
	@brief	JSON 解析中のエラーを表します。
*/
class JsonError2
{
public:
	JsonParseError2	code = JsonParseError2::NoError;
	int				line = 0;
	int				column = 0;
	String			message;
};

} // namespace tr
LN_NAMESPACE_END

