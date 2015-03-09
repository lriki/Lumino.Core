
#pragma once

#include <stack>

namespace Lumino
{

/**
	@brief	参照カウントを持つスタックのクラスです。
*/
template<typename T/*, typename TAllocator = STLAllocator<T>*/ >	// TODO: Allocator が使えない。vector 化を検討する。
class Stack : public RefObject
{
public:
	typedef typename std::stack<T/*, TAllocator*/>	std_stack;

public:
	Stack() {}
	virtual ~Stack() {}

public:

	/// スタックが空であるかを判定します。
	bool IsEmpty() const { return m_stack.empty(); }

	/// 全ての要素を削除します。
	void Clear() { m_stack.clear(); }

	/// 格納されている要素数を取得します。
	int GetCount() const { return (int)m_stack.size(); }

	/// スタックの先頭に要素を追加します。
	void Push(const T& value) { m_stack.push(value); }

	/// スタックの先頭の要素を削除します。
	void Pop(T* outTop = NULL) { if (outTop) *outTop = GetTop(); m_stack.pop(); }

	/// スタックの先頭の要素を取得します。
	T& GetTop() { return m_stack.top(); }
	const T& GetTop() const { return m_stack.top(); }

private:
	std_stack	m_stack;
};

} // namespace Lumino
