
/*
■.inlに実装を書くと、.h に書いた関数コメントがインテリセンスに表示されなくなる。


■Note: 旧verの非COWとの速度比較 (Split+代入操作1000000回)
非COW	: 270ms
COW		: 200ms

■ムーブコンストラクタについて
	COW の最適化が圧倒的過ぎてムーブコンストラクタを実装するまでも無かったかも。
	COW			: 200ms
	COW+move	: 200ms (ほとんど誤差の範囲)
*/

/*
		最適化のヒント
	メモリコピーを最小限にするために、Array をできるだけ const で修飾してください。
	
		const Array<int>& GetArray()
		{
			return m_ary;
		}

		void main()
		{
			Array<int>        ary1 = GetArray();
			const Array<int>  ary2 = GetArray();
			const Array<int>& ary3 = GetArray();
			cout << ary1[0];	// 非 const の operator[] が呼ばれ、配列の実体が複製されます。
			cout << ary2[0];	// const の operator[] が呼ばれます。複製はされません。
			cout << ary3[0];	// const の operator[] が呼ばれます。複製はされません。
		}
	
■SortedArray vs std::map(VS2013)
	要素数 100000 個の検索
	std::map	: 6,126 us
	SortedArray	: 4,965 us

	要素数 100000 個のイテレート
	std::map	: 7,836 us
	SortedArray	: 6,123 us

	http://srad.jp/~Yak!/journal/503087/

	ソート済み std::vector と同じような結果。
	要素数が増えるほど差が開いていく。
*/


namespace Lumino
{

//=============================================================================
// Array
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename T, typename TAllocator>
Array<T, TAllocator>::Array()
	: m_data(NULL)
{
	m_data = ArrayData::GetSharedEmpty();
	m_data->AddRef();
}
template<typename T, typename TAllocator>
Array<T, TAllocator>::Array(const Array& ary)
{
	m_data = ary.m_data;
	m_data->AddRef();
}
template<typename T, typename TAllocator>
Array<T, TAllocator>::Array(Array&& ary)
	: m_data(ary.m_data)
{
	ary.m_data = NULL;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename T, typename TAllocator>
Array<T, TAllocator>::~Array()
{
	if (m_data != NULL) {
		m_data->Release();
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename T, typename TAllocator>
Array<T, TAllocator>& Array<T, TAllocator>::operator=(const Array& ary)
{
	m_data->Release();
	m_data = ary.m_data;
	m_data->AddRef();
	return *this;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename T, typename TAllocator>
T& Array<T, TAllocator>::operator[] (int index)
{
	CheckOutOfRange(index);
	CheckDetachShared();
	return m_data->m_vector[index];
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename T, typename TAllocator>
const T& Array<T, TAllocator>::operator[] (int index) const
{
	CheckOutOfRange(index);
	return m_data->m_vector[index];
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename T, typename TAllocator>
void Array<T, TAllocator>::CheckDetachShared()
{
	if (m_data->IsShared())
	{
		auto* newData = LN_NEW ArrayData(m_data->m_vector);	// copy
		m_data->Release();
		m_data = newData;
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename T, typename TAllocator>
void Array<T, TAllocator>::CheckOutOfRange(int index) const
{
	LN_THROW(!IsOutOfRange(index), OutOfRangeException);
}

//=============================================================================
// Array::ArrayData
//=============================================================================
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename T, typename TAllocator>
void Array<T, TAllocator>::ArrayData::Release()
{
	--m_refCount;
	if (m_refCount <= 0)
	{
		if (this != GetSharedEmpty()) {
			delete this;
		}
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
template<typename T, typename TAllocator>
typename Array<T, TAllocator>::ArrayData* Array<T, TAllocator>::ArrayData::GetSharedEmpty()
{
	static ArrayData sharedEmpty(100);	// 共有オブジェクトは delete しないように、適当に参照カウントを増やしておく
	return &sharedEmpty;
}


#if 0

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

	/// 配列のインデックスとして有効な整数値であるかを確認する
	bool CheckValidIndex(int index) const { return (0 <= index && index < GetCount()); }

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

#endif

} // namespace Lumino

