#include "Engine/Core/Memory/JobSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Memory/Thread.hpp"

#include <thread>


JobSystem* g_theJobSystem = nullptr;

JobSystem::JobSystem()
	:m_JobPool(1024)
	, m_isRunning(false)
{
	m_jobQueue.resize(NUM_OF_JOB_CATEGORIES);

	for (int j = 0; j < eJob_Categories::NUM_OF_JOB_CATEGORIES; j++)
	{
		m_jobQueue[j] = new ThreadSafe_Queue<Job*>();
	}
}

unsigned int JobSystem::SystemGetCoreCount()
{
	return (unsigned int)std::thread::hardware_concurrency();
}

void JobSystem::JobSystemStartUp(int numThreadsToUse)
{
	m_isRunning = true;
	//Create all Queues and Threads
	int threadsToUse = numThreadsToUse;

	if (numThreadsToUse < 0)
	{
		threadsToUse = SystemGetCoreCount() + numThreadsToUse;
		ASSERT_OR_DIE(threadsToUse > 0, "ERROR: Negative thread count");
	}

	if (threadsToUse < 0)
	{
		threadsToUse = 1;
	}

	for (int i = 0; i < threadsToUse; i++)
	{
		m_threads.push_back(Thread(&(JobThread)));
		
		
	}

	
}

void JobSystem::JobSystemShutdown()
{
	//Join All threads & cleanup
	m_isRunning = false;
	for (unsigned int i = 0; i < m_threads.size(); i++)
	{
		m_threads[i].Join();
	}
	m_JobPool.Destroy();

	for (unsigned int j = 0; j > eJob_Categories::NUM_OF_JOB_CATEGORIES; j++)
	{
		delete m_jobQueue[j];
	}

}

Job* JobSystem::JobCreate(eJob_Categories category, JobCallBack* callback)
{
	Job* newJob = m_JobPool.Alloc();

	newJob->m_callbackFunc = callback;
	newJob->m_jobCategory = category;
	newJob->m_readHead = 0;
	newJob->m_writeHead = 0;
	newJob->m_refCount = 1;

	return newJob;
}

void JobSystem::JobDispatch(Job* job)
{
	ThreadSafe_Queue<Job*>* jobQueue = GetQueueForCategory(job->m_jobCategory);
	job->m_refCount++;
	jobQueue->PushBack(job);
}

void JobSystem::JobDetatch(Job* job)
{
	job->m_refCount--;
	if (job->m_refCount == 0)
	{
		m_JobPool.Delete(job);
	}
}

void JobSystem::JobJoin(Job* job)
{
	while (job->m_refCount == 2);
	JobDetatch(job);
}

void JobThread()
{
	JobConsumer consumer = JobConsumer();

	//#TODO Make this alternate between SLOW,GENERIC and  GENERIC,SLOW
	consumer.AddCategory(GENERIC_SLOW);
	consumer.AddCategory(GENERIC);

	while (g_theJobSystem->m_isRunning)
	{
		consumer.ConsumeAll();
		std::this_thread::yield();
	}

	consumer.ConsumeAll(); //Cleanup
}

ThreadSafe_Queue<Job*>* JobSystem::GetQueueForCategory(eJob_Categories cat)
{
	return m_jobQueue[cat];
}

JobConsumer::JobConsumer()
{

}

void JobConsumer::AddCategory(eJob_Categories category)
{
	m_QueuesICareAbout.push_back(category);
}

void JobConsumer::ConsumeAll()
{
	while (Consume());
}

bool JobConsumer::Consume()
{
	for each (eJob_Categories cat in m_QueuesICareAbout)
	{
		ThreadSafe_Queue<Job*>* thisQueue = g_theJobSystem->GetQueueForCategory(cat);
		Job* thisJob;
		if (thisQueue->PopFront(&thisJob))
		{
			thisJob->DoWork();
			thisJob->FinishJob();
			//FinishJob(job)
			return true;
		}
	}
	return false;
}

void JobConsumer::FinishJob(Job* job)
{
	g_theJobSystem->JobDetatch(job);
}

Job::Job()
{

}

void Job::EndJobRead()
{
	m_readHead = 0;
}

void Job::EndJobWrite()
{
	m_writeHead = 0;	
}

void Job::DoWork()
{
	(*m_callbackFunc)(this);
}

void Job::FinishJob()
{
	m_refCount--;
}
