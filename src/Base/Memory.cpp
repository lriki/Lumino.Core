
#include <new>
#include "../Internal.h"
#include "../../include/Lumino/Base/Memory.h"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void* LN_CDECL operator new ( size_t size, LNMemoryFlag flag )
{
	void* ptr = NULL;
	try
	{
		ptr = ::operator new(size);
	}
	catch (std::bad_alloc)
	{
	}
	if (ptr == NULL) {
		LN_THROW(0, Lumino::OutOfMemoryException);
	}
	return ptr;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void* LN_CDECL operator new[] ( size_t size, LNMemoryFlag flag )
{
	void* ptr = NULL;
	try
	{
		ptr = ::operator new[](size);
	}
	catch (std::bad_alloc)
	{
	}
	if (ptr == NULL) {
		LN_THROW(0, Lumino::OutOfMemoryException);
	}
	return ptr;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void  LN_CDECL operator delete ( void* ptr, LNMemoryFlag flag )
{
	return ::operator delete(ptr);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void  LN_CDECL operator delete[] ( void* ptr, LNMemoryFlag flag )
{
	return ::operator delete[](ptr);
}

