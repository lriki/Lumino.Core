
#pragma once

#include "Common.h"

LN_NAMESPACE_BEGIN

/**
	@brief	コピーを禁止するクラスに private 継承させる
*/
class LUMINO_EXPORT NonCopyable
{
protected:
    NonCopyable() {}
    virtual ~NonCopyable() {}

private:
	NonCopyable(const NonCopyable &);
	NonCopyable& operator=(const NonCopyable &);
};

LN_NAMESPACE_END
