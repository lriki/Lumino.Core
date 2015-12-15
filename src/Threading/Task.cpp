
#include <atomic>
#include "../Internal.h"
#include <Lumino/Threading/Task.h>
#include <Lumino/Threading/TaskScheduler.h>
#include <Lumino/Threading/ThreadingExceptions.h>

LN_NAMESPACE_BEGIN
namespace tr
{

//=============================================================================
// Task
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TaskPtr Task::Run(const Delegate<void()>& action)
{
	TaskPtr task(LN_NEW Task(action), false);
	TaskScheduler::GetDefault()->QueueTask(task);
	return task;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Task::Task(const Delegate<void()>& action)
	: m_action(action)
	, m_exception(nullptr)
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Task::~Task()
{
	LN_SAFE_DELETE(m_exception);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TaskStatus Task::GetStatus() const
{
	return m_status;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Task::IsCompleted() const
{
	//std::atomic_thread_fence(std::memory_order_acquire);
	return m_status == TaskStatus::Completed;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
bool Task::IsFaulted() const
{
	return m_status == TaskStatus::Faulted;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Exception* Task::GetException() const
{
	return m_exception;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Task::Execute()
{
	m_status = TaskStatus::Running;
	try
	{
		m_action.Call();
		//printf("a\n");
		m_status = TaskStatus::Completed;
		//std::atomic_thread_fence(std::memory_order_release);
	}
	catch (Exception& e)
	{
		m_exception = e.Copy();
		//m_status = TaskStatus::Faulted;
	}
	catch (...)
	{
		m_exception = LN_NEW Threading::ThreadException();
		//m_status = TaskStatus::Faulted;
	}
}

} // namespace tr
LN_NAMESPACE_END
