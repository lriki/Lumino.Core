/**
	@file	BinaryReader.h
*/
#pragma once

#include "../Base/Common.h"

namespace Lumino
{
class Stream;

/// �G���f�B�A���������l
enum ByteOrder
{
	ByteOrder_Little = 0,	///< ���g���G���f�B�A��
	ByteOrder_Big,			///< �r�b�O�G���f�B�A��
};

/**
	@brief	�X�g���[������o�C�i���f�[�^��ǂݎ��N���X�ł��B
*/
class BinaryReader
{
public:
	BinaryReader(Stream* stream);
	~BinaryReader();

public:

	/**
		@brief		�����t�� 1 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 1 �o�C�g�i�߂܂��B
		@param[in]	byteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	int8_t ReadInt8(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		�����t�� 2 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 2 �o�C�g�i�߂܂��B
		@param[in]	byteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	int16_t ReadInt16(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		�����t�� 4 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 4 �o�C�g�i�߂܂��B
		@param[in]	byteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	int32_t ReadInt32(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		�����t�� 8 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 8 �o�C�g�i�߂܂��B
		@param[in]	byteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	int64_t ReadUInt64(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		�������� 1 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 1 �o�C�g�i�߂܂��B
		@param[in]	byteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	uint8_t ReadIUInt8(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		�������� 2 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 2 �o�C�g�i�߂܂��B
		@param[in]	byteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	uint16_t ReadIUInt16(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		�������� 4 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 4 �o�C�g�i�߂܂��B
		@param[in]	byteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	uint32_t ReadIUInt32(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		�������� 8 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 8 �o�C�g�i�߂܂��B
		@param[in]	byteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	uint64_t ReadIUInt64(ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		�w�肵���o�C�g���̕����t��������ǂݎ��A�X�g���[���̌��݈ʒu��i�߂܂��B
		@param[in]	byteCount	: �ǂݎ��o�C�g�� (1, 2, 4, 8�̂����ꂩ)
		@param[in]	byteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	int64_t ReadInt(int byteCount, ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		�w�肵���o�C�g���̕�������������ǂݎ��A�X�g���[���̌��݈ʒu��i�߂܂��B
		@param[in]	byteCount	: �ǂݎ��o�C�g�� (1, 2, 4, 8�̂����ꂩ)
		@param[in]	byteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	uint64_t ReadUInt(int byteCount, ByteOrder byteOrder = ByteOrder_Little);

	/**
		@brief		�w�肵���o�C�g���̃f�[�^��ǂݎ��A�X�g���[���̌��݈ʒu��i�߂܂��B
	*/
	size_t Read(void* buffer, size_t count);

	/**
		@brief		�X�g���[���̌��݈ʒu���A�w�肵���I�t�Z�b�g�����i�߂܂��B
	*/
	void Seek(int offset);

	/**
		@brief		�X�g���[���̌��݈ʒu���擾���܂��B
	*/
	int64_t GetPosition();

	/**
		@brief		�t�@�C���I�[�ɓ��B���Ă��邩���m�F���܂��B
	*/
	bool IsEOF() const;

private:
	Stream*   m_stream;
};

} // namespace Lumino
