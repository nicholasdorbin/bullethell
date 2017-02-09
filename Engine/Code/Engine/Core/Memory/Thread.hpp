#pragma once
#include <thread>

typedef void(ThreadFP)();
class Thread
{
	private:
		std::thread m_thread;
	public:
		Thread();
		Thread(ThreadFP* funcPtr);
		void Detach();
		void Join();
		void SleepFor(float time);
		void yield();
};