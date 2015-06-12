/*
 *	new に失敗した時は必ず OutOfMemoryException 例外を発生させます。
 *	戻り値を NULL チェックする必要はありません。
 *	使い方は普通の new と同じです。
 *	例)
 *		A* a = LN_NEW A(1, 2);
 */

#pragma once

namespace Lumino
{
	enum MemoryFlag
	{
		LN_MM_NORMAL_BLOCK = 1,
	};
}

//void* LN_CDECL operator new ( size_t size );
//void  LN_CDECL operator delete ( void* ptr );

void* LN_CDECL operator new (size_t size, ::Lumino::MemoryFlag flag);
void* LN_CDECL operator new[](size_t size, ::Lumino::MemoryFlag flag);
void  LN_CDECL operator delete (void* ptr, ::Lumino::MemoryFlag flag);
void  LN_CDECL operator delete[](void* ptr, ::Lumino::MemoryFlag flag);

#define LN_NEW						new(::Lumino::LN_MM_NORMAL_BLOCK)
#define LN_OPERATOR_NEW(size)		::operator new(size,::Lumino:: LN_MM_NORMAL_BLOCK)
#define LN_OPERATOR_DELETE(ptr) {	::operator delete(ptr); (ptr) = 0; }
