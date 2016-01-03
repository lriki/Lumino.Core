
#pragma once
#include "Common.h"
#include "../Base/Array.h"
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
	ReflectionEventInfo(TypeInfo* ownerClass, const String& name, RaiseEventFunc raiseEvent);
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
	Array< Delegate<void(TArgs*)> > m_handlerList;

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
	typeInfo					ownerClass::_init_##eventInfoVar(tr::TypeInfo::GetTypeInfo<ownerClass>(), name, &ownerClass::_raise_##eventInfoVar); \
	const typeInfo*				ownerClass::eventInfoVar = &_init_##eventInfoVar; \
	void						ownerClass::_raise_##eventInfoVar(tr::ReflectionObject* obj, tr::ReflectionEventArgs* e) { static_cast<ownerClass*>(obj)->RaiseReflectionEvent(static_cast<ownerClass*>(obj)->ev, static_cast<eventArgs*>(e)); }

#define LN_REFLECTION_EVENT(eventArgs, eventInfoVar) \
	LN_REFLECTION_EVENT_COMMON(tr::ReflectionEventInfo, eventArgs, eventInfoVar);

#define LN_REFLECTION_EVENT_IMPLEMENT(ownerClass, eventArgs, eventInfoVar, name, ev) \
	LN_REFLECTION_EVENT_IMPLEMENT_COMMON(tr::ReflectionEventInfo, ownerClass, eventArgs, eventInfoVar, name, ev);

} // namespace tr
LN_NAMESPACE_END
