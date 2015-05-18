
#pragma once

#include <Lumino/Base/String.h>

namespace Lumino
{
namespace Json
{

/// JSON 解析のエラーコード
enum ParseError
{
	ParseError_NoError = 0,						///< エラーは発生していない
	ParseError_DocumentEmpty,					///< 入力文字列が空だった
	ParseError_DocumentRootNotSingular,			///< 複数のルート要素が見つかった
	ParseError_ValueInvalid,					///< 無効な値
	ParseError_StringEscapeInvalid,				///< 無効なエスケープシーケンス
	ParseError_StringMissQuotationMark,			///< 文字列の開始と終端が一致しなかった (" が少ない)
	ParseError_StringInvalidEncoding,			///< 変換できない文字が存在した
	ParseError_ArrayMissCommaOrSquareBracket,	///< 配列の要素の後に , か ] が無かった
	ParseError_ObjectMissKeyStart,				///< オブジェクトメンバの名前の開始 " が見つからなかった
	ParseError_ObjectMissColon,					///< オブジェクトメンバの : が見つからなかった
	ParseError_ObjectMissCommaOrCurlyBracket,	///< オブジェクトメンバの後に , か } が見つからなかった

	ParseError_Termination,						///< Hander で中断された
};

/*
	@brief	JSON 解析中のエラーを表します。
*/
class JsonError
{
public:
	void SetError(ParseError errorCode, int offset) { ErrorCode = errorCode; Offset = offset; }

public:
	ParseError	ErrorCode;
	int			Offset;
	String		Message;
};

} // namespace Json
} // namespace Lumino
