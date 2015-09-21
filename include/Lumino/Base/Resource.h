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
		@brief		�w�肵�����\�[�X���ɑΉ����镶���񃊃\�[�X���擾���܂��B
		@return		�����񃊃\�[�X�B������Ȃ��ꍇ�͋󕶎����Ԃ��܂��B
	*/
	const String& GetString(const String& name);

	/**
		@brief	�w�肵�����\�[�X���ɑΉ����镶���񃊃\�[�X��ǉ����܂��B
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
