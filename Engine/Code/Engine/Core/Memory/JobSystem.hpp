#pragma once
#include "Engine/Core/Memory/ThreadSafeQueue.hpp"
#include "Engine/Core/Memory/ObjectPool.hpp"



#include <vector>
#include <atomic>
class Thread;
class JobSystem;
class Job;


extern JobSystem* g_theJobSystem;


typedef unsigned char byte_t;
typedef void(JobCallBack)(Job*);


enum eJob_Categories
{
	GENERIC = 0, //Can run on main thread
	GENERIC_SLOW, //Shouldn't run on main thread, but can
	NUM_OF_JOB_CATEGORIES
};

class Job
{
private:
public:

	Job();
	eJob_Categories m_jobCategory;
	std::atomic<int> m_refCount;
	byte_t m_byteBuffer[512];
	unsigned int m_readHead;
	unsigned int m_writeHead;
	JobCallBack* m_callbackFunc;

	template <typename T>
	void JobWrite(T const &v)
	{
		memcpy(m_byteBuffer + m_writeHead, &v, sizeof(T));
		m_writeHead += sizeof(T);
	}

	template <typename T>
	T JobRead()
	{
		T retData;

		memcpy(&retData, m_byteBuffer + m_readHead, sizeof(T));
		m_readHead += sizeof(T);
		return retData;
	}
	void EndJobRead();
	void EndJobWrite();

	void DoWork();
	void FinishJob();
};

class JobConsumer
{
public:
	//List of queues I care about. Do not make new queues
	std::vector<eJob_Categories> m_QueuesICareAbout;

	JobConsumer();
	void AddCategory(eJob_Categories category);
	void ConsumeAll();
	bool Consume();
	void FinishJob(Job* job);
};

class JobSystem
{
public:

	std::vector<ThreadSafe_Queue<Job*>*> m_jobQueue;
	std::vector<Thread> m_threads;
	ObjectPool<Job> m_JobPool;
	bool m_isRunning;
	//std::atomic<bool> s_isAlternate;

	JobSystem();

	unsigned int SystemGetCoreCount();
	void JobSystemStartUp(int numThreadsToUse);
	void JobSystemShutdown();
	Job* JobCreate(eJob_Categories category, JobCallBack* callback);
	void JobDispatch(Job* job);
	void JobDetatch(Job* job);
	void JobJoin(Job* job);
	//void JobFinish();

	
	ThreadSafe_Queue<Job*>* GetQueueForCategory(eJob_Categories cat);


};

void JobThread();