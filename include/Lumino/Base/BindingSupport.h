
#pragma once
#include "Array.h"

namespace Lumino
{
	
/**
	@brief	
*/
class RefObjectList
	: public RefObject
{
public:
	RefObjectList() {}
	virtual ~RefObjectList() { Clear(); }

public:

	/// �v�f�����擾����
	int GetCount() const { return m_list.GetCount(); }

	/// �w��C���f�b�N�X�ɗv�f���i�[����
	void SetAtBase(int index, RefObject* item)
	{
		if (OnItemAdding(item))
		{
			m_list.SetAt(index, item);
			LN_SAFE_ADDREF(item);
			OnItemAdded(item);
		}
	}

	/// �w��C���f�b�N�X�̗v�f���擾����
	RefObject* GetAtBase(int index) const
	{
		return m_list.GetAt(index);
	}

	/// �v�f�𖖔��ɒǉ�����
	void Add(RefObject* item)
	{
		if (OnItemAdding(item))
		{
			m_list.Add(item);
			LN_SAFE_ADDREF(item);
			OnItemAdded(item);
		}
	}

	/// �S�Ă̗v�f���폜����
	void Clear()
	{
		LN_FOREACH(RefObject* item, m_list) {
			OnItemRemoved(item);	// TODO: erase ���Ȃ���ЂƂ��ĂԂׂ�����
			LN_SAFE_RELEASE(item);
		}
		m_list.Clear();
	}

	/// �w�肵���C���f�b�N�X�̈ʒu�ɗv�f��}������
	void Insert(int index, RefObject* item)
	{
		if (OnItemAdding(item))
		{
			m_list.Insert(index, item);
			LN_SAFE_ADDREF(item);
			OnItemAdded(item);
		}
	}

	/// item �ƈ�v����ŏ��̗v�f���폜����
	void Remove(RefObject* item)
	{
		bool b = m_list.Remove(item);
		if (b) {
			OnItemRemoved(item);
			LN_SAFE_RELEASE(item);
		}
	}

	/// �w�肵���C���f�b�N�X�̗v�f���폜����
	void RemoveAt(int index)
	{
		RefObject* item = m_list.GetAt(index);
		m_list.RemoveAt(index);
		OnItemRemoved(item);
		LN_SAFE_RELEASE(item);
	}

protected:
	bool OnItemAdding(RefObject* item) { return true; }
	void OnItemAdded(RefObject* item) {}
	void OnItemRemoved(RefObject* item) {}

private:
	ArrayList<RefObject*>	m_list;
};

#define LN_REF_OBJECT_LIST_DECL(itemType) \
	void SetAt(int index, itemType* item) { RefObjectList::SetAtBase(index, item); } \
	itemType* GetAt(int index) const { return static_cast<itemType*>(RefObjectList::GetAtBase(index)); }


} // namespace Lumino
