
#pragma once

#include <deque>
#include "STLUtils.h"

namespace Lumino
{

/**
	@brief	参照カウントを持つキューのクラスです。
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

	/// キューが空であるかを判定します。
	bool IsEmpty() const { return m_queue.empty(); }

	/// 全ての要素を削除します。
	void Clear() { m_queue.clear(); }

	/// 格納されている要素数を取得します。
	int GetCount() const { return (int)m_queue.size(); }

	/// スタックの先頭に要素を追加します。
	void Enqueue(const T& value) { m_queue.push_back(value); }

	/// キューの先頭の要素を削除します。
	void Dequeue(T* outHead = NULL) { if (outHead) *outHead = GetHead(); m_queue.pop_front(); }

	/// キューの先頭の要素を取得します。
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
