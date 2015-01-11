
#pragma once

#include "RefObject.h"

namespace Lumino
{

/**
	@brief	�Q�ƃJ�E���g�t�̃������o�b�t�@
*/
class RefBuffer
	: public RefObject
{
public:

	RefBuffer()
		: mBuffer(NULL)
		, mSize(0)
		, mAutoDelete(false)
	{}

	RefBuffer(size_t size)
		: mBuffer(NULL)
		, mSize(0)
		, mAutoDelete(true)
	{
		Reserve(size);
	}

	RefBuffer(byte_t* data, size_t size, bool autoDelete)
		: mBuffer(data)
		, mSize(size)
		, mAutoDelete(autoDelete)
	{}

	virtual ~RefBuffer()
	{
		Dispose();
	}

public:

	/// ���������蓖��
	void Reserve(size_t size)
	{
		Dispose();
		mBuffer = LN_NEW byte_t[size];
		mSize = size;
		mAutoDelete = true;
	}

	/// ���������蓖�� + �R�s�[
	void Reserve(const byte_t* data, size_t size)
	{
		Dispose();
		mBuffer = LN_NEW byte_t[size];
		mSize = size;
		mAutoDelete = true;
		memcpy(mBuffer, data, mSize);
	}

	/// �R�s�[
	void Copy(const byte_t* data, size_t size)
	{
		//mSize = size;
		memcpy(mBuffer, data, size);
	}

	/// �R�s�[
	void Copy(size_t uOffset, const byte_t* data, size_t size)
	{
		memcpy(mBuffer + uOffset, data, size);
	}

	/// 0�N���A
	void Clear()
	{
		if (mBuffer) {
			memset(mBuffer, 0, mSize);
		}
	}

	/// �A�h���X�擾
	byte_t* GetPointer() const
	{
		return mBuffer;
	}

	/// �T�C�Y�擾
	size_t GetSize() const
	{
		return mSize;
	}

	/// �T�C�Y�ύX
	void Resize(size_t newSize)
	{
		assert(newSize <= mSize);	// �k�������݂̂Ƃ肠��������
		mSize = newSize;
	}

	/// ���b�s���O
	void setBytesBuffer(byte_t* data, size_t size, bool autoDelete)
	{
		Dispose();
		mBuffer = data;
		mSize = size;
		mAutoDelete = autoDelete;
	}

	byte_t& operator[] (size_t index)
	{
		return mBuffer[index];
	}

private:

	void Dispose()
	{
		if (mAutoDelete) {
			LN_SAFE_DELETE(mBuffer);
			mSize = 0;
			mAutoDelete = false;
		}
	}

private:

	byte_t*		mBuffer;
	size_t		mSize;
	bool		mAutoDelete;
};

} // namespace Lumino
