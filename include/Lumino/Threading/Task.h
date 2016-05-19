
#pragma once
#include "../Base/Exception.h"
#include "../Base/RefObject.h"
#include "../Base/Delegate.h"
#include "ConditionFlag.h"

LN_NAMESPACE_BEGIN
namespace tr
{
class Task;
typedef RefPtr<Task>	TaskPtr;

enum class TaskStatus
{
	Created,	/**< Task �I�u�W�F�N�g�͐�������Ă��邪�A�X�P�W���[���ɒǉ�����Ă��Ȃ��B*/

	Waiting,	/**< �X�P�W���[���ɒǉ�����A���s�����̂�ҋ@���B*/

	Running,	/**< ���s���B*/

	Completed,	/**< �����͐���Ɋ��������B*/

	Faulted,	/**< ���s���Ƀn���h������Ȃ���O���������ďI�������B*/
};

/**
	@brief	
*/
class Task
	: public RefObject
{
public:

	static TaskPtr Create(const Delegate<void()>& action);

	static TaskPtr Run(const Delegate<void()>& action);

	void Start();

	void Wait();

	/** ���� Task �̌��݂̏�Ԃ��擾���܂��B*/
	TaskStatus GetStatus() const;

	/** ���� Task �������������ǂ������m�F���܂��B*/
	bool IsCompleted() const;

	/** ���s���Ƀn���h������Ȃ���O�������������Ƃ������� Task ���I�����������m�F���܂��B*/
	bool IsFaulted() const;

	/** ���s���ɔ������n���h������Ȃ�������O��Ԃ��܂��B��O���������Ă��Ȃ���� nullptr �ł��B*/
	Exception* GetException() const;

LN_INTERNAL_ACCESS:
	void Execute();

private:
	Task(const Delegate<void()>& action);	// ���� Run ���炾���l����
	~Task();

	Delegate<void()>	m_action;
	TaskStatus		m_status = TaskStatus::Created;
	Exception*		m_exception = nullptr;
	ConditionFlag	m_waiting;
};

} // namespace tr
LN_NAMESPACE_END
