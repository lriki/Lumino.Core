
#pragma once
#include <vector>
#include <algorithm>

LN_NAMESPACE_BEGIN

/**
	@brief	STL ユーティリティ
*/
class STLUtils
{
public:

	/// item と一致する最初の要素を削除する
	template <class T, typename TItem/*, typename TAllocator*/>
	static bool Remove(T& vec, const TItem& item)
	{
		typename T::iterator itr = vec.begin();
		typename T::iterator end = vec.end();
		for (; itr != end; ++itr)
		{
			if (*itr == item) {
				vec.erase(itr);
				return true;
			}
		}
		return false;
	}

	/// vector から等しい要素をすべて削除する
	template <class T, typename TAllocator>
	static bool RemoveAll(std::vector<T, TAllocator>& vec, const T& value)
    {
        size_t n = vec.size();
        vec.erase( std::remove( vec.begin(), vec.end(), value ), vec.end() );
        return n != vec.size();
    }

	/// vector から等しい要素をすべて削除する
	template <class T, typename TAllocator, typename TPred>
	static bool RemoveAll(std::vector<T, TAllocator>& vec, TPred pred)
	{
		size_t n = vec.size();
		vec.erase(std::remove_if(vec.begin(), vec.end(), pred), vec.end());
		return n != vec.size();
	}

	/// std::vector に要素が含まれているかチェックする
	template <class T>
	static bool Contains(const std::vector<T>& vec, const T& value)
    {
		return std::find( vec.begin(), vec.end(), value ) != vec.end();
	}




	template<typename ...Args>
	struct first_enabled {};

	template<typename T, typename ...Args>
	struct first_enabled<std::enable_if<true, T>, Args...> { using type = T; };
	template<typename T, typename ...Args>
	struct first_enabled<std::enable_if<false, T>, Args...> : first_enabled<Args...>{};
	template<typename T, typename ...Args>
	struct first_enabled<T, Args...> { using type = T; };

	template<typename ...Args>
	using first_enabled_t = typename first_enabled<Args...>::type;
};

/**
	@brief	STL カスタムアロケータ
	@note	アロケータのシンプルな実装は http://www.codeproject.com/Articles/4795/C-Standard-Allocator-An-Introduction-and-Implement が参考になる。
			しかし、VisualC++12.0ではこの実装だと アロケータをテンプレートのデフォルト引数にできなかった。("コンパイラ内部エラー" となる)
			環境依存の実装が必要なのだと思われるが、愚直に実装しても他のプラットフォームに移植できないので std::allocator から派生させている。
*/
template<class T>
class STLAllocator
	: public std::allocator<T> 
{
public:
	typedef T* pointer;					// need GCC
	typedef const T* const_pointer;		// need GCC
    typedef std::size_t size_type;		// need GCC
	
public:
	template<class U>
	struct rebind
	{
		typedef STLAllocator<U> other;
	};

public:
	STLAllocator() { }
	STLAllocator(const STLAllocator& x) { }

	template<class U>
	STLAllocator(const STLAllocator<U>& x) { }

	pointer allocate(size_type cnt, const_pointer hint = 0)
	{
		return reinterpret_cast<pointer>(::operator new(cnt * sizeof(T)));
	}

	void deallocate(pointer ptr, size_type n)
	{
		::operator delete(ptr);
	}
};

#ifdef LN_DYNAMIC_TRACE
#define LN_FOREACH( var_, col_ ) for (var_ : col_)
#else

//==============================================================================
/*	LN_FOREACH マクロ

	BOOST C++ の BOOST_FOREACH のミニマル実装です。

	STL の 各種コンテナの単純なイテレートを簡略化します。

	例)
		vector<int> vec;
		vec.push_back(1);
		vec.push_back(2);
		vector<int>::iterator itr = vec.begin();
		vector<int>::iterator end = vec.end();
		for (; itr != end; itr++) {
			printf("%d", *itr);
		}
		↓↓↓↓↓

		vector<int> vec;
		vec.push_back(1);
		vec.push_back(2);
		LN_FOREACH(v, vec) {
			printf("%d", v);
		}

	break / continue / return / 多重ループ も通常通り使用できます。

 */
