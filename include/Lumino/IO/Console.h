/**
	@file	Console.h
*/
#pragma once

namespace Lumino
{

/**
	@brief		コンソールウィンドウの操作を行います。
*/
class Console
{
public:
	
	/**
		@brief		コンソールウィンドウを割り当てます。
	*/
	static void Alloc();
	
	/**
		@brief		コンソールウィンドウを解放します。
	*/
	static void Free();
};

} // namespace Lumino
