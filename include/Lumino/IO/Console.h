/**
	@file	Console.h
*/
#pragma once

namespace Lumino
{

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

} // namespace Lumino