#define LN_FOREACH( var_, col_ ) \
    if ( ::ln::ForeachCore::any_itr_t _ln_foreach_cur_ = ::ln::ForeachCore::begin( col_ ) ) \
    if ( ::ln::ForeachCore::any_itr_t _ln_foreach_end_ = ::ln::ForeachCore::end( col_ ) ) \
    for ( \
        bool _ln_foreach_continue_ = true; \
		_ln_foreach_continue_ && ::ln::ForeachCore::get_itr(_ln_foreach_cur_, col_) != ::ln::ForeachCore::get_itr(_ln_foreach_end_, col_); \
        ++::ln::ForeachCore::get_itr( _ln_foreach_cur_, col_ )) \
		if (::ln::ForeachCore::set_false(_ln_foreach_continue_)) \
			for ( var_ = *::ln::ForeachCore::get_itr( _ln_foreach_cur_, col_ ); !_ln_foreach_continue_; _ln_foreach_continue_ = true )

namespace ForeachCore
{

inline bool set_false(bool& v)
{
	v = false;
	return true;
}

struct any_itr_base
{
    operator bool() const { return true; }
};

template< typename ITR_T >
struct any_itr
    : any_itr_base
{
    explicit any_itr(const ITR_T  &t_ )
      : Item( t_ )
    {}

    mutable ITR_T Item;
};

typedef any_itr_base const &any_itr_t;

template< typename COL_T >
struct col_type
{
    typedef typename COL_T::iterator		itr_type;
    typedef typename COL_T::const_iterator	const_itr_type;
};

/// begin を指す any_itr を取得する

template< typename COL_T >
inline any_itr< typename col_type< COL_T >::itr_type > begin( COL_T& col_ )
{
    return any_itr< typename COL_T::iterator >( col_.begin() );
}
template< typename COL_T >
inline any_itr< typename col_type< COL_T >::const_itr_type > begin( const COL_T& col_ )
{
    return any_itr< typename COL_T::const_iterator >( col_.begin() );
}

/// end を指す any_itr を取得する

template< typename COL_T >
inline any_itr< typename col_type< COL_T >::itr_type > end( COL_T& col_ )
{
    return any_itr< typename COL_T::iterator >( col_.end() );
}
template< typename COL_T >
inline any_itr< typename col_type< COL_T >::const_itr_type > end( const COL_T& col_ )
{
    return any_itr< typename COL_T::const_iterator >( col_.end() );
}

/// any_itr を本来の型の iterator に変換する
template< typename COL_T >
inline typename COL_T::iterator& get_itr( any_itr_t any_itr_, COL_T& )
{
    return static_cast< any_itr< typename COL_T::iterator > const& >( any_itr_ ).Item;
}

} // namespace ForeachCore
#endif





template<typename TChar> class GenericStringRef;

namespace detail
{
	
template<typename TString>
struct StringCaseInsensitiveLess
{
	typename typedef TString::CharType CharType;
	typename typedef GenericStringRef<CharType> StringRef;

	LN_CONSTEXPR bool operator()(const TString& left, const TString& right) const
	{
		return left.Compare(right.c_str(), -1, CaseSensitivity::CaseInsensitive) < 0;
	}
	// String and char*
	LN_CONSTEXPR bool operator()(const TString& left, const CharType* right) const
	{
		return left.Compare(right, -1, CaseSensitivity::CaseInsensitive) < 0;
	}
	LN_CONSTEXPR bool operator()(const CharType* left, const TString& right) const
	{
		return right.Compare(left, -1, CaseSensitivity::CaseInsensitive) > 0;
	}
	// String and StringRef
	LN_CONSTEXPR bool operator()(const TString& left, const StringRef& right) const
	{
		return left.Compare(right.GetBegin(), -1, CaseSensitivity::CaseInsensitive) < 0;
	}
	LN_CONSTEXPR bool operator()(const StringRef& left, const TString& right) const
	{
		return right.Compare(left.GetBegin(), -1, CaseSensitivity::CaseInsensitive) > 0;
	}
};

template<typename T>
struct SortedArrayBasicLess
{
	template<typename TOtherKey>
	LN_CONSTEXPR bool operator()(const T& left, const TOtherKey& right) const
	{
		return (left < right);
	}
};

} // namespace detail

LN_NAMESPACE_END
