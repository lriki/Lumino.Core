
#pragma once

#include <vector>
#include <algorithm>
#include "Exception.h"
#include "STLUtils.h"
#include "RefObject.h"

namespace Lumino
{

/**
	@brief		動的配列のテンプレートクラスです。
	@details	隣接するメモリ位置に要素を格納し、高速なランダムアクセスを行うことができます。
				(現在は内部的に std::vector で実装されています)
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

	Array() {}

	Array(const Array<T, TAllocator>& ary) : m_vector(ary.m_vector) { }
	
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
	void Add(const Array<T>& items) { m_vector.insert(m_vector.end(), items.m_vector.begin(), items.m_vector.end()); }

	/// 指定したインデックスの位置に要素を挿入します。
	void Insert(int index, const T& item) { m_vector.insert(m_vector.begin() + index, item); }

	/// 全ての要素を削除します。
	void Clear() { m_vector.clear(); }

	/// item に一致する最初の要素を削除します。(正常に削除された場合は true を返す。要素が見つからなければ false を返す)
	bool Remove(const T& item) { return STLUtils::Remove(m_vector, item); }

	/// 指定したインデックスにある要素を削除します。
	void RemoveAt(int index) { m_vector.erase(m_vector.begin() + index); }

	/// item に一致する全ての要素を削除します。
	void RemoveAll(const T& item) { STLUtils::RemoveAll(m_vector, item); }

	/// 終端要素を削除します。
	void RemoveLast() { RemoveAt(GetCount() - 1); }

	/// ある要素がこの配列内に存在するかどうかを判断します。
	bool Contains(const T& item) const { return std::find(m_vector.begin(), m_vector.end(), item) != m_vector.end(); }

	/// 格納されている要素数を取得します。
	int GetCount() const { return (int)m_vector.size(); }

	/// 指定したインデックスに要素を設定します。
	void SetAt(int index, const T& item) { m_vector.at(index) = item; }

	/// 指定したインデックスにある要素への参照を取得します。
	reference GetAt(int index) { return m_vector.at(index); }				// VC++ や GCC では bool は特殊化され、T& には変換できない。reference で返さないとコンパイルエラーとなる
	const_reference GetAt(int index) const { return m_vector.at(index); }

	/// 先頭要素の参照を返します。
	T& GetFront() { return m_vector.front(); }
	const T& GetFront() const { return m_vector.front(); }

	/// 終端要素の参照を返します。
	T& GetLast() { return *(m_vector.rbegin()); }
	const T& GetLast() const { return *(m_vector.rbegin()); }

	/// 別の配列をこの配列に上書きコピーします。
	void CopyFrom(const Array<T>& ary) { m_vector = ary.m_vector; }

	/// 追加のメモリ割り当てを行わずに追加できる要素の最大数を取得します。
	int GetCapacity() const { return m_vector.capacity(); }

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
	typedef typename std::pair<TKey, TValue> Pair;
	typedef typename std::vector<Pair, TAllocator> InternalArray;
	typedef typename InternalArray::iterator Internal_iterator;
	typedef typename InternalArray::const_iterator Internal_const_iterator;

	/// 末尾に要素を追加する
	void Add(const TKey& key, const TValue& value);

	/// item に一致する最初の要素を削除する
	void Remove(const TValue& item);

	/// 指定したキーに値を設定します。(キーが存在しなければ追加する)
	void SetValue(const TKey& key, const TValue& value)
	{
		int index = Find(key);
		if (index >= 0) {
			m_vector[index].second = value;
		}
		else {
			Add(key, value);
		}
	}

	/// 指定したキーに関連付けられている値を取得します。
	const TValue& GetValue(const TKey& key) const
	{
		int index = Find(key);
		if (index >= 0) {
			return m_vector[index].second;
		}
		LN_THROW(0, KeyNotFoundException);
		return TValue();
	}

	/// 指定したキーに関連付けられている値を取得します。
	bool TryGetValue(const TKey& key, TValue* value) const
	{
		int index = Find(key);
		if (index >= 0) {
			*value = m_vector[index].second;
			return true;
		}
		return false;
	}

	/// 格納されている要素数を取得します。
	int GetCount() const { return (int)m_vector.size(); }

public:
	Pair& operator[] (int index) { return m_vector[index]; }
	const Pair& operator[] (int index) const { return m_vector[index]; }

private:

	int Find(const TKey& key) const
	{
		size_t low = 0;					// 範囲の下限
		size_t high = m_vector.size();	// 範囲の上限。解はあれば [low, high] の中に存在する
		while (low < high)
		{
			size_t mid = (low + high) / 2;		// 中心点
			const Pair& midval = m_vector[mid];	// 中心の値

			// 見つかった
			if (midval.first == key) {
				return mid;
			}
			// 見つからなかった。範囲を狭める
			if (midval.first < key) {	// key の方が大きい → 目的の値は後半にある
				low = mid + 1;
			}
			else {
				high = mid;				// key の方が小さい → 目的の値は前半にある
			}
		}
		return -1;	// 見つからなかった
	}

public:



	class iterator
	{
	public:
		iterator() { m_pos = NULL; }
		iterator(const iterator& obj) { m_pos = obj.m_pos; }
		iterator(Internal_iterator pos) { m_pos = pos; }
		Pair& operator*() { return *m_pos; }//Pair* p = &m_pos; return *p; }
		Pair* operator->() { return &m_pos; }
		iterator& operator++() { ++m_pos; return(*this); }
		iterator operator++(int) { iterator t = *this; ++(*this); return t; }
		iterator& operator--() { --m_pos; return(*this); }
		iterator operator--(int) { iterator t = *this; --(*this); return t; }
		bool operator==(const iterator& right) const { return (m_pos == right.m_pos); }
		bool operator!=(const iterator& right) const { return (!(*this == right)); }

	private:
		Internal_iterator	m_pos;
	};

	class const_iterator
	{
	public:
		const_iterator() { m_pos = NULL;  }
		const_iterator(const const_iterator& obj) { m_pos = obj.m_pos; }
		const_iterator(Internal_const_iterator pos) { m_pos = pos; }
		const Pair& operator*() const { return m_pos; }
		const Pair* operator->() const { return &m_pos; }
		iterator& operator++() { ++m_pos; return(*this); }
		iterator operator++(int) { iterator t = *this; ++(*this); return t; }
		iterator& operator--() { --m_pos; return(*this); }
		iterator operator--(int) { iterator t = *this; --(*this); return t; }
		bool operator==(const iterator& right) const { return (m_pos == right.m_pos); }
		bool operator!=(const iterator& right) const { return (!(*this == right)); }

	private:
		Internal_const_iterator	m_pos;
	};

	iterator begin() { return m_vector.begin(); }
	iterator end() { return m_vector.end(); }

private:
	std::vector<Pair, TAllocator>	m_vector;
};


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TKey, typename TValue, typename TAllocator>
inline void SortedArray<TKey, TValue, TAllocator>::Add(const TKey& key, const TValue& value)
{
	struct Cmp
	{
		static bool CmpEventListener(const Pair& left, const Pair& right)
		{
			return left.first < right.first;
		}
	};

	// 追加後、ソートを掛ける
	Pair e;
	e.first = key;
	e.second = value;
	m_vector.push_back(e);
	std::stable_sort(m_vector.begin(), m_vector.end(), Cmp::CmpEventListener);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename TKey, typename TValue, typename TAllocator>
inline void SortedArray<TKey, TValue, TAllocator>::Remove(const TValue& item)
{
	typename InternalArray::iterator itr = m_vector.begin();
	typename InternalArray::iterator end = m_vector.end();
	for (; itr != end; ++itr)
	{
		if (itr->second == item) {
			m_vector.erase(itr);
			return;
		}
	}
}

} // namespace Lumino
