
#pragma once

#include "../Base/Common.h"

namespace Lumino
{

/**
	@brief	���݂̊��E�v���b�g�t�H�[���Ɋւ�����
*/
class Environment
{
public:

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
