﻿
#pragma once

#include <vector>
#include "STLUtils.h"
#include "RefObject.h"

namespace Lumino
{

/**
	@brief	動的配列 (デフォルトのアロケータにカスタムアロケータを指定)
*/
template<typename T, typename TAllocator = STLAllocator<T> >
class Array : public std::vector<T, TAllocator >
{
public:
	typedef std::vector<T, TAllocator> std_vector;

public:
	/// 配列のインデックスとして有効な整数値であるかを確認する
	bool CheckValidIndex(int index) const { return (0 <= index && index < GetCount()); }
	
	/// 要素数を取得する
	int GetCount() const { return (int)std_vector::size(); }

	/// 末尾に要素を追加する
	void Add(const T& value) { std_vector::push_back(value); }

	/// 終端要素の参照を返す
	T& GetLast() { return *(std_vector::rbegin()); }

	/// 終端要素の参照を返す
	const T& GetLast() const { return *(std_vector::rbegin()); }

	/// item に一致する最初の要素を削除する
	void Remove(const T& item) { STLUtils::Remove(*this, item); }

	/// item に一致する全ての要素を削除する
	void RemoveAll(const T& item) { STLUtils::RemoveAll(*this, item); }

	/// ある要素がこの配列内に存在するかどうかを判断します。
	bool Contains(const T& item) const { return std::find(std_vector::begin(), std_vector::end(), item) != std_vector::end(); }
};

/**
	@brief	参照カウントを持つ動的配列のクラスです。
*/
template<typename T, typename TAllocator = STLAllocator<T> >
class ArrayList : public RefObject
{
public:
	typedef typename std::vector<T, TAllocator>		std_vector;
	typedef typename std_vector::iterator			iterator;
	typedef typename std_vector::const_iterator		const_iterator;
	typedef typename std_vector::reference			reference;
	typedef typename std_vector::const_reference	const_reference;

public:

	ArrayList() {}

	ArrayList(const ArrayList<T, TAllocator>& ary) : m_vector(ary.m_vector) { }
	
public:

	/// 配列が空であるかを判定します。
	bool IsEmpty() const { return m_vector.empty(); }

	/// 保持できる要素数を設定します。
	void Reserve(int size) { m_vector.reserve(size); }

	/// 配列の要素数を変更します。
	void Resize(int size) { m_vector.resize(size); }

	/// 末尾に要素を追加します。
	void Add(const T& item) { m_vector.push_back(item); }

	/// 末尾に別の配列を連結します。
	void Add(const ArrayList<T>& items) { m_vector.insert(m_vector.end(), items.m_vector.begin(), items.m_vector.end()); }

	/// 指定したインデックスの位置に要素を挿入します。
	void Insert(int index, const T& item) { m_vector.insert(m_vector.begin() + index, item); }

	/// 全ての要素を削除します。
	void Clear() { m_vector.clear(); }

	/// item に一致する最初の要素を削除します。
	void Remove(const T& item) { STLUtils::Remove(m_vector, item); }

	/// 指定したインデックスにある要素を削除します。
	void RemoveAt(int index) { m_vector.erase(m_vector.begin() + index); }

	/// item に一致する全ての要素を削除します。
	void RemoveAll(const T& item) { STLUtils::RemoveAll(m_vector, item); }

	/// ある要素がこの配列内に存在するかどうかを判断します。
	bool Contains(const T& item) const { return std::find(m_vector.begin(), m_vector.end(), item) != m_vector.end(); }

	/// 格納されている要素数を取得します。
	int GetCount() const { return (int)m_vector.size(); }

	/// 指定したインデックスに要素を設定します。
	void SetAt(int index, const T& item) { m_vector.at(index) = item; }

	/// 指定したインデックスにある要素への参照を取得します。
	reference GetAt(int index) { return m_vector.at(index); }				// VC++ や GCC では bool は特殊化され、T& には変換できない。reference で返さないとコンパイルエラーとなる
	const_reference GetAt(int index) const { return m_vector.at(index); }

	/// 終端要素の参照を返します。
	T& GetLast() { return *(m_vector.rbegin()); }
	const T& GetLast() const { return *(m_vector.rbegin()); }

