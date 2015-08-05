﻿/**
	@file	Array.h
*/
#pragma once
#include <vector>
#include <algorithm>
#include "Exception.h"
#include "STLUtils.h"

namespace Lumino
{

/**
	@brief		動的配列のテンプレートクラスです。
	@details	隣接するメモリ位置に要素を格納し、高速なランダムアクセスを行うことができます。
				内部では、メモリ使用量を削減し、データの不必要なコピーを避けるためにコピーオンライト(COW)の共有を行います。
*/
template<typename T, typename TAllocator = STLAllocator<T> >
class Array
{
public:
	typedef typename std::vector<T, TAllocator>		std_vector;
	typedef typename std_vector::value_type			value_type;
	typedef typename std_vector::difference_type	difference_type;
	typedef typename std_vector::pointer			pointer;
	typedef typename std_vector::const_pointer		const_pointer;
	typedef typename std_vector::iterator			iterator;
	typedef typename std_vector::const_iterator		const_iterator;
	typedef typename std_vector::reference			reference;
	typedef typename std_vector::const_reference	const_reference;

public:

	/** 空の配列で新しい配列を作成します。*/
	Array();

	/** 配列をコピーして新しい配列を作成します。*/
	Array(const Array& ary);

	/** 別の配列からムーブして作成します。*/
	Array(Array&& ary);

	/** デストラクタ */
	~Array();

public:

	/** 配列が空であるかを確認します。*/
	bool IsEmpty() const
	{
		return m_data->m_vector.empty();
	}

	/** 格納されている要素の数を取得します。*/
	int GetCount() const
	{
		return (int)m_data->m_vector.size();
	}

	/** 追加のメモリ割り当てを行わずに追加できる要素の最大数を取得します。*/
	int GetCapacity() const
	{ 
		return m_data->m_vector.capacity();
	}

	/** 末尾に要素を追加します。*/
	void Add(const T& item)
	{
		CheckDetachShared();
		m_data->m_vector.push_back(item);
	}

	/** 末尾に別の配列を連結します。*/
	void Add(const Array<T>& items)
	{
		CheckDetachShared();
		m_data->m_vector.insert(m_data->m_vector.end(), items.m_data->m_vector.begin(), items.m_data->m_vector.end());
	}

	/** 指定したインデックスの位置に要素を挿入します。*/
	void Insert(int index, const T& item)
	{
		LN_THROW(0 <= index && index <= GetCount(), OutOfRangeException);	// Count と同じインデックスを指定できる
		CheckDetachShared();
		m_data->m_vector.insert(m_data->m_vector.begin() + index, item);
	}

	/** 全ての要素を削除します。*/
	void Clear()
	{
		CheckDetachShared();
		m_data->m_vector.clear();
	}

	/** item に一致する最初の要素を削除します。(正常に削除された場合は true を返す。要素が見つからなければ false を返す)*/
	bool Remove(const T& item)
	{
		CheckDetachShared();
		return STLUtils::Remove(m_data->m_vector, item);
	}

	/** 指定したインデックスにある要素を削除します。*/
	void RemoveAt(int index)
	{
		CheckOutOfRange(index);
		CheckDetachShared();
		m_data->m_vector.erase(m_data->m_vector.begin() + index);
	}

	/** item に一致する全ての要素を削除します。*/
	void RemoveAll(const T& item)
	{
		CheckDetachShared();
		STLUtils::RemoveAll(m_data->m_vector, item);
	}

	/** 終端要素を削除します。*/
	void RemoveLast()
	{
		CheckOutOfRange(GetCount() - 1);
		CheckDetachShared();
		RemoveAt(GetCount() - 1);
	}

	/** 配列用のメモリを指定したサイズで確保します。*/
	void Reserve(int size)
	{
		LN_CHECK_ARGS_RETURN(size >= 0);
		CheckDetachShared();
		m_data->m_vector.reserve(size);
	}

	/** 配列の要素数を変更します。*/
	void Resize(int size)
	{
		LN_CHECK_ARGS_RETURN(size >= 0);
		CheckDetachShared();
		m_data->m_vector.resize(size);
	}

