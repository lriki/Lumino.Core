
#pragma once

#include <vector>
#include "STLUtils.h"

namespace Lumino
{

/**
	@brief	���I�z�� (�f�t�H���g�̃A���P�[�^�ɃJ�X�^���A���P�[�^���w��)
*/
template<typename T, typename TAllocator = STLAllocator<T> >
class Array : public std::vector<T, TAllocator >
{
public:
	typedef std::vector<T, TAllocator> std_vector;

public:
	/// �z��̃C���f�b�N�X�Ƃ��ėL���Ȑ����l�ł��邩���m�F����
	bool CheckValidIndex(int index) const { return (0 <= index && index < GetCount()); }
	
	/// �v�f�����擾����
	int GetCount() const { return (int)std_vector::size(); }

	/// �����ɗv�f��ǉ�����
	void Add(const T& value) { std_vector::push_back(value); }

	/// �I�[�v�f�̎Q�Ƃ�Ԃ�
	T& GetLast() { return *(std_vector::rbegin()); }

	/// �I�[�v�f�̎Q�Ƃ�Ԃ�
	const T& GetLast() const { return *(std_vector::rbegin()); }

	/// item �Ɉ�v����ŏ��̗v�f���폜����
	void Remove(const T& item) { STLUtils::Remove(*this, item); }

	/// item �Ɉ�v����S�Ă̗v�f���폜����
	void RemoveAll(const T& item) { STLUtils::RemoveAll(*this, item); }

	/// ����v�f�����̔z����ɑ��݂��邩�ǂ����𔻒f���܂��B
	bool Contains(const T& item) const { return std::find(std_vector::begin(), std_vector::end(), item) != std_vector::end(); }
};

/**
	@brief		�L�[�� "operator <" �̎����ɂ��l����בւ��铮�I�z��
*/
template<typename TKey, typename TValue, typename TAllocator = STLAllocator< std::pair<TKey, TValue> > >
class SortedArray
{
public:

	/// �����ɗv�f��ǉ�����
	void Add(const TKey& key, const TValue& value);

	/// item �Ɉ�v����ŏ��̗v�f���폜����
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

	// �ǉ���A�\�[�g���|����
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
