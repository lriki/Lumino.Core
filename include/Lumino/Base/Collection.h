/**
	@file	Collection.h
*/
#pragma once
#include "Array.h"
#include "NonCopyable.h"

namespace Lumino
{

/**
	@brief		�R���N�V�����̊�{�N���X�ł��B
	@details	Collection �́A�p�����ēƎ��̃R���N�V�������`���邱�Ƃ�z�肵�� Array �N���X�̃��b�p�[�ł��B
*/
template<typename T, typename TAllocator = STLAllocator<T> >
class Collection : private NonCopyable
{
public:
	typedef typename Array<T>							internal_array;
	typedef typename internal_array::value_type			value_type;
	typedef typename internal_array::difference_type	difference_type;
	typedef typename internal_array::pointer			pointer;
	typedef typename internal_array::const_pointer		const_pointer;
	//typedef typename internal_array::iterator			iterator;
	//typedef typename internal_array::const_iterator		const_iterator;
	typedef typename internal_array::reference			reference;
	typedef typename internal_array::const_reference	const_reference;
	class iterator;
	class const_iterator;

public:

	/** ��̃R���N�V���������������܂��B*/
	Collection() {}

	/** �f�X�g���N�^ */
	virtual ~Collection() {}

public:

	/** �R���N�V��������ł��邩���m�F���܂��B*/
	bool IsEmpty() const
	{
		return m_array.IsEmpty();
	}

	/** �i�[����Ă���v�f�̐����擾���܂��B*/
	int GetCount() const
	{
		return m_array.GetCount();
	}

	/** �����ɗv�f��ǉ����܂��B*/
	void Add(const T& item)
	{
		InsertItem(GetCount(), item);
	}

	/** �w�肵���C���f�b�N�X�̈ʒu�ɗv�f��}�����܂��B*/
	void Insert(int index, const T& item)
	{
		InsertItem(index, item);
	}

	/** item �Ɉ�v����ŏ��̗v�f���폜���܂��B(����ɍ폜���ꂽ�ꍇ�� true ��Ԃ��B�v�f��������Ȃ���� false ��Ԃ�)*/
	bool Remove(const T& item)
	{
		int index = m_array.IndexOf(item);
		if (index < 0) { return false; }
		RemoveItem(index);
		return true;
	}

	/** �w�肵���C���f�b�N�X�ɂ���v�f���폜���܂��B*/
	void RemoveAt(int index)
	{
		RemoveItem(index);
	}

	/** �S�Ă̗v�f���폜���܂��B*/
	void Clear()
	{
		ClearItems();
	}

	/** �w�肵���v�f�����̔z����ɑ��݂��邩�ǂ����𔻒f���܂��B*/
	bool Contains(const T& item) const
	{
		return m_array.Contains(item);
	}

	/** �w�肵���C���f�b�N�X�����̔z��̋��E�͈̔͊O�����m�F���܂��B*/
	bool IsOutOfRange(int index) const
	{
		return m_array.IsOutOfRange(index);
	}

	/**
		@brief		�v�f���w�肵���ʒu���猟�����A�ŏ��Ɍ��������C���f�b�N�X��Ԃ��܂��B
		@param[in]	item		: ��������v�f
		@param[in]	startIndex	: �������J�n����C���f�b�N�X (�ȗ������ꍇ�͐擪����)
		@return		���������v�f���ŏ��Ɍ��ꂽ�ʒu�B������Ȃ������ꍇ�� -1�B
	*/
	int IndexOf(const T& item, int startIndex = 0) const
	{
		return m_array.IndexOf(item, startIndex);
	}

	/** �w�肵���C���f�b�N�X�ɗv�f��ݒ肵�܂��B*/
	void SetAt(int index, const T& item)
	{
		SetItem(index, item);
	}

	/** �w�肵���C���f�b�N�X�ɂ���v�f�ւ̎Q�Ƃ��擾���܂��B*/
	const_reference GetAt(int index) const
	{
		return m_array.GetAt(index);
	}

