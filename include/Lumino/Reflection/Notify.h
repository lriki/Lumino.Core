
#pragma once
#include "Common.h"
#include "../Base/List.h"
#include "../Base/String.h"
#include "../Base/Delegate.h"

LN_NAMESPACE_BEGIN
namespace tr
{
	
/**
	@brief		
*/
class ReflectionEventInfo
{
public:
	// Event �𒼐ڎQ�Ƃ��Ă͂Ȃ�Ȃ��B���̃N���X�� Property �Ɠ������A������ UIElement �ŋ��L�����B��Ԃ������Ă͂Ȃ�Ȃ��B
	// �Ȃ̂ŁA�C�x���g�� Raise ����֐��|�C���^���Q�Ƃ���B

	typedef void(*RaiseEventFunc)(ReflectionObject* target, ReflectionEventArgs* e);

public:
	ReflectionEventInfo(TypeInfo* ownerClass, const TCHAR* name, RaiseEventFunc raiseEvent);
	~ReflectionEventInfo();

	const String& GetName() const { return m_name; }
	void CallEvent(ReflectionObject* target, ReflectionEventArgs* e) const { m_raiseEvent(target, e); }

private:
	friend class TypeInfo;
	String			m_name;
	RaiseEventFunc	m_raiseEvent;
	bool			m_registerd;
};


/**
	@brief		
*/
class ReflectionEventBase
{
protected:
	ReflectionEventBase() = default;
	virtual ~ReflectionEventBase() = default;

private:
	friend class ReflectionObject;
	virtual void Raise(ReflectionEventArgs* e) const = 0;
};

/**
	@brief		
	@details	
*/
template<class TArgs>
class ReflectionEvent
	: public ReflectionEventBase
{
public:
	ReflectionEvent() = default;
	virtual ~ReflectionEvent() = default;

public:

	/**
		@brief	���[�e�B���O�C�x���g�̃n���h����ǉ����܂��B
	*/
	void AddHandler(const Delegate<void(TArgs*)>& handler)
	{
		m_handlerList.Add(handler);
	}
	
	/**
		@brief	�w�肵���n���h���Ɉ�v����n���h�����A���̃X���b�g����폜���܂��B
	*/
	void RemoveHandler(const Delegate<void(TArgs*)>& handler)
	{
		m_handlerList.Remove(handler);
	}
	
	/**
		@brief	���[�e�B���O�C�x���g�̃n���h����ǉ����܂��B
	*/
	void operator += (const std::function<void(TArgs*)>& handler)
	{
		AddHandler(handler);
	}

	void operator += (const Delegate<void(TArgs*)>& handler)
	{
		m_handlerList.Add(handler);
	}
	
	/**
		@brief	�w�肵���n���h���Ɉ�v����n���h�����A���̃X���b�g����폜���܂��B
	*/
	void operator -= (const Delegate<void(TArgs*)>& handler)
	{
		m_handlerList.Remove(handler);
	}

private:
	List< Delegate<void(TArgs*)> > m_handlerList;

	virtual void Raise(ReflectionEventArgs* e) const override
	{
		for (const Delegate<void(TArgs*)>& d : m_handlerList)
		{
			d.Call(static_cast<TArgs*>(e));
		}
	}
};


#define LN_REFLECTION_EVENT_COMMON(typeInfo, eventArgs, eventInfoVar) \
	public:  static const typeInfo* eventInfoVar; \
	private: static void		_raise_##eventInfoVar(tr::ReflectionObject* obj, tr::ReflectionEventArgs* e); \
	private: static typeInfo	_init_##eventInfoVar;

#define LN_REFLECTION_EVENT_IMPLEMENT_COMMON(typeInfo, ownerClass, eventArgs, eventInfoVar, name, ev) \
	typeInfo					ownerClass::_init_##eventInfoVar(tr::TypeInfo::GetTypeInfo<ownerClass>(), _T(name), &ownerClass::_raise_##eventInfoVar); \
	const typeInfo*				ownerClass::eventInfoVar = &_init_##eventInfoVar; \
	void						ownerClass::_raise_##eventInfoVar(tr::ReflectionObject* obj, tr::ReflectionEventArgs* e) { static_cast<ownerClass*>(obj)->RaiseReflectionEvent(static_cast<ownerClass*>(obj)->ev, static_cast<eventArgs*>(e)); }

#define LN_REFLECTION_EVENT(eventArgs, eventInfoVar) \
	LN_REFLECTION_EVENT_COMMON(tr::ReflectionEventInfo, eventArgs, eventInfoVar);

#define LN_REFLECTION_EVENT_IMPLEMENT(ownerClass, eventArgs, eventInfoVar, name, ev) \
	LN_REFLECTION_EVENT_IMPLEMENT_COMMON(tr::ReflectionEventInfo, ownerClass, eventArgs, eventInfoVar, name, ev);






/**
	@brief		���[�e�B���O�t���[�����[�N�𔺂�Ȃ��C�x���g�B
*/
//template<typename>
//class DelegateEvent {};
template<typename... TArgs>
class DelegateEvent/*<void(TArgs...)>*/		// �߂�l�� void �Œ�B�n���h����1���o�^����Ă��Ȃ��Ƃ��͉���Ԃ��΂����̂�������Ȃ����߁B
{
public:
	typedef Delegate<void(TArgs...)> DelegateType;

	void AddHandler(const DelegateType& handler)
	{
		m_handlerList.Add(handler);
	}

	void AddHandler(const std::function<void(TArgs...)>& handler)	// void operator += (const DelegateType& handler) �������ƈÖٕϊ����������R���p�C���G���[�ƂȂ��Ă������ߗp��
	{
		AddHandler(DelegateType(handler));
	}

	void RemoveHandler(const DelegateType& handler)
	{
		m_handlerList.Remove(handler);
	}

	void operator += (const DelegateType& handler)
	{
		AddHandler(handler);
	}

	void operator += (const std::function<void(TArgs...)>& handler)	// void operator += (const DelegateType& handler) �������ƈÖٕϊ����������R���p�C���G���[�ƂȂ��Ă������ߗp��
	{
		AddHandler(DelegateType(handler));
	}

	void operator -= (const DelegateType& handler)
	{
		RemoveHandler(handler);
	}

private:
	friend class ReflectionObject;

	List<DelegateType> m_handlerList;

	void Clear()
	{
		m_handlerList.Clear();
	}

	bool IsEmpty() const
	{
		return m_handlerList.IsEmpty();
	}

	void Raise(TArgs... args)
	{
		int count = m_handlerList.GetCount();
		if (count > 0)
		{
			for (int i = 0; i < count - 1; ++i)
			{
				m_handlerList[i].Call(args...);
			}
			m_handlerList[count - 1].Call(args...);	// �߂�l��߂��͍̂Ō��1�B(.NET �̓���)
		}
	}
};





} // namespace tr
LN_NAMESPACE_END
