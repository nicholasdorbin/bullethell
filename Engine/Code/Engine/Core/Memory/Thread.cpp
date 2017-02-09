#include "Engine/Core/Memory/Thread.hpp"



Thread::Thread(ThreadFP* funcPtr)
{
	m_thread = std::thread(funcPtr);
}

Thread::Thread()
{

}

void Thread::Detach()
{
	m_thread.detach();
}

void Thread::Join()
{
	m_thread.join();
}

void Thread::SleepFor(float time)
{
	std::this_thread::sleep_for(std::chrono::milliseconds((long)time));
}

void Thread::yield()
{
	//std::thread::this_thread::yield();
	std::this_thread::yield();
}
