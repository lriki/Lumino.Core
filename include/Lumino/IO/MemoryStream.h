
#pragma once

#include <vector>
#include "Stream.h"

namespace Lumino
{
/**
	@brief	�f�[�^�̓ǂݏ�����Ƃ��ă��������g�p����X�g���[���ł��B
*/
class MemoryStream
    : public Stream
{
public:
	/**
		@brief		�g���\�ȃ������X�g���[�����쐬���܂��B
		@details	���̃R���X�g���N�^�̌�A�K�v�ɉ����� Create() ���Ăяo���čď��������邱�Ƃ��ł��܂��B
	*/
	MemoryStream();

	~MemoryStream();

public:

	/**
		@brief	�L���p�V�e�B���w�肵�āA�g���\�ȃ������X�g���[�����쐬���܂��B
		@param	size	: �T�C�Y(�o�C�g��)
	*/
	void Create(size_t size);

	/**
		@brief		�����̃o�b�t�@���w�肵�āA�T�C�Y�ύX�ł��Ȃ��������X�g���[�����쐬���܂��B
		@param		buffer	: �o�b�t�@�̐擪�A�h���X
		@param		size	: �T�C�Y(�o�C�g��)
		@details	buffer �Ɏw�肵���o�b�t�@�́A���̃N���X�̃C���X�^���X�����݂���Ԃ͊J�����Ă͂����܂���B
	*/
	void Create(void* buffer, size_t size);

	/**
		@brief		�o�C�g�z��̐擪�A�h���X���擾���܂��B
	*/
	void* GetBuffer();

public:
	// override CHLDStream
	virtual bool CanRead() { return true; }
	virtual bool CanWrite() { return true; }
	virtual size_t GetSize() { return m_buffer.size(); }
	virtual size_t Read(void* buffer, size_t bufferSize, size_t byteCount) { LN_THROW(0, NotImplementedException); return 0; }
	virtual void Write(const void* pData, size_t nByteCount);
	virtual void Flush() {}		// Write �����ڃ������ɏ������ނ̂ŕs�v

private:
	std::vector<byte_t>	m_buffer;			///< �ϒ��̏ꍇ�͂��̃o�b�t�@���g��
	size_t				m_seekPos;
	
	void*				m_fixedBuffer;		///< �Œ蒷�̏ꍇ�͂��̃o�b�t�@���g��
	size_t				m_fixedBufferSize;
};

} // namespace Lumino
