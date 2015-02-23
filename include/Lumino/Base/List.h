
#pragma once

#if 0
#include <vector>
#include "STLUtils.h"
#include "RefObject.h"

namespace Lumino
{


template<class T>
class List
{
public:

	typedef int size_type;

private:

	struct Node
	{
		int Next;
		int Prev;
	};

	template<class T>
	class ListData : public RefObject
	{
	public:

		ListData()
		{
			mValues = NULL;
			mNodes = NULL;
			mCapacity = 0;
			mSize = 0;
			mIndexStack = NULL;
			mIndexStackEnd = 0;
		}

	public:

		void Reserve(size_type capacity)
		{
			LN_THROW(mValues == NULL, InvalidOperationException);
			if (capacity <= 0) {
				return;
			}

			mCapacity = capacity;
			mNodes = LN_NEW Node[mCapacity + 1];
			mValues = static_cast<T*>(LN_OPERATOR_NEW(sizeof(T) * mCapacity));

			// 空き番号スタックを作成して番号を全部詰め込む
			mIndexStack = LN_NEW int[mCapacity];
			for (int i = 0; i < mCapacity; ++i)
			{
				mIndexStack[i] = i;
			}
			mIndexStackEnd = mCapacity;

			// mCapacity 番は最初の要素の前かつ最後の要素の後にあるダミー要素
			mNodes[mCapacity].Next = mNodes[mCapacity].Prev = mCapacity;
		}

		void push_back(const T& value)
		{

		}

	public:
		T*				mValues;		///< オブジェクト配列
		Node*			mNodes;			///< ノード配列
		int				mCapacity;		///< 最大要素数
		int				mSize;			///< 現在の要素数
		int*			mIndexStack;	///< オブジェクト配列の空き番号スタック
		int				mIndexStackEnd;	///< スタックの終端 (これのひとつ前まで要素がある)
	};

public:

	/// イテレータ
	class iterator
	{
	public:

		iterator()
		{
			mValues = NULL;
			mNodes = NULL;
			mPos = 0;
		}

		iterator(const iterator& obj)
		{
			mValues = obj.mValues;
			mNodes = obj.mNodes;
			mPos = obj.mPos;
		}

		iterator& operator = (const iterator& obj)
		{
			mValues = obj.mValues;
			mNodes = obj.mNodes;
			mPos = obj.mPos;
			return (*this);
		}

		T& operator * () { return mValues[mPos]; }
		T* operator -> () { return &mValues[mPos]; }

		iterator& operator ++ () { mPos = mNodes[mPos].Next; return *this; }
		iterator operator ++ (int) { iterator c(*this); mPos = mNodes[mPos].Next; return c; }
		iterator& operator -- () { mPos = mNodes[mPos].Prev; return *this; }
		iterator operator -- (int) { iterator c(*this); mPos = mNodes[mPos].Prev; return c; }

		bool operator == (const iterator& obj) const { return (mPos == obj.mPos); }
		bool operator != (const iterator& obj) const { return (mPos != obj.mPos); }

	private:

		iterator(T* values_, Node* nodes_, int pos_)
		{
			mValues = values_;
			mNodes = nodes_;
			mPos = pos_;
		}

	private:

		T*		mValues;		///< オブジェクト配列
		Node*	mNodes;			///< ノード配列
		int		mPos;			///< ノード配列の位置 (このイテレータの指す位置)

		friend class List<T>;
	};

public:

	List() {}

	void push_back(const T& value)
	{

	}

private:
	ListData<T>	mListData;
};













} // namespace Lumino

#endif

