/**
	@file	Hash.h
*/
#pragma once

namespace Lumino
{

/**
	@brief		�n�b�V���l�̌v�Z���s���N���X�ł��B
*/
class Hash
{
public:

	/**
		@brief		�w�肵��������̃n�b�V���l�����߂܂��B
	*/
	static uint32_t CalcHash(const char* str, int len = -1);
	static uint32_t CalcHash(const wchar_t* str, int len = -1);
};

} // namespace Lumino
