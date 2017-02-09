#pragma once
#if !defined( __DEBUG_CALLSTACK__ )
#define __DEBUG_CALLSTACK__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "Engine/Core/EngineCommon.hpp"
#include <stdint.h>


/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* MACROS                                                               */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/
struct CallStack
{
	void** frames;
	unsigned int frame_count;
};

struct CallStackLine
{
	char filename[128];
	char function_name[128];
	uint32_t line;
	uint32_t offset;
};

struct CallStackStats
{
	unsigned int alloc_count;
	unsigned int total_bytes;
	unsigned int fileline;
	unsigned char* filename;
};

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* FUNCTION PROTOTYPES                                                  */
/*                                                                      */
/************************************************************************/

bool CallstackSystemInit();
void CallstackSystemDeinit();
CallStack* CallstackFetch(unsigned int skip_frames);


// Single Threaded - only from debug output thread (if I need the string names elsewhere
// then I need to make a "debug" job consumer)
CallStackLine* CallstackGetLines(CallStack *cs);
CallStackLine* CallstackGetLine(CallStack *cs);

#endif 
