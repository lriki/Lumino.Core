/**
	@file	Console.h
*/
#pragma once

LN_NAMESPACE_BEGIN

/**
	@brief		�R���\�[���E�B���h�E�̑�����s���܂��B
*/
class Console
{
public:
	
	/**
		@brief		�R���\�[���E�B���h�E�����蓖�Ă܂��B
	*/
	static void Alloc();
	
	/**
		@brief		�R���\�[���E�B���h�E��������܂��B
	*/
	static void Free();
};

LN_NAMESPACE_END