	/// 別の配列をこの配列に上書きコピーします。
	void CopyFrom(const ArrayList<T>& ary) { m_vector = ary.m_vector; }

public:
	T& operator[] (int index) { return m_vector[index]; }
	const T& operator[] (int index) const { return m_vector[index]; }

public:
	// STL interface
	iterator		begin()			{ return m_vector.begin(); }
	const_iterator	begin() const	{ return m_vector.begin(); }
	iterator		end()			{ return m_vector.end(); }
	const_iterator	end() const		{ return m_vector.end(); }
	iterator		erase(iterator pos) { return m_vector.erase(pos); }
	iterator		erase(iterator begin, iterator end) { return m_vector.erase(begin, end); }

private:
	std_vector	m_vector;
};

/**
	@brief		キーの "operator <" の実装により値を並べ替える動的配列
*/
template<typename TKey, typename TValue, typename TAllocator = STLAllocator< std::pair<TKey, TValue> > >
class SortedArray
{
public:

	/// 末尾に要素を追加する
	void Add(const TKey& key, const TValue& value);

	/// item に一致する最初の要素を削除する
	void Remove(const TValue& item);

public:

	typedef typename std::pair<TKey, TValue> Entry;
	typedef typename std::vector<Entry, TAllocator> InternalArray;
	typedef typename InternalArray::iterator Internal_iterator;
	typedef typename InternalArray::const_iterator Internal_const_iterator;


	class iterator
	{
	public:
		iterator() { mPos = NULL; }
		iterator(const iterator& obj) { mPos = obj.mPos; }
		iterator(Internal_iterator pos) { mPos = pos; }
		TValue& operator*() const { return mPos->second; }
		TValue* operator->() const { return &mPos->second;  }
		iterator& operator++() { ++mPos; return(*this); }
		iterator operator++(int) { iterator t = *this; ++(*this); return t; }
		iterator& operator--() { --mPos; return(*this); }
		iterator operator--(int) { iterator t = *this; --(*this); return t; }
		bool operator==(const iterator& right) const { return (mPos == right.mPos); }
		bool operator!=(const iterator& right) const { return (!(*this == right)); }

	private:
		Internal_iterator	mPos;
	};

	class const_iterator
	{
	public:
		const_iterator() { mPos = NULL;  }
		const_iterator(const const_iterator& obj) { mPos = obj.mPos; }
		const_iterator(Internal_const_iterator pos) { mPos = pos; }
		const TValue& operator*() const { return mPos->second; }
		const TValue* operator->() const { return &mPos->second; }
		iterator& operator++() { ++mPos; return(*this); }
		iterator operator++(int) { iterator t = *this; ++(*this); return t; }
		iterator& operator--() { --mPos; return(*this); }
		iterator operator--(int) { iterator t = *this; --(*this); return t; }
		bool operator==(const iterator& right) const { return (mPos == right.mPos); }
		bool operator!=(const iterator& right) const { return (!(*this == right)); }

	private:
		Internal_const_iterator	mPos;
	};

	iterator begin() { return mArray.begin(); }
	iterator end() { return mArray.end(); }

private:
	std::vector<Entry, TAllocator>	mArray;
};


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TKey, typename TValue, typename TAllocator>
inline void SortedArray<TKey, TValue, TAllocator>::Add(const TKey& key, const TValue& value)
{
	struct Cmp
	{
		static bool CmpEventListener(const Entry& left, const Entry& right)
		{
			return left.first < right.first;
		}
	};

	// 追加後、ソートを掛ける
	Entry e;
	e.first = key;
	e.second = value;
	mArray.push_back(e);
	std::stable_sort(mArray.begin(), mArray.end(), Cmp::CmpEventListener);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TKey, typename TValue, typename TAllocator>
inline void SortedArray<TKey, TValue, TAllocator>::Remove(const TValue& item)
{
	typename InternalArray::iterator itr = mArray.begin();
	typename InternalArray::iterator end = mArray.end();
	for (; itr != end; ++itr)
	{
		if (itr->second == item) {
			mArray.erase(itr);
			return;
		}
	}
}

} // namespace Lumino