	/** �w�肵���C���f�b�N�X�ɂ���v�f�ւ̎Q�Ƃ��擾���܂��B*/
	//reference operator[] (int index)
	//{
	//	return m_array[index];
	//}

	/** �w�肵���C���f�b�N�X�ɂ���v�f�ւ̎Q�Ƃ��擾���܂��B*/
	const_reference operator[] (int index) const
	{
		return m_array[index];
	}

	//----------------------------------------------------------------------------
	/** @name STL interface */
	/** @{ */

	iterator		begin()			{ return iterator(m_array.begin()); }
	const_iterator	begin() const	{ return const_iterator(m_array.begin()); }
	iterator		end()			{ return iterator(m_array.end()); }
	const_iterator	end() const		{ return const_iterator(m_array.end()); }

	/** @} */

public:
	class const_iterator
		: public std::iterator<std::input_iterator_tag, T>
	{
	public:
		const_iterator() : m_internalItr() {}
		const_iterator(const const_iterator& obj) : m_internalItr(obj.m_internalItr) {}
		const_iterator& operator = (const const_iterator& obj) { m_internalItr = obj.m_internalItr; return (*this); }

		bool operator==(const const_iterator& right) const	{ return m_internalItr == right.m_internalItr; }
		bool operator!=(const const_iterator& right) const	{ return m_internalItr != right.m_internalItr; }
		bool operator<(const const_iterator& right) const	{ return m_internalItr < right.m_internalItr; }

		const_reference operator*() const	{ return *m_internalItr; }
		pointer operator->() const			{ return m_internalItr.operator->(); }

		const_iterator& operator++()		{ ++m_internalItr; return (*this); }
		const_iterator operator++(int)		{ const_iterator tmp = *this; ++(*this); return tmp; }

	private:
		friend class Collection;
		const_iterator(const typename internal_array::const_iterator& itr) : m_internalItr(itr) {}
		typename internal_array::const_iterator m_internalItr;
	};

	class iterator
		: public std::iterator<std::input_iterator_tag, T>
	{
	public:
		iterator() : m_internalItr() {}
		iterator(const iterator& obj) : m_internalItr(obj.m_internalItr) {}
		iterator& operator = (const iterator& obj) { m_internalItr = obj.m_internalItr; return (*this); }

		bool operator==(const iterator& right) const		{ return m_internalItr == right.m_internalItr; }
		bool operator!=(const iterator& right) const		{ return m_internalItr != right.m_internalItr; }
		bool operator<(const iterator& right) const			{ return m_internalItr < right.m_internalItr; }

		const_reference operator*() const	{ return *m_internalItr; }
		pointer operator->() const			{ return m_internalItr.operator->(); }

		iterator& operator++()				{ ++m_internalItr; return (*this); }
		iterator operator++(int)			{ iterator tmp = *this; ++(*this); return tmp; }

	private:
		friend class Collection;
		iterator(const typename internal_array::iterator& itr) : m_internalItr(itr) {}
		typename internal_array::iterator m_internalItr;
	};

protected:

	/** �w�肵���C���f�b�N�X�̈ʒu�ɗv�f��}�����܂��B*/
	virtual void InsertItem(int index, const value_type& item)
	{
		m_array.Insert(index, item);
	}

	/** �S�Ă̗v�f���폜���܂��B*/
	virtual void ClearItems()
	{
		m_array.Clear();
	}

	/** �w�肵���C���f�b�N�X�ɂ���v�f���폜���܂��B*/
	virtual void RemoveItem(int index)
	{
		m_array.RemoveAt(index);
	}

	/** �w�肵���C���f�b�N�X�ʒu�ɂ���v�f��u�������܂��B*/
	virtual void SetItem(int index, const value_type& item)
	{
		m_array.SetAt(index, item);
	}

private:
	Array<T>	m_array;
};

} // namespace Lumino