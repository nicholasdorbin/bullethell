#pragma once

//------------------------------------
//Memory Tracker
// 0 = Basic. Detect if we have leaks, and at most, the number of leaks.  Should be able to track total number of allocations and the size of those allocations.
// 1 = Verbose. Should tell us a lot of information, including where our leaks are coming from, size, and optionally lifetime.
//--------------------------------
//#define MEMORY_TRACKER 0




//------------------------------------
//Log Level
// 0 = No Logs
// 1 = Severe Warnings
// 2 = Significant Warnings
// 3 = (production quality) warnings.
// 4 = (informational) warnings
//--------------------------------
#define LOG_LEVEL 4



//------------------------------------
// Profiler
//--------------------------------

#define PROFILER


//Cinematic Camera
#define CINEMATIC_CAMERA