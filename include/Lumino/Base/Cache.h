/**
	@file	Cache.h
*/
#pragma once

#include <map>
#include "String.h"
#include "../IO/PathName.h"
#include "../Threading/Mutex.h"

namespace Lumino
{

/**
	@brief	�L���b�V���̌����L�[�ł��B
*/
class CacheKey
{
public:
	static const CacheKey Null;

public:
	CacheKey();
	explicit CacheKey(uint32_t hashCode);
	explicit CacheKey(const TCHAR* str);
	explicit CacheKey(const String& str);
	explicit CacheKey(const PathName& path);

	bool operator == (const CacheKey& key) const;
	bool operator < (const CacheKey& key) const;

	bool IsNull() const;

private:

	enum Type
	{
		Type_Null = 0,
		Type_HashCode,
		Type_String,
		Type_PathName,
	};

	Type		m_keyType;
	uint32_t	m_hashCode;
	String		m_string;
	PathName	m_pathName;
};

/**
	@brief		�L���b�V���\�ȃI�u�W�F�N�g�̃C���^�[�t�F�C�X�ł��B
	@details	ICacheObject �� RefObject �ƃZ�b�g�Ŏg�p���܂��B
				�������p�������N���X�� LN_CACHE_OBJECT_DECL �}�N���ŕK�v�ȋ@�\���������܂��B
				ICacheObject �͎Q�ƃJ�E���g�� 0 �ɂȂ����Ƃ��Adelete �͂��ꂸ�L���b�V���ɓ�����܂��B
*/
class ICacheObject
{
protected:
	friend class CacheManager;
	struct CacheObjectInfo
	{
		CacheManager*	Manager;
		CacheKey		Key;
		size_t			CacheMemorySize;
		bool			InCacheList;
		//bool			mIsStockObject;

		CacheObjectInfo();
		~CacheObjectInfo();
	};

public:
	ICacheObject() {}

protected:
	virtual ~ICacheObject() {}
	virtual CacheObjectInfo&	GetCacheObjectInfo() = 0;

public:
	virtual int32_t	AddRef() = 0;
	virtual int32_t	Release() = 0;
};

/// ICacheObject �̎������[�e�B���e�B
#define LN_CACHE_OBJECT_DECL \
protected: \
	CacheObjectInfo m_cacheObjectInfo; \
public: \
	virtual CacheObjectInfo& GetCacheObjectInfo() { return m_cacheObjectInfo; } \
	virtual int32_t AddRef() { return RefObject::AddRef(); } \
	virtual int32_t Release() \
	{ \
		if (m_cacheObjectInfo.Manager == NULL/* || m_cacheObjectInfo.mIsStockObject*/) \
		{ \
			return RefObject::Release(); \
		} \
		int32_t count = mReferenceCount.Decrement(); \
		LN_ASSERT(count >= 0); \
		if (count == 0) \
		{ \
			m_cacheObjectInfo.Manager->AddCacheUnusedList(this); \
		} \
		return count; \
	}

/**
	@brief	�L���b�V���I�u�W�F�N�g�̊Ǘ��N���X�ł��B
*/
class CacheManager
	: public RefObject
{
public:
	/**
		@brief		������
		@param[in]	capacityCount	: �L���b�V���ɕێ��ł���ő�I�u�W�F�N�g��
		@param[in]	maxMemorySize	: �L���b�V���ɕێ��ł���ő僁������ (byte�P�ʁB0 �̏ꍇ�̓������ʂ��l�����Ȃ�)
	*/
	CacheManager(int capacityCount, size_t maxMemorySize);

public:

	/**
		@brief	�I������ (����ȍ~�L���b�V���ɃI�u�W�F�N�g��ǉ����Ȃ��悤�ɂ���)
	*/
	void Finalize();

	/**
		@brief	�I�u�W�F�N�g���L���b�V���Ǘ��ɒǉ����܂��B
	*/
	void RegisterCacheObject(const CacheKey& key, ICacheObject* obj);

	/**
		@brief		�L�[�Ɉ�v����I�u�W�F�N�g���������܂��B
		@details	�Q�ƃJ�E���g���C���N�������g���ĕԂ��܂��B
	*/
	ICacheObject* FindObjectAddRef(const CacheKey& key);
	
	/**
		@brief	�L���b�V�����N���A���܂��B
	*/
	void ClearCache();

public:
	void AddCacheUnusedList(ICacheObject* obj);
	void DeleteCachedObject(ICacheObject* obj);

private:
	virtual ~CacheManager();

private:
	friend class ICacheObject;
	class CacheUnusedList;
	typedef std::map<CacheKey, ICacheObject*>	CacheUsingMap;
	typedef std::pair<CacheKey, ICacheObject*>	CacheUsingPair;

	CacheUnusedList*	m_cacheUnusedList;
	CacheUsingMap		m_cacheUsingMap;
	Threading::Mutex	m_mutex;
};

} // namespace Lumino
