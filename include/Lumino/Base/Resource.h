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
	static const String OutOfMemory;

public:
	static const String& GetString(const String& name);
};

} // namespace Lumino
