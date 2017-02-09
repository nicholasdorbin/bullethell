#pragma once

#include <vector>
#include <map>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Memory/CallStack.hpp"
#include "Engine/Core/Memory/CriticalSection.hpp"
#include "Engine/Core/Memory/UntrackerAllocator.hpp"


extern int g_NumberOfAllocations;
extern unsigned int g_StartAllocated;
extern unsigned int g_TotalAllocated;
extern unsigned int g_MaxAllocated;
extern unsigned int g_BytesAllocedPerSec;
extern unsigned int g_BytesFreedPerSec;
extern unsigned int g_BytesAllocedLastTick;
extern unsigned int g_BytesFreedLastTick;
extern bool g_trackAllocations;
extern float g_timestampMemory;
extern float g_timestampMemoryVerbose;





// STL BULLSHIT
static std::vector<int, TUntrackedAllocator<int>> gTestVector;



void MemoryInit();
void MemoryShutdown();
void MemoryFlush();
uint32_t HashMemory(void const *memory, size_t const memory_size);
void UpdateCallstackMemoryHashMap();
extern std::map<void*, CallStack*, std::less<void*>, TUntrackedAllocator<std::pair<void*, CallStack*>>> g_callstackMap;
extern std::map<uint32_t, CallStackStats, std::less<uint32_t>, TUntrackedAllocator<std::pair<uint32_t, CallStackStats>>> g_callstackHashMap;
extern CriticalSection g_callstackCritSection;