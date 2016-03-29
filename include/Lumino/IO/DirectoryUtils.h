#pragma once

#include "../Base/Array.h"

LN_NAMESPACE_BEGIN

/**
	@brief	
*/
class DirectoryUtils
{
public:

	/**
		@brief		カレントディレクトリのパスを取得します。
		@param[in]	outPath	: パス文字列を格納するバッファ (LN_MAX_PATH 文字数分のサイズが必要) 
		@return		パスの文字数 (\0 は除く)
	*/
	template<typename TChar>
	static size_t GetCurrentDirectory(TChar* outPath);

	/**
		@brief		ディレクトリ内に含まれる全てのファイルのパスを取得します。
		@param[in]	dirPath	: 検索するディレクトリ
		@param[in]	pattern	: 検索文字列 (ワイルドカード指定可能)
	*/
	static Array<String> GetFiles(const TCHAR* dirPath, const TCHAR* pattern);


};


#if defined(LN_OS_WIN32)
template<typename TChar>
class GenericFileFinder
{
public:
	GenericFileFinder(const GenericStringRef<TChar>& path);
	~GenericFileFinder();
	const TChar* GetCurrent() const;
	bool Next();
};

// for char
template<>
class GenericFileFinder<char>
{
public:
	GenericFileFinder(const GenericStringRef<char>& path);
	~GenericFileFinder();
	const char* GetCurrent() const;
	bool Next();

private:
	HANDLE				m_fh;
	WIN32_FIND_DATAA	m_fd;
	bool				m_end;
};

// for wchar_t
template<>
class GenericFileFinder<wchar_t>
{
public:
	GenericFileFinder(const GenericStringRef<wchar_t>& path);
	~GenericFileFinder();
	const wchar_t* GetCurrent() const;
	bool Next();

private:
	HANDLE				m_fh;
	WIN32_FIND_DATAW	m_fd;
	bool				m_end;
};
#elif defined(LN_OS_FAMILY_UNIX)
#endif


//template<typename TChar>
//class GenericFileFinder
//{
//public:
//	GenericFileFinder(const GenericStringRef<TChar>& path);
//	~GenericFileFinder();
//
//	const TChar* Next();
//
//private:
//#if defined(LN_OS_WIN32)
//	HANDLE		m_fh;
//	TChar		m_current[LN_MAX_PATH];
//#else defined(LN_OS_FAMILY_UNIX)
//	DIR*        _pDir
//	TChar		m_current[LN_MAX_PATH];
//#endif
//};

LN_NAMESPACE_END
