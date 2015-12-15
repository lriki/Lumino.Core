
#include "../Internal.h"
#include <Lumino/Threading/TaskScheduler.h>

LN_NAMESPACE_BEGIN
namespace tr
{

//=============================================================================
// TaskScheduler
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TaskScheduler* TaskScheduler::GetDefault()
{
	static TaskScheduler instance(4);
	return &instance;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TaskScheduler::TaskScheduler(int threadCount)
	: m_semaphore(0, INT_MAX)
	, m_endRequested(false)
{
	m_threadList.Reserve(threadCount);
	for (int i = 0; i < threadCount; ++i)
	{
		auto thr = LN_NEW Threading::DelegateThread();
		thr->Start(CreateDelegate(this, &TaskScheduler::ExecuteThread));
		m_threadList.Add(thr);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
TaskScheduler::~TaskScheduler()
{
	m_endRequested.SetTrue();		// �I���v�����o���āA

	for (auto& thr : m_threadList)	// �X���b�h�̐������Z�}�t�H���₵�đS���N�����āA
	{
		m_semaphore.Unlock();
	}
	for (auto& thr : m_threadList)	// �S���I���܂ő҂�
	{
		thr->Wait();
		LN_SAFE_DELETE(thr);
	}
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
int TaskScheduler::GetMaxConcurrencyLevel() const
{
	return m_threadList.GetCount();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TaskScheduler::QueueTask(Task* task)
{
	LN_CHECK_ARGS_RETURN(task != nullptr);

	Threading::MutexScopedLock lock(m_taskQueueLock);
	m_taskQueue.Enqueue(task);
	task->AddRef();

	m_semaphore.Unlock();	// �L���[�ɓ��ꂽ�̂Ŏ��o�������l�͂ǂ����B
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void TaskScheduler::ExecuteThread()
{
	while (true)
	{
		m_semaphore.Lock();	// �L���[�ɉ����ǉ������܂ő҂B�܂��͏I���v���܂ŁB

		// �I���v�������Ă����炨���܂�
		if (m_endRequested.IsTrue()) {
			break;
		}

		// �L���[����1���o��
		Task* task = nullptr;
		{
			Threading::MutexScopedLock lock(m_taskQueueLock);
			m_taskQueue.Dequeue(&task);
		}

		// ���s�B��ԕω��͓����ōs��
		task->Execute();
		task->Release();
	}
}

} // namespace tr
LN_NAMESPACE_END
