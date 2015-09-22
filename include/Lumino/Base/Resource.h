/**
	@file	Resource.h
*/
#pragma once
#include <map>
#include "String.h"

namespace Lumino
{
	
/**
	@brief	
*/
class ResourceManager
{
public:

	/**
		@brief		指定したリソース名に対応する文字列リソースを取得します。
		@return		文字列リソース。見つからない場合は空文字列を返します。
	*/
	const String& GetString(const String& name);

	/**
		@brief	指定したリソース名に対応する文字列リソースを追加します。
	*/
	void AddString(const String& name, const String& value);

private:
	std::map<String, String>	m_stringMap;
};

/**
	@brief	
*/
class InternalResource
{
public:
	static const String UnknownError;
	static const String VerifyError;
	static const String ArgumentError;
	static const String InvalidOperationError;
	static const String NotImplementedError;
	static const String OutOfMemoryError;
	static const String OutOfRangeError;
	static const String KeyNotFoundError;
	static const String OverflowError;
	static const String IOError;
	static const String FileNotFoundError;
	static const String DirectoryNotFoundError;
	static const String InvalidFormatError;
	static const String EndOfStreamError;
	static const String EncodingError;
	static const String Win32Error;
	static const String ComError;

	static const String OpenGLError;

public:
	static const String& GetString(const String& name);
};

} // namespace Lumino
