#pragma once
#include <execinfo.h>

namespace Lumino
{

class SimleBackTrace
{
public:
	
	static int BackTrace(void** outCallStack, int count)
	{
		return ::backtrace(outCallStack, count);
	}
	
	// count は BackTrace の戻り値。
	static void MakeSymbolString(void** callStack, int count, char* outStr, int len)
	{
		char* pos = outStr;
		char* end = outStr + len;
		char** strs = ::backtrace_symbols(callStack, count);
		for (int i = 0; i < count; ++i)
		{
			pos += snprintf(pos, end - pos, "%s\n", strs[i]);
			if (pos >= end) { break; }
		}
		free(strs);
	}
};

} // namespace Lumino
