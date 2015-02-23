
#pragma once

#include "RefObject.h"

namespace Lumino
{

/**
	@brief	参照カウント付のメモリバッファ
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

	/// メモリ割り当て
	void Reserve(size_t size)
	{
		Dispose();
		mBuffer = LN_NEW byte_t[size];
		mSize = size;
		mAutoDelete = true;
	}

	/// メモリ割り当て + コピー
	void Reserve(const byte_t* data, size_t size)
	{
		Dispose();
		mBuffer = LN_NEW byte_t[size];
		mSize = size;
		mAutoDelete = true;
		memcpy(mBuffer, data, mSize);
	}

	/// コピー
	void Copy(const byte_t* data, size_t size)
	{
		//mSize = size;
		memcpy(mBuffer, data, size);
	}

	/// コピー
	void Copy(size_t uOffset, const byte_t* data, size_t size)
	{
		memcpy(mBuffer + uOffset, data, size);
	}

	/// 0クリア
	void Clear()
	{
		if (mBuffer) {
			memset(mBuffer, 0, mSize);
		}
	}

	/// アドレス取得
	byte_t* GetPointer() const
	{
		return mBuffer;
	}

	/// サイズ取得
	size_t GetSize() const
	{
		return mSize;
	}

	/// サイズ変更
	void Resize(size_t newSize)
	{
		assert(newSize <= mSize);	// 縮小方向のみとりあえず許可
		mSize = newSize;
	}

	/// ラッピング
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
