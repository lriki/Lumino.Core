
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

	/** この TaskScheduler が同時に並列実行できる Task の数を取得します。*/
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
	Threading::ConditionFlag			m_endRequested;
};

} // namespace tr
LN_NAMESPACE_END
