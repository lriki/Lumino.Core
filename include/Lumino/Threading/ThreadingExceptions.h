
#pragma once

#include "../Base/Common.h"
#include "../Base/Exception.h"
#include "../Base/NonCopyable.h"

namespace Lumino
{
namespace Threading
{

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

} // namespace Threading
} // namespace Lumino
