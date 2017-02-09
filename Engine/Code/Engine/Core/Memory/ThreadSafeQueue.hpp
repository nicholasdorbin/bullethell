#pragma once
#include <queue>
#include "Engine/Core/Memory/CriticalSection.hpp"
template<typename Type>
class ThreadSafe_Queue : protected std::queue<Type> //inheriting private. Could do protected if names were similar to queue's
{

	private:
		CriticalSection criticalSection;
	public:

		ThreadSafe_Queue()
		{

		}
		void PushBack(Type const & value)
		{
			criticalSection.lock();
			push(value);
			criticalSection.unlock();
		}


		//---------------------------------------------------------------------------------------------
		bool PopFront(Type * out_value)
		{
			bool result = false;
			criticalSection.lock();
			if (!empty())
			{
				*out_value = front();
				pop();
				result = true;
			}
			criticalSection.unlock();
			return result;
		}
};