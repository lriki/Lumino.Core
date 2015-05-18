
#pragma once

#include <Lumino/Base/String.h>

namespace Lumino
{
namespace Json
{

/// JSON 解析のエラーコード
enum ParseError
{
	ParseError_NoError = 0,					///< エラーは発生していない
	ParseError_DocumentEmpty,				///< 入力文字列が空だった
	ParseError_DocumentRootNotSingular,		///< 複数のルート要素が見つかった
	ParseError_ValueInvalid,				///< 無効な値

	ParseError_Termination,					///< Hander で中断された
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
