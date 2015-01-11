
#pragma once

#include "../Base/Common.h"
#include "../Base/Exception.h"
#include "../Base/NonCopyable.h"

namespace Lumino
{
namespace Threading
{

/**
	@brief	�X���b�h���s���Ƀn���h������Ȃ���O(���̃��C�u�����ȊO�̗�O)����������
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
