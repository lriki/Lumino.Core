/**
	@file	Console.h
*/
#pragma once

LN_NAMESPACE_BEGIN

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

LN_NAMESPACE_END
