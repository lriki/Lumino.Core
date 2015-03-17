
#pragma once

#include <deque>
#include "STLUtils.h"

namespace Lumino
{

/**
	@brief	�Q�ƃJ�E���g�����L���[�̃N���X�ł��B
*/
template<typename T, typename TAllocator = STLAllocator<T> >
class Queue 
	: public RefObject
{
public:
	typedef typename std::deque<T, TAllocator>	std_deque;
	typedef typename std_deque::iterator		iterator;
	typedef typename std_deque::const_iterator	const_iterator;

public:
	Queue() {}
	virtual ~Queue() {}

public:

	/// �L���[����ł��邩�𔻒肵�܂��B
	bool IsEmpty() const { return m_queue.empty(); }

	/// �S�Ă̗v�f���폜���܂��B
	void Clear() { m_queue.clear(); }

	/// �i�[����Ă���v�f�����擾���܂��B
	int GetCount() const { return (int)m_queue.size(); }

	/// �X�^�b�N�̐擪�ɗv�f��ǉ����܂��B
	void Enqueue(const T& value) { m_queue.push_back(value); }

	/// �L���[�̐擪�̗v�f���폜���܂��B
	void Dequeue(T* outHead = NULL) { if (outHead) *outHead = GetHead(); m_queue.pop_front(); }

	/// �L���[�̐擪�̗v�f���擾���܂��B
	T& GetHead() { return m_queue.front(); }
	const T& GetHead() const { return m_queue.front(); }

public:
	// STL interface
	iterator		begin()			{ return m_queue.begin(); }
	const_iterator	begin() const	{ return m_queue.begin(); }
	iterator		end()			{ return m_queue.end(); }
	const_iterator	end() const		{ return m_queue.end(); }

private:
	std_deque	m_queue;
};

} // namespace Lumino
