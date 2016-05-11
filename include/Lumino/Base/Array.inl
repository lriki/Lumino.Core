
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

LN_NAMESPACE_BEGIN

//==============================================================================
// Array
//==============================================================================
//------------------------------------------------------------------------------
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
template<typename T, typename TAllocator>
Array<T, TAllocator>::Array(std::initializer_list<T> list)
{
	m_data = LN_NEW ArrayData(1);
	m_data->m_vector.assign(list.begin(), list.end());
}

//------------------------------------------------------------------------------
template<typename T, typename TAllocator>
Array<T, TAllocator>::~Array()
{
	if (m_data != NULL) {
		m_data->Release();
	}
}

//------------------------------------------------------------------------------
template<typename T, typename TAllocator>
Array<T, TAllocator>& Array<T, TAllocator>::operator=(const Array& ary)
{
	m_data->Release();
	m_data = ary.m_data;
	m_data->AddRef();
	return *this;
}

//------------------------------------------------------------------------------
template<typename T, typename TAllocator>
T& Array<T, TAllocator>::operator[] (int index)
{
	CheckOutOfRange(index);
	CheckDetachShared();
	return m_data->m_vector[index];
}

//------------------------------------------------------------------------------
template<typename T, typename TAllocator>
const T& Array<T, TAllocator>::operator[] (int index) const
{
	CheckOutOfRange(index);
	return m_data->m_vector[index];
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
template<typename T, typename TAllocator>
void Array<T, TAllocator>::CheckOutOfRange(int index) const
{
	LN_THROW(!IsOutOfRange(index), OutOfRangeException);
}

//==============================================================================
// Array::ArrayData
//==============================================================================
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
template<typename T, typename TAllocator>
typename Array<T, TAllocator>::ArrayData* Array<T, TAllocator>::ArrayData::GetSharedEmpty()
{
	static ArrayData sharedEmpty(100);	// 共有オブジェクトは delete しないように、適当に参照カウントを増やしておく
	return &sharedEmpty;
}

LN_NAMESPACE_END
