/**
	@file	BinaryReader.h
*/
#pragma once

#include "../Base/Common.h"
#include "../Base/Environment.h"

namespace Lumino
{
class Stream;

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
	*/
	int8_t ReadInt8();

	/**
		@brief		�����t�� 2 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 2 �o�C�g�i�߂܂��B
		@param[in]	dataByteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	int16_t ReadInt16(ByteOrder dataByteOrder = ByteOrder_Little);

	/**
		@brief		�����t�� 4 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 4 �o�C�g�i�߂܂��B
		@param[in]	dataByteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	int32_t ReadInt32(ByteOrder dataByteOrder = ByteOrder_Little);

	/**
		@brief		�����t�� 8 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 8 �o�C�g�i�߂܂��B
		@param[in]	dataByteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	int64_t ReadInt64(ByteOrder dataByteOrder = ByteOrder_Little);

	/**
		@brief		�������� 1 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 1 �o�C�g�i�߂܂��B
	*/
	uint8_t ReadUInt8();

	/**
		@brief		�������� 2 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 2 �o�C�g�i�߂܂��B
		@param[in]	dataByteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	uint16_t ReadUInt16(ByteOrder dataByteOrder = ByteOrder_Little);

	/**
		@brief		�������� 4 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 4 �o�C�g�i�߂܂��B
		@param[in]	dataByteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	uint32_t ReadUInt32(ByteOrder dataByteOrder = ByteOrder_Little);

	/**
		@brief		�������� 8 �o�C�g������ǂݎ��A�X�g���[���̌��݈ʒu�� 8 �o�C�g�i�߂܂��B
		@param[in]	dataByteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	uint64_t ReadUInt64(ByteOrder dataByteOrder = ByteOrder_Little);

	/**
		@brief		�w�肵���o�C�g���̕����t��������ǂݎ��A�X�g���[���̌��݈ʒu��i�߂܂��B
		@param[in]	byteCount	: �ǂݎ��o�C�g�� (1, 2, 4, 8�̂����ꂩ)
		@param[in]	dataByteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	int64_t ReadInt(int byteCount, ByteOrder dataByteOrder = ByteOrder_Little);

	/**
		@brief		�w�肵���o�C�g���̕�������������ǂݎ��A�X�g���[���̌��݈ʒu��i�߂܂��B
		@param[in]	byteCount	: �ǂݎ��o�C�g�� (1, 2, 4, 8�̂����ꂩ)
		@param[in]	dataByteOrder	: �ǂݎ��f�[�^�̃o�C�g�I�[�_�[
	*/
	uint64_t ReadUInt(int byteCount, ByteOrder dataByteOrder = ByteOrder_Little);

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
