
#pragma once
#include "../Base/SortedArray.h"
#include "ReflectionEventArgs.h"

LN_NAMESPACE_BEGIN
namespace tr
{

/**
	@brief	
	@note	���̃N���X�̓C�x���g�����̕p�ɂ� new ������邽�߂Ɏg�p����B
			�Ⴆ�� MouseMove �C�x���g�͍ň����t���[���̂悤�ɔ�������\��������Anew �͑傫�ȃI�[�o�[�w�b�h�ɂȂ�B
			����Ȃ� union �𗘗p�����C�x���g�����\���̂��g���̂��ЂƂ̎肩������Ȃ��B
			
			�������A�{���C�u������ C# �� Ruby ���A���̌���ւ̃o�C���f�B���O��z�肵�Ă���B
			���R GUI ���W���[�������J����A�g������邱�Ƃ��O��B
			C# ���ō쐬�������[�U�[�R���g���[���� MouseEventArgs ���g�p���邱�Ƃ͑z�肵�Ȃ���΂Ȃ�Ȃ��B

			union �� struct �ɂ����ꍇ�A����͂���Ō���ʂɗ]�v�ȃI�[�o�[�w�b�h�⍇�킹���݂̎����������邱�ƂɂȂ�B
			�Ⴆ�� C# ��B�͒l�n���̂܂܂ł悢�̂��Aref ������̂��BRuby �� struct �͎g���Ȃ��̂Ō��� new ����H�ڂɂȂ邪�����̂��B

			Pool ���Ă����΁A�኱�����I�ł͂Ȃ��Ȃ邪�A�o�C���_���� new ���}���邱�Ƃ��ł���B

			��X�C�x���g�̎�ނ������Ă����Ƃ��͊g�����̂��߁A�C�x���g�����L�[�ɂ��� Create ����悤�Ȏd�g�݂��K�v�ɂȂ邩������Ȃ��B
*/
class EventArgsPool
{
public:
	EventArgsPool() = default;
	~EventArgsPool()
	{
		for (auto list : m_pool)
		{
			for (auto e : (*list.second))
			{
				e->Release();
			}
			delete list.second;
		}
	}

	template<class TEventArgs, typename ...TArgs>
	TEventArgs* Create(TArgs... args)
	{
		TEventArgs* e = static_cast<TEventArgs* >(Find(tr::TypeInfo::GetTypeInfo<TEventArgs>()));
		if (e == NULL) {
			e = LN_NEW TEventArgs(args...);
			Register(e);
		}
		else {
			e->~TEventArgs();
			new (e)TEventArgs(args...);
		}
		e->handled = false;
		e->AddRef();
		return e;
	}

private:
	typedef Array<ReflectionEventArgs*>					EventArgsList;
	typedef SortedArray<tr::TypeInfo*, EventArgsList*>	EventArgsMap;
	EventArgsMap	m_pool;

	ReflectionEventArgs* Find(tr::TypeInfo* typeId)
	{
		EventArgsList* list;
		if (m_pool.TryGetValue(typeId, &list))
		{
			for (auto e : (*list))
			{
				if (e->GetRefCount() == 1) {	// ���̃��X�g���炵���Q�Ƃ���Ă��Ȃ���ΕԂ�
					return e;
				}
			}
		}
		return NULL;
	}

	void Register(ReflectionEventArgs* e)
	{
		EventArgsList* list;
		if (!m_pool.TryGetValue(tr::TypeInfo::GetTypeInfo(e), &list))
		{
			list = LN_NEW EventArgsList();
			m_pool.Add(tr::TypeInfo::GetTypeInfo(e), list);
		}
		list->Add(e);
	}


	//Array<MouseEventArgs*>	m_mouseEventArgsPool;





	template<class T>
	T FindFreeObject(const Array<T>& pool)
	{
		LN_FOREACH(T a, pool)
		{
			if (a->GetRefCount() == 1) {
				return a;
			}
		}
		return NULL;
	}

	//Array<MouseEventArgs*>	m_mouseEventArgsPool;
	//Array<KeyEventArgs*>	m_keyEventArgsPool;
};

} // namespace tr
LN_NAMESPACE_END
