#include "Engine/Core/Memory/CriticalSection.hpp"

#include <windows.h>

void CriticalSection::lock()
{
	mutex.lock();
}

void CriticalSection::unlock()
{
	mutex.unlock();
}

bool CriticalSection::try_lock()
{
	return mutex.try_lock();
}

CriticalSection::CriticalSection()
{

}
