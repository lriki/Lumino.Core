#pragma once

#include <Base/Common.h>
#include <Base/Exception.h>

#if defined(LN_GNUC) || defined(__CYGWIN__)
#include <unistd.h>
//#include <Base/CRT.h>
#endif

namespace Lumino
{
class ImplAccessor
{
public:

	template<typename T, typename TImpl>
	static TImpl* GetImpl(T* frontEnd)
	{
		return frontEnd->mImpl;
	}
};

#define LN_GET_IMPL(frontEndType, ptr) ImplAccessor::GetImpl<frontEndType, frontEndType##Impl>(ptr);

} // namespace Lumino
