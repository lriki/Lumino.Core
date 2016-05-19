﻿
#pragma once
#include "../Base/Exception.h"

LN_NAMESPACE_BEGIN

/**
	@brief	スレッド実行中にハンドルされない例外(このライブラリ以外の例外)が発生した
*/
class LUMINO_EXPORT ThreadException 
	: public Exception
{
public:
	ThreadException();
	virtual ~ThreadException() throw();

public:
	// override Exception
	virtual Exception* Copy() const { return LN_NEW ThreadException( *this ); }
};

LN_NAMESPACE_END
