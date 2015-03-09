
#pragma once

#include "Common.h"
#include "RefObject.h"

namespace Lumino
{

/**
	@brief	参照カウント付のメモリバッファ (バイト配列) を表すクラスです。
*/
class ByteBuffer
	: public RefObject
{
public:

	/**
		@brief		サイズ 0 として初期化します。
		@details	実際にメモリ確保は行いません。
					別途、Reserve() 等により確保する必要があります。
	*/
	ByteBuffer();

	/**
		@brief		指定されたバイト数でバッファを確保します。
		@param[in]	size		: バイト数
		@param[in]	zeroClear	: true の場合、確保したバッファ全体を 0 でクリアする
	*/
	ByteBuffer(size_t size, bool zeroClear = true);

	/**
		@brief		指定されたバッファの内容をコピーし、バッファを構築します。
		@param[in]	data		: コピー元のバッファ
		@param[in]	size		: コピーするバイト数
	*/
	ByteBuffer(const void* data, size_t size);

	/**
		@brief		指定されたバッファの内容をコピーし、バッファを構築します。
		@param[in]	data		: コピー元のバッファ
		@param[in]	size		: コピーするバイト数
		@param[in]	refMode		: (解説参照)
		@details	size 分のメモリを確保し、data の示すバッファからデータをコピーします。
					ただし、refMode が true の場合はメモリ確保やコピーを行わず、
					data のバッファを参照するだけになり、Clear() や Copy() 等の書き込み操作は
					このバッファに直接書き込まれます。
					また、このクラスは data のバッファを開放しません。
		@code
					// refMode の使用例
					char* data = new char[5];
					ByteBuffer buf(data, 5, true);
					buf.Clear();
					buf.Copy("abc", 3);
					printf(data);		// => "abc"
					delete[] data;
		@endcode
	*/
	ByteBuffer(void* data, size_t size, bool refMode);

	/**
		@brief		指定された NULL 終端文字列をコピーしてバッファを構築します。
		@param[in]	str			: NULL 終端文字列
		@details	終端の NULL 文字はコピーされません。
	*/
	ByteBuffer(const char* str);
	ByteBuffer(const wchar_t* str);

	virtual ~ByteBuffer();

public:

	/**
		@brief		バッファの再割り当てを行います。
		@param[in]	size		: バッファのバイト数
	*/
	void Alloc(size_t size, bool zeroClear = true);

	/**
		@brief		バッファの再割り当てを行い、指定されたデータをコピーします。
		@param[in]	data		: コピー元のデータ
		@param[in]	size		: コピー元のデータのバイト数
	*/
	void Alloc(const void* data, size_t size);

	/**
		@brief		バッファのサイズを変更します。
		@param[in]	size		: 新しいサイズ (バイト数)
		@param[in]	zeroClear	: 伸長する場合、 0 でクリアを行うか
		@details	再割り当て時には元の領域から新しい領域へ内容をコピーします。
					その際、縮退する場合は末尾の余剰データは切り捨てられ、
					伸長する場合は zeroClear 引数に従い 0 クリアを行います。
	*/
	void Resize(size_t size, bool zeroClear = true);

	/**
		@brief		指定したメモリアドレスをデータストアとして参照します。
		@param[in]	data		: 参照先アドレス
		@param[in]	size		: 参照先データのバイト数
	*/
	void SetReferenceBuffer(void* buffer, size_t size);

	/**
		@brief		指定したバッファからこのバッファにデータをコピーします。
		@param[in]	data		: コピー元バッファ
		@param[in]	size		: コピー元バッファのバイト数
	*/
	void Copy(const void* data, size_t size);

	/**
		@brief		指定したコピー先位置に、指定したバッファからこのバッファにデータをコピーします。
		@param[in]	offset		: コピー先の先頭インデックス
		@param[in]	data		: コピー元バッファ
		@param[in]	size		: コピー元バッファのバイト数
	*/
	void Copy(size_t offset, const void* data, size_t size);

	/**
		@brief		バッファ全体を 0 でクリアします。
	*/
	void Clear();

	/**
		@brief		バッファの先頭アドレスを取得します。
	*/
	void* GetData() { return m_buffer; }
	const void* GetData() const { return m_buffer; }
	
	/**
		@brief		バッファのサイズ (バイト数) を取得します。
	*/
	size_t GetSize() const { return m_size; }
	

public:
	byte_t& operator[] (size_t index);

private:
	void Dispose();

private:
	byte_t*		m_buffer;
	size_t		m_capacity;
	size_t		m_size;
	bool		m_refMode;
};

} // namespace Lumino
