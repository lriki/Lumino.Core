/**
	@file	SortedArray.h
*/
#pragma once
#include <vector>
#include <algorithm>
#include "Exception.h"
#include "STLUtils.h"

namespace Lumino
{

/**
	@brief		�L�[�� "operator<" �̎����ɂ��l����בւ���ꂽ�A�L�[�ƒl�̃y�A�̔z��ł��B
	@details	���̃N���X�̖����� std::map �Ǝ��Ă��܂����A��荂���Ɍ����y�їv�f�̃C�e���[�g���s�����Ƃ��ł��܂��B
				�������A�V�����L�[���i�[���邽�тɔz��S�̂��\�[�g����邽�߁A�L�[�̍X�V�ɂ͎��Ԃ�������܂��B
*/
template<typename TKey, typename TValue, typename TAllocator = STLAllocator< std::pair<TKey, TValue> > >
class SortedArray
{
public:
	typedef typename std::pair<TKey, TValue>		value_type;
	typedef typename std::pair<TKey, TValue>		Pair;
	typedef typename std::vector<Pair, TAllocator>	InternalArray;
	typedef typename InternalArray::const_iterator	iterator;
	typedef typename InternalArray::const_iterator	const_iterator;

public:

	/**
		@brief	�i�[����Ă���L�[/�l�y�A�̐����擾���܂��B
	*/
	int GetCount() const
	{
		return (int)m_vector.size();
	}

	/** 
		@brief		�w�肵���L�[�ƒl�̃y�A��ǉ����܂��B
		@param[in]	key		: �v�f�̃L�[
		@param[in]	value	: �v�f�̒l
		@exception	ArgumentException	�L�[�����ɒǉ�����Ă��܂��B
	*/
	void Add(const TKey& key, const TValue& value)
	{
		LN_THROW(!ContainsKey(key), ArgumentException);
		struct Cmp
		{
			static bool CmpEventListener(const Pair& left, const Pair& right)
			{
				return left.first < right.first;
			}
		};
		m_vector.push_back(Pair(key, value));
		std::sort(m_vector.begin(), m_vector.end(), Cmp::CmpEventListener);
	}

	/**
		@brief	�w�肵���l�ƈ�v����l�����S�Ă̗v�f���폜���܂��B
	*/
	void RemoveAllValue(const TValue& item)
	{
		typename InternalArray::iterator itr = m_vector.begin();
		for (; itr != m_vector.end();)
		{
			if (itr->second == item) {
				itr = m_vector.erase(itr);
			}
			else {
				++itr;
			}
		}
	}

	/**
		@brief	�w�肵���L�[���i�[����Ă��邩���m�F���܂��B
		@return	�i�[����Ă���ꍇ�� true�B����ȊO�̏ꍇ�� false�B
	*/
	bool ContainsKey(const TKey& key) const
	{
		int index = LowerBound(key);
		if (index < GetCount() && m_vector[index].first == key) {
			return true;
		}
		return false;
	}

	/**
		@brief		�w�肵���L�[�ɒl��ݒ肵�܂��B
		@details	�L�[�����݂��Ȃ��ꍇ�A�w�肵���L�[�ƒl�����V�����v�f���쐬���܂��B
	*/
	void SetValue(const TKey& key, const TValue& value)
	{
		int index = LowerBound(key);
		if (index < GetCount() && m_vector[index].first == key)
		{
			m_vector[index].second = value;
		}
		else {
			Add(key, value);
		}
	}

	/**
		@brief		�w�肵���L�[�Ɋ֘A�t�����Ă���l���擾���܂��B
		@exception	KeyNotFoundException
	*/
	const TValue& GetValue(const TKey& key) const
	{
		int index = LowerBound(key);
		if (index < GetCount() && m_vector[index].first == key)
		{
			return m_vector[index].second;
		}
		LN_THROW(0, KeyNotFoundException);
		return m_vector.end()->second;
	}

	/**
		@brief		�w�肵���L�[�Ɋ֘A�t�����Ă���l���擾���܂��B
		@return		�L�[���i�[����Ă���ꍇ�� true�B����ȊO�̏ꍇ�� false�B
		@details	GetValue() �Ǝ�����������܂����A�����̐��ۂ��O�ł͂Ȃ��߂�l�ŕԂ��܂��B
	*/
	bool TryGetValue(const TKey& key, TValue* value) const
	{
		int index = LowerBound(key);
		if (index < GetCount() && m_vector[index].first == key)
		{
			if (value != NULL) { *value = m_vector[index].second; }
			return true;
		}
		return false;
	}

	/**
		@brief		�w�肵���L�[�̒l�̎Q�Ƃ��擾���܂��B
		@details	�L�[�����݂��Ȃ��ꍇ�A�w�肵���L�[�����V�����v�f���쐬���܂��B
	*/
	TValue& operator[](const TKey& key)
	{
		int index = LowerBound(key);
		if (index < GetCount() && m_vector[index].first == key) {
			
		}
		else {
			Add(key, TValue());
			index = LowerBound(key);
		}
		return m_vector[index].second;
	}

	/**
		@brief		�w�肵���L�[�̒l�̎Q�Ƃ��擾���܂��B
		@exception	KeyNotFoundException
	*/
	const TValue& operator[](const TKey& key) const
	{
		return GetValue(key);
	}

public:

	/*-----------------------------------------------------------------------*/
	/** @name STL interface */
	/** @{ */

	iterator		begin() { return m_vector.begin(); }
	iterator		end() { return m_vector.end(); }
	const_iterator	begin() const { return m_vector.begin(); }
	const_iterator	end() const { return m_vector.end(); }

	/** @} */

private:

	int LowerBound(const TKey& key) const
	{
		if (m_vector.empty()) { return GetCount(); }

		int count = (int)m_vector.size();
		int first = 0;

		const Pair* pairs = &m_vector[0];

		while (0 < count)
		{
			int count2 = count / 2;
			int mid = first + count2;

			if (pairs[mid].first < key) {
				first = ++mid;
				count -= count2 + 1;
			}
			else {
				count = count2;
			}
		}
		return first;
	}

private:
	std::vector<Pair, TAllocator>	m_vector;
};


} // namespace Lumino
