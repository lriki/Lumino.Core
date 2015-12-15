
#pragma once
#include <list>
#include "../Base/Array.h"
#include "../Base/Queue.h"
#include "Task.h"
#include "Thread.h"
#include "Mutex.h"
#include "Semaphore.h"

LN_NAMESPACE_BEGIN
class Semaphore;
namespace tr
{

/**
	@brief	
*/
class TaskScheduler
{
public:
	static TaskScheduler* GetDefault();

	/** ‚±‚Ì TaskScheduler ‚ª“¯‚É•À—ñÀs‚Å‚«‚é Task ‚Ì”‚ğæ“¾‚µ‚Ü‚·B*/
	int GetMaxConcurrencyLevel() const;

LN_INTERNAL_ACCESS:
	void QueueTask(Task* task);

private:
	TaskScheduler(int threadCount);
	~TaskScheduler();
	void ExecuteThread();

	Array<Threading::DelegateThread*>	m_threadList;
	Queue<Task*>						m_taskQueue;
	Semaphore							m_semaphore;
	Threading::Mutex					m_taskQueueLock;
	Threading::EventFlag				m_endRequested;
};

} // namespace tr
LN_NAMESPACE_END
