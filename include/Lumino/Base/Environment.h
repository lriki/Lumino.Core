/**
	@file	Environment.h
*/

#pragma once

namespace Lumino
{

/**
	@brief	���݂̊��Ɋւ�����ɃA�N�Z�X���邽�߂̃N���X�ł��B
*/
class Environment
{
public:

	/**
		@brief	���g���G���f�B�A�����ł��邩���m�F���܂��B
	*/
	static bool IsLittleEndian();

	/**
		@brief		�V�X�e���N������̎��Ԃ� ms �P�ʂŎ擾���܂��B
	*/
	static uint64_t GetTickCount();

	/**
		@brief		�V�X�e���N������̎��Ԃ� ns �P�ʂŎ擾���܂��B
	*/
	static uint64_t GetTickCountNS();

	/**
		@brief		���݂̊��Œ�`����Ă�����s��������擾���܂��B
	*/
	template<typename TChar>
	static const TChar* GetNewLine();

};

} // namespace Lumino
