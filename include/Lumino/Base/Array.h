
#pragma once

#include <vector>
#include "STLUtils.h"

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
