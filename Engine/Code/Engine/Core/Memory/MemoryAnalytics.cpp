#include "Engine/Core/Memory/MemoryAnalytics.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineBuildConfig.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/HashUtils.hpp"


#pragma warning( disable : 4127 )  // conditional expression is constant

#ifdef MEMORY_TRACKER

int g_NumberOfAllocations = 0;
unsigned int g_StartAllocated = 0;
unsigned int g_TotalAllocated = 0;
unsigned int g_MaxAllocated = 0;
unsigned int g_BytesAllocedPerSec = 0;
unsigned int g_BytesFreedPerSec = 0;
unsigned int g_BytesAllocedLastTick = 0;
unsigned int g_BytesFreedLastTick = 0;
float g_timestampMemory = 0.f;
float g_timestampMemoryVerbose = 0.f;
bool g_trackAllocations = false;
CriticalSection g_callstackCritSection;

std::map<void*, CallStack*, std::less<void*>, TUntrackedAllocator<std::pair<void*, CallStack*>>> g_callstackMap;
std::map<uint32_t, CallStackStats, std::less<uint32_t>, TUntrackedAllocator<std::pair<uint32_t, CallStackStats>>> g_callstackHashMap;
void* operator new (const size_t numberOfBytes)
{
	size_t *ptr = (size_t*)malloc(numberOfBytes + sizeof(size_t));
	*ptr = numberOfBytes;
	ptr++;
	
	if (g_trackAllocations) //is verbose check
	{
	

		g_callstackCritSection.lock();
		{
			++g_NumberOfAllocations;
			g_TotalAllocated += numberOfBytes;
			g_BytesAllocedPerSec += numberOfBytes;
			if (g_MaxAllocated < g_TotalAllocated)
				g_MaxAllocated = g_TotalAllocated;
		}
		g_callstackCritSection.unlock();

		



		//if verbose
		if (MEMORY_TRACKER == 1)
		{
			CallStack* cs = CallstackFetch(1);
			g_callstackCritSection.lock();
			{
				g_callstackMap.insert(std::pair<void*, CallStack*>(ptr, cs)); //Verbose
			}
			g_callstackCritSection.unlock();
		}

	}



	/*
	What we just did was add a size_t to the front,
	Put the amount of bytes there, then shifted the ptr to the
	Right by size_t to return the proper data
	*/
	return ptr;
}

void operator delete (void *ptr)
{
	size_t* ptr_size = (size_t*)ptr;
	--ptr_size;
	size_t numberOfBytes = *ptr_size;

	//if (MEMORY_TRACKER == 1)

	if (g_trackAllocations)
	{
		g_callstackCritSection.lock();
		{
			--g_NumberOfAllocations;
			g_TotalAllocated -= numberOfBytes;
			g_BytesFreedPerSec += numberOfBytes;
			if (MEMORY_TRACKER == 1)
			{
				auto foundCallstack = g_callstackMap.find(ptr);
				if (foundCallstack != g_callstackMap.end())
				{

					free(foundCallstack->second);
					g_callstackMap.erase(foundCallstack);
				}
			}
			
		}
		g_callstackCritSection.unlock();
	}
	free(ptr_size);
	//Can only free on the start part of a malloc

}


void MemoryInit()
{
	DebuggerPrintf("Number of Allocs at Start: %i", g_NumberOfAllocations);
	g_StartAllocated = g_NumberOfAllocations;
	CallstackSystemInit();
	g_trackAllocations = true;
	g_timestampMemory = g_engineTotalTime;
	g_timestampMemoryVerbose = g_engineTotalTime;
}

