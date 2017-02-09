#pragma once
#include <mutex>

//class CRITICAL_SECTION;
class CriticalSection
{
public: 
	CriticalSection();
	void lock();
	void unlock();
	bool try_lock(); //if lock returns true, else false

	CriticalSection(CriticalSection const &copy) = delete; //Removes the copy constructor. Woah.

 	std::recursive_mutex mutex;
// 	//or
	//CRITICAL_SECTION cs;

};