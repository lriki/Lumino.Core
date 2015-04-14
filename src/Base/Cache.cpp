
#include <Lumino/Base/Common.h>
#include <Lumino/Base/Stack.h>
#include <Lumino/Base/Cache.h>

namespace Lumino
{

//=============================================================================
// CacheKey
//=============================================================================

const CacheKey CacheKey::Null;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CacheKey::CacheKey()
	: m_keyType(Type_Null)
	, m_hashCode(0)
	, m_string()
	, m_pathName()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CacheKey::CacheKey(uint32_t hashCode)
	: m_keyType(Type_HashCode)
	, m_hashCode(hashCode)
	, m_string()
	, m_pathName()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CacheKey::CacheKey(const TCHAR* str)
	: m_keyType(Type_String)
	, m_hashCode(0)
	, m_string(str)
	, m_pathName()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CacheKey::CacheKey(const String& str)
	: m_keyType(Type_String)
	, m_hashCode(0)
	, m_string(str)
	, m_pathName()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CacheKey::CacheKey(const PathName& path)
	: m_keyType(Type_PathName)
	, m_hashCode(0)
	, m_string()
	, m_pathName(path)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool CacheKey::operator == (const CacheKey& key) const
{
	if (m_keyType != key.m_keyType) {
		return false;
	}

	switch (m_keyType)
	{
	case Type_Null:
		return true;
	case Type_HashCode:
		return (m_hashCode == key.m_hashCode);
	case Type_String:
		return (m_string == key.m_string);
	case Type_PathName:
		return (m_pathName == key.m_pathName);
	}

	LN_ASSERT(0);	// �����ɗ���̂� NG
	return false;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool CacheKey::operator < (const CacheKey& key) const
{
	if (m_keyType != key.m_keyType) {
		return m_keyType < key.m_keyType;
	}

	switch (m_keyType)
	{
	case Type_Null:
		return false;
	case Type_HashCode:
		return (m_hashCode < key.m_hashCode);
	case Type_String:
		return (m_string < key.m_string);
	case Type_PathName:
		return (m_pathName < key.m_pathName);
	}

	LN_ASSERT(0);	// �����ɗ���̂� NG
	return false;
	// map,set ���������s�����́uX1 < X2�v�uX2 < X1�v�̗������������Ȃ������Ƃ��uX1==X2�v�Ƃ݂Ȃ��̂��W���I�ȓ���炵��
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool CacheKey::IsNull() const
{
	return (m_keyType == Type_Null);
}

//=============================================================================
// CacheManager::CacheUnusedList
//=============================================================================
class CacheManager::CacheUnusedList
{
public:
	CacheUnusedList(CacheManager* manager)
		: m_manager(manager)
		, mNodes(NULL)
		, m_internalCapacity(0)
		, mSize(0)
		, m_usedMemorySize(0)
		, m_maxMemorySize(ULLONG_MAX)
	{}

	~CacheUnusedList()
	{
		Clear();
		LN_SAFE_DELETE_ARRAY(mNodes);
	}

public:

	/// �ő�v�f���̎擾
	int GetCapacity() const { return m_internalCapacity - 1; }

	/// �̈�̊m��
	void SetCapacity(int capacityCount, size_t maxMemorySize)
	{
		Clear();
		LN_SAFE_DELETE_ARRAY(mNodes);

		if (capacityCount == 0)
		{
			m_internalCapacity = 0;
			return;
		}
		m_maxMemorySize = maxMemorySize;
		if (m_maxMemorySize == 0) {
			m_maxMemorySize = ULLONG_MAX;
		}

		// ���X�g�̃I�u�W�F�N�g����́A�V�����I�u�W�F�N�g���ǉ����ꂽ��ōs���B
		// ���̂��߁A���X�g�̗v�f���� +1 �����ێ��ł���悤�ɂ���K�v������B
		m_internalCapacity = capacityCount + 1;
		mNodes = LN_NEW Node[m_internalCapacity + 1];	// �_�~�[�m�[�h�̕��� +1 (mNodes[m_internalCapacity] ���_�~�[�m�[�h)

		// �󂫔ԍ��X�^�b�N
		mIndexStack.Reserve(m_internalCapacity);
		for (int i = 0; i < m_internalCapacity; ++i)
		{
			mIndexStack.Push(i);
		}

		// m_internalCapacity �Ԃ͍ŏ��̗v�f�̑O���Ō�̗v�f�̌�ɂ���_�~�[�v�f
		mNodes[m_internalCapacity].Next = mNodes[m_internalCapacity].Prev = m_internalCapacity;
	}

	/// �N���A (�S delete �Ăяo��)
	void Clear()
	{
		if (mNodes && m_internalCapacity > 0)
		{
			Node* node = &mNodes[m_internalCapacity];	// �_�~�[�m�[�h
			uint32_t index = node->Next;             // �_�~�[�m�[�h�̎����J�n�C���f�b�N�X
			int i = 0;

			while (true)
			{
				// �_�~�[�m�[�h���w���Ă���ꍇ�͏I��
				if (index == m_internalCapacity)
				{
					break;
				}
				// �폜
				m_manager->DeleteCachedObject(mNodes[index].Value);
				// ����
				index = mNodes[index].Next;

				++i;
			}

			mNodeMap.clear();

			// �󂫔ԍ��X�^�b�N
			mIndexStack.Clear();
			for (int i = 0; i < m_internalCapacity; ++i)
			{
				mIndexStack.Push(i);
			}

			// �m�[�h�� new �Ƃ����Ă�킯����Ȃ��̂ŁA�_�~�[�̑O�ゾ���t���ς��Ă����΃��X�g�͋�̏�ԂɂȂ�
			mNodes[m_internalCapacity].Next = mNodes[m_internalCapacity].Prev = m_internalCapacity;
			mSize = 0;
			m_usedMemorySize = 0;
		}
	}

	/// �I�u�W�F�N�g��ǉ����� (�������T�C�Y�̓s���ȂǂŒǉ��ł��Ȃ������ꍇ�� false)
	bool AddObject(ICacheObject* obj)
	{
		ICacheObject::CacheObjectInfo& info = obj->GetCacheObjectInfo();

		if (obj && !info.Key.IsNull() && m_internalCapacity > 0)
		{
			// ���łɒǉ�����Ă��邩���ׂ�
			if (info.InCacheList) {
				return false;
			}
			// �I�u�W�F�N�g�P�̂̃T�C�Y���L���b�V���������ʂ𒴂��Ă���B�ǉ��ł��Ȃ�
			if (info.CacheMemorySize > m_maxMemorySize) {
				return false;
			}

			int idx = mIndexStack.GetTop();
			mIndexStack.Pop();

			Node* dummy = &mNodes[m_internalCapacity];	    // �_�~�[�m�[�h
			Node* prev = &mNodes[dummy->Prev];	// �_�~�[�̂ЂƂO ( �Ō� )
			Node* new_node = &mNodes[idx];           // ���̊Ԃɓ����m�[�h

			// new_node �����X�g�̖����ɒǉ�����
			new_node->Prev = dummy->Prev;
			new_node->Next = m_internalCapacity;
			dummy->Prev = idx;
			prev->Next = idx;

			new_node->Value = obj;

			mNodeMap.insert(std::pair< CacheKey, Node* >(info.Key, new_node));

			// �폜�҂����������t���O ON
			info.InCacheList = true;

			// �L���b�V�����������T�C�Y�𑝂₷
			m_usedMemorySize += info.CacheMemorySize;

			++mSize;

			// �L���b�V���𒲂ׂČÂ����̂�����΍폜
			CheckList();

			return true;
		}
		return false;
	}

	/// �L�[�Ɉ�v����I�u�W�F�N�g�̌��� (�Ԃ����I�u�W�F�N�g�̎Q�ƃJ�E���g�͏�� 0)             
	ICacheObject* FindObject(const CacheKey& key)
	{
		if (m_internalCapacity == 0) return NULL;

		NodeMap::iterator itr = mNodeMap.find(key);
		if (itr != mNodeMap.end())
		{
			// �폜�҂����X�g����O��
			Node* n = itr->second;
			uint32_t idx = mNodes[n->Next].Prev;   // �����̃C���f�b�N�X
			mNodes[n->Next].Prev = n->Prev;
			mNodes[n->Prev].Next = n->Next;

			// �󂫃C���f�b�N�X��߂�
			mIndexStack.Push(idx);

			ICacheObject* v = itr->second->Value;

			mNodeMap.erase(itr);

			--mSize;

			// �g�p���ɂ���
			v->GetCacheObjectInfo().InCacheList = false;

			return v;
		}
		return NULL;
	}

	/// �L���b�V���̓��e���o�͂���
	void dump(FILE* stream);

private:

	/// �L���b�V���̃��X�g�𒲂ׂāA�Â�����(�擪)������ꍇ�͍폜
	void CheckList()
	{
		// �I�u�W�F�N�g���ƃ������T�C�Y�������������Ɏ��܂�܂ō폜���J��Ԃ�
		while (mSize >= m_internalCapacity || m_usedMemorySize > m_maxMemorySize)
		{
			Node* dummy = &mNodes[m_internalCapacity];	    // �_�~�[�m�[�h
			uint32_t front_index = dummy->Next;
			Node* front = &mNodes[front_index];	// �擪�̃m�[�h���o��

			// front �����X�g����O��
			dummy->Next = front->Next;
			mNodes[front->Next].Prev = m_internalCapacity;

			// �󂢂��C���f�b�N�X���X�^�b�N�ɐς�
			mIndexStack.Push(front_index);

			// Map ������폜
			mNodeMap.erase(front->Value->GetCacheObjectInfo().Key);

			--mSize;

			// �L���b�V�����������T�C�Y�����炷
			m_usedMemorySize -= front->Value->GetCacheObjectInfo().CacheMemorySize;

			// �Ȃ��Ǝv�����ǁA�ꉞ front ���_�~�[����Ȃ����`�F�b�N���Ă�����
			if (front != dummy)
			{
				m_manager->DeleteCachedObject(front->Value);
			}
		}
	}

private:

	struct Node
	{
		int             Next;
		int             Prev;
		ICacheObject*	Value;
	};

	typedef std::map<CacheKey, Node*>    NodeMap;

private:
	CacheManager*	m_manager;
	Node*           mNodes;         ///< ����҂����X�g
	int             m_internalCapacity;      ///< �ő�v�f�� (SetCapacity �Ŏw�肳�ꂽ�T�C�Y +1)
	int             mSize;          ///< ���݂̗v�f��
	Stack<int>		mIndexStack;    ///< �󂫔ԍ��X�^�b�N
	NodeMap         mNodeMap;       ///< �L�[�ƃm�[�h�̑Ή��\
	uint64_t		m_usedMemorySize;
	uint64_t		m_maxMemorySize;
};

//=============================================================================
// ICacheObject::CacheObjectInfo
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ICacheObject::CacheObjectInfo::CacheObjectInfo()
	: Manager(NULL)
	, Key()
	, CacheMemorySize(0)
	, InCacheList(false)
	//, IsStockObject(false)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ICacheObject::CacheObjectInfo::~CacheObjectInfo()
{
	LN_SAFE_RELEASE(Manager);
}


//=============================================================================
// CacheManager
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CacheManager::CacheManager(int capacityCount, size_t maxMemorySize)
	: m_cacheUnusedList(NULL)
{
	m_cacheUnusedList = LN_NEW CacheUnusedList(this);
	m_cacheUnusedList->SetCapacity(capacityCount, maxMemorySize);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
CacheManager::~CacheManager()
{
	Finalize();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CacheManager::Finalize()
{
	ClearCache();
	LN_SAFE_DELETE(m_cacheUnusedList);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CacheManager::RegisterCacheObject(const CacheKey& key, ICacheObject* obj)
{
	Threading::MutexScopedLock lock(m_mutex);

	ICacheObject::CacheObjectInfo& info = obj->GetCacheObjectInfo();

	LN_ASSERT(!key.IsNull());					// �L�[�͒��g��������΂Ȃ�Ȃ�
	LN_ASSERT(info.Key.IsNull());		// ���ɃL�[�����蓖�Ă��Ă��� obj �͓o�^�ς݂Ȃ̂ŃG���[
	if (m_cacheUnusedList == NULL) { return; }	// ����������Ă��Ȃ���Ή������Ȃ�

	m_cacheUsingMap.insert(CacheUsingPair(key, obj));
	info.Manager = this;
	info.Manager->AddRef();
	info.Key = key;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ICacheObject* CacheManager::FindObjectAddRef(const CacheKey& key)
{
	Threading::MutexScopedLock lock(m_mutex);

	ICacheObject* obj = NULL;
	if (!key.IsNull() && m_cacheUnusedList != NULL)
	{
		// �܂��͍��g���Ă镨������
		CacheUsingMap::iterator itr = m_cacheUsingMap.find(key);
		if (itr != m_cacheUsingMap.end())
		{
			obj = itr->second;
			obj->AddRef();
		}
		// �L���b�V�����X�g������
		else
		{
			obj = m_cacheUnusedList->FindObject(key);
			if (obj != NULL)
			{
				// �g�p���I�u�W�F�N�g�}�b�v�ɓ���Ă���
				m_cacheUsingMap.insert(CacheUsingPair(obj->GetCacheObjectInfo().Key, obj));
				obj->AddRef();
			}
		}
	}
	return obj;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CacheManager::ClearCache()
{
	Threading::MutexScopedLock lock(m_mutex);
	if (m_cacheUnusedList != NULL) {	// Finalize() �ς݃`�F�b�N
		m_cacheUnusedList->Clear();
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//void CacheManager::clearStockList()
//{
//	Threading::ScopedLock lock(mMutex);
//	mClearingCacheList = true;
//	LN_FOREACH(ICacheObject* obj, mStockObjectArray)
//	{
//		obj->Release();
//	}
//	mStockObjectArray.clear();
//	mClearingCacheList = false;
//}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CacheManager::AddCacheUnusedList(ICacheObject* obj)
{
	LN_ASSERT(obj != NULL);
	Threading::MutexScopedLock lock(m_mutex);

	bool addedObj = false;
	if (m_cacheUnusedList != NULL)	// Finalize() �ς݃`�F�b�N
	{
		m_cacheUsingMap.erase(obj->GetCacheObjectInfo().Key);
		addedObj = m_cacheUnusedList->AddObject(obj);
	}

	// �ǉ��ł��Ȃ�������A�Q�ƃJ�E���g 0 �Ȃ̂ɃL���b�V���ɓ���Ȃ������Ƃ������ƁBdelete ����
	if (!addedObj) {
		DeleteCachedObject(obj);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CacheManager::DeleteCachedObject(ICacheObject* obj)
{
	// ���̊֐��� AddCacheUnusedList() => m_cacheUnusedList->AddObject �o�R�ŌĂ΂��̂ŁA
	// ���b�N����悤�Ȏd�l���K�v�ɂȂ����Ƃ��͒��ӂ��邱�ƁB

	LN_ASSERT(obj != NULL);
	LN_ASSERT(obj->GetCacheObjectInfo().Manager != NULL);

	delete obj;
}

} // namespace Lumino