	/** 指定した要素がこの配列内に存在するかどうかを判断します。*/
	bool Contains(const T& item) const
	{
		return std::find(m_data->m_vector.begin(), m_data->m_vector.end(), item) != m_data->m_vector.end();
	}

	/**
		@brief		要素を指定した位置から検索し、最初に見つかったインデックスを返します。
		@param[in]	item		: 検索する要素
		@param[in]	startIndex	: 検索を開始するインデックス (省略した場合は先頭から)
		@return		検索した要素が最初に現れた位置。見つからなかった場合は -1。
	*/
	int IndexOf(const T& item, int startIndex = 0) const
	{
		if (IsEmpty()) { return -1; }
		CheckOutOfRange(startIndex);
		const_iterator itr = std::find(m_data->m_vector.begin() + startIndex, m_data->m_vector.end(), item);
		if (itr != m_data->m_vector.end()) { return itr - m_data->m_vector.begin(); }
		return -1;
	}

	/** 指定したインデックスに要素を設定します。*/
	void SetAt(int index, const T& item)
	{
		CheckOutOfRange(index);
		CheckDetachShared();
		m_data->m_vector.at(index) = item;
	}

	/** 指定したインデックスにある要素への参照を取得します。*/
	reference GetAt(int index)
	{
		CheckOutOfRange(index);
		CheckDetachShared();
		return m_data->m_vector.at(index);
	}

	/** 指定したインデックスにある要素への参照を取得します。*/
	const_reference GetAt(int index) const
	{
		CheckOutOfRange(index);
		return m_data->m_vector.at(index);
	}

	/** 先頭要素の参照を返します。*/
	T& GetFront()
	{
		LN_CHECK_STATE(!IsEmpty());
		CheckDetachShared();
		return m_data->m_vector.front();
	}

	/** 先頭要素の参照を返します。*/
	const T& GetFront() const
	{
		LN_CHECK_STATE(!IsEmpty());
		return m_data->m_vector.front();
	}

	/** 終端要素の参照を返します。*/
	T& GetLast()
	{
		LN_CHECK_STATE(!IsEmpty());
		CheckDetachShared();
		return m_data->m_vector.back();
	}

	/** 終端要素の参照を返します。*/
	const T& GetLast() const
	{
		LN_CHECK_STATE(!IsEmpty());
		return m_data->m_vector.back();
	}

	/** 別の配列をこの配列に上書きコピーします。*/
	void CopyFrom(const Array<T>& ary)
	{
		CheckDetachShared();
		m_data->m_vector = ary.m_data->m_vector;
	}

	/** 指定したインデックスがこの配列の境界の範囲外かを確認します。*/
	bool IsOutOfRange(int index) const
	{
		return (index < 0 || GetCount() <= index);
	}

	/** 別の配列をこの配列に割り当てます。*/
	Array& operator=(const Array& ary);

	/** 指定したインデックスにある要素への参照を取得します。*/
	T& operator[] (int index);

	/** 指定したインデックスにある要素への参照を取得します。*/
	const T& operator[] (int index) const;

	/*-----------------------------------------------------------------------*/
	/** @name STL interface */
	/** @{ */

	iterator		begin()			{ CheckDetachShared(); return m_data->m_vector.begin(); }
	const_iterator	begin() const	{ return m_data->m_vector.begin(); }
	iterator		end()			{ CheckDetachShared(); return m_data->m_vector.end(); }
	const_iterator	end() const		{ return m_data->m_vector.end(); }
	iterator		erase(iterator pos) { CheckDetachShared(); return m_data->m_vector.erase(pos); }
	iterator		erase(iterator begin, iterator end) { CheckDetachShared(); return m_data->m_vector.erase(begin, end); }

	/** @} */

private:
	void CheckDetachShared();
	void CheckOutOfRange(int index) const;

private:
	struct ArrayData
	{
		int			m_refCount;
		std_vector	m_vector;
		ArrayData(int refCount) : m_refCount(refCount) {}
		ArrayData(const std_vector& vec) : m_refCount(1), m_vector(vec) {}
		inline bool IsShared() const { return (m_refCount > 1); }
		inline void AddRef() { ++m_refCount; }
		inline void Release();
		static ArrayData* GetSharedEmpty();
	};

	ArrayData*	m_data;
};

} // namespace Lumino

#include "Array.inl"
