#include <TestConfig.h>
using namespace Lumino::Threading;

class Test_Threading_Atomic : public ::testing::Test
{
protected:
	virtual void SetUp() {}
	virtual void TearDown() {}
};

//-----------------------------------------------------------------------------
TEST_F(Test_Threading_Atomic, Basic)
{
	class IncrThread : public Thread
	{
	public:
		Atomic* value;
		Atomic* valueIncr;
		virtual void Execute()
		{
			for (int i = 0; i < 1000; i++)
			{
				value->Increment();
				valueIncr->Increment();
			}
		}
	};

	class DecrThread : public Thread
	{
	public:
		Atomic* value;
		Atomic* valueDecr;
		virtual void Execute()
		{
			for (int i = 0; i < 1000; i++)
			{
				value->Decrement();
				valueDecr->Decrement();
			}
		}
	};

	Atomic value;		// ++ �� -- ���J��Ԃ�
	Atomic valueIncr;	// ++ ����
	Atomic valueDecr;	// -- ����
	value.Set(10);			// �����l

	// ++ �� -- �X���b�h�� 10 �����点��
	const int ThreadCount = 10;
	IncrThread incrThreads[ThreadCount];
	DecrThread decrThreads[ThreadCount];
	for (int i = 0; i < ThreadCount; i++) 
	{
		incrThreads[i].value = &value;
		incrThreads[i].valueIncr = &valueIncr;
		incrThreads[i].Start();
		decrThreads[i].value = &value;
		decrThreads[i].valueDecr = &valueDecr;
		decrThreads[i].Start();
	}
	// �I���ҋ@
	for (int i = 0; i < ThreadCount; i++)
	{
		incrThreads[i].Wait();
		decrThreads[i].Wait();
	}

	ASSERT_EQ(10, value.Get());
	ASSERT_EQ(ThreadCount * 1000, valueIncr.Get());
	ASSERT_EQ(-ThreadCount * 1000, valueDecr.Get());
}