void MemoryShutdown()
{
	DebuggerPrintf("\n//---------------------------------------------------------------------------------------------\n\n");

	DebuggerPrintf("Number of Allocs at End: %i\n", g_NumberOfAllocations);

	g_trackAllocations = false;
	ASSERT_RECOVERABLE((size_t)g_NumberOfAllocations == g_StartAllocated, Stringf(" %u More Allocs at end than at start", g_NumberOfAllocations - g_StartAllocated));
	if (MEMORY_TRACKER == 1)
	{
		MemoryFlush();
	}

// 	for (auto callstackIter = g_callstackMap.begin(); callstackIter != g_callstackMap.end(); ++callstackIter)
// 	{
// 		CallStackLine* line = CallstackGetLines(callstackIter->second);
// 
// 		DebuggerPrintf(Stringf("%s\n%s\n(%u)",  line->filename, line->function_name, line->line).c_str());
// 	}

	
	CallstackSystemDeinit();
}

void MemoryFlush()
{
	int currentAllocation = 0;
	g_callstackCritSection.lock();
	{
		for (auto callstackItem : g_callstackMap)
		{
			void* ptr = callstackItem.first;
			size_t* ptr_size = (size_t*)ptr;
			--ptr_size;
			size_t numberOfBytes = *ptr_size;

			CallStack * currentCallstack = callstackItem.second;
			CallStackLine * lines = CallstackGetLines(currentCallstack);


			DebuggerPrintf("\n//---------------------------------------------------------------------------------------------\n\nLeak # %i Size: %u Bytes\n//---------------------------------------------------------------------------------------------\n\n"
				, currentAllocation, numberOfBytes);
			for (unsigned int index = 0; index < currentCallstack->frame_count; ++index)
			{
				DebuggerPrintf(Stringf("%s\n%s(%u)\n", lines[index].function_name, lines[index].filename, lines[index].line).c_str());
			}
			DebuggerPrintf("\nEnd Leak # %i\n//---------------------------------------------------------------------------------------------\n\n", currentAllocation);
			currentAllocation++;
		}
	}
	g_callstackCritSection.unlock();
}

uint32_t HashMemory(void const *memory, size_t const memory_size)

{
	unsigned char const *bytes = (unsigned char const*)memory;
	uint32_t hash = 0;

	// do some hash function
	hash = crc32b(bytes, memory_size);


	return hash;

}

void UpdateCallstackMemoryHashMap()
{
	g_callstackHashMap.clear();
	//Iterate thru the callstack map
	g_callstackCritSection.lock();
	{


		for (auto callstackItem : g_callstackMap)
		{
			void* ptr = callstackItem.first;
			size_t* ptr_size = (size_t*)ptr;
			--ptr_size;
			size_t numberOfBytes = *ptr_size;

			CallStack* cs = callstackItem.second;
			//Hash the callstack
			uint32_t hash = HashMemory(cs->frames, (cs->frame_count * sizeof(void*)));
			auto foundCallstackStats = g_callstackHashMap.find(hash);
			if (foundCallstackStats != g_callstackHashMap.end())
			{
				foundCallstackStats->second.alloc_count++;
				foundCallstackStats->second.total_bytes += numberOfBytes;
			}
			else
			{
				CallStackLine* csLine = CallstackGetLine(cs);
				CallStackStats stats;
				stats.fileline = csLine->line;
				stats.filename = (unsigned char*)csLine->filename;
				stats.alloc_count = 1;
				stats.total_bytes = numberOfBytes;

				g_callstackHashMap.insert(std::pair<uint32_t, CallStackStats>(hash, stats));
			}
		}
	}
	g_callstackCritSection.unlock();
	
}

CONSOLE_COMMAND(memory_flush)
{
	args;
	MemoryFlush();
}

//std::map<void*, CallStack*, std::less<void*>, TUntrackedAllocator<std::pair<void*, CallStack*>>> g_callstackMap;
#else
void MemoryInit() { CallstackSystemInit(); }
void MemoryShutdown() { CallstackSystemDeinit(); }
void MemoryFlush() {}
uint32_t HashMemory(void const *memory, size_t const memory_size) { return 0; }
#endif)