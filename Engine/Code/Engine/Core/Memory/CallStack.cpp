/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#pragma warning( disable : 4091 )  // typedef ignored on left
#include "Engine/Core/Memory/CallStack.hpp"

#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_
#include <Windows.h>
#include <DbgHelp.h>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Memory/Buffer.hpp"

// #include "core/mem/allocator/system.h"
// #include "core/mem/refcounted.h"
// #include "core/math/misc.h"
// #include "core/util/buffer.h"
// #include "core/util/string.h"

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
#define MAX_SYMBOL_NAME_LENGTH 128
#define MAX_FILENAME_LENGTH 1024
#define MAX_DEPTH 128

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/

// SymInitialize()
typedef BOOL(__stdcall *sym_initialize_t)(IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess);
typedef BOOL(__stdcall *sym_cleanup_t)(IN HANDLE hProcess);
typedef BOOL(__stdcall *sym_from_addr_t)(IN HANDLE hProcess, IN DWORD64 Address, OUT PDWORD64 Displacement, OUT PSYMBOL_INFO Symbol);

typedef BOOL(__stdcall *sym_get_line_t)(IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Symbol);

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL VARIABLES                                                      */
/*                                                                      */
/************************************************************************/
static HMODULE gDebugHelp;
static HANDLE gProcess;
static SYMBOL_INFO  *gSymbol;


// only called from single thread - so can use a shared buffer
static char gFileName[MAX_FILENAME_LENGTH];
static CallStackLine gCallstackBuffer[MAX_DEPTH];

static sym_initialize_t LSymInitialize;
static sym_cleanup_t LSymCleanup;
static sym_from_addr_t LSymFromAddr;
static sym_get_line_t LSymGetLineFromAddr64;


/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL FUNCTIONS                                                      */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* EXTERNAL FUNCTIONS                                                   */
/*                                                                      */
/************************************************************************/


//------------------------------------------------------------------------
bool CallstackSystemInit()
{

	gDebugHelp = LoadLibraryA("dbghelp.dll");
	ASSERT_OR_DIE(gDebugHelp != nullptr, "Could not load dbghelp.dll");
	LSymInitialize = (sym_initialize_t)GetProcAddress(gDebugHelp, "SymInitialize");
	LSymCleanup = (sym_cleanup_t)GetProcAddress(gDebugHelp, "SymCleanup");
	LSymFromAddr = (sym_from_addr_t)GetProcAddress(gDebugHelp, "SymFromAddr");
	LSymGetLineFromAddr64 = (sym_get_line_t)GetProcAddress(gDebugHelp, "SymGetLineFromAddr64");

	gProcess = GetCurrentProcess();
	LSymInitialize(gProcess, NULL, TRUE);

	//gSymbol = (SYMBOL_INFO *)gStackPool.alloc(sizeof(SYMBOL_INFO) + (MAX_FILENAME_LENGTH * sizeof(char)));
	gSymbol = (SYMBOL_INFO *)malloc(sizeof(SYMBOL_INFO) + (MAX_FILENAME_LENGTH * sizeof(char)));
	gSymbol->MaxNameLen = MAX_FILENAME_LENGTH;
	gSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	return true;
}

//------------------------------------------------------------------------
static int gCallstackCount = 0;
void CallstackSystemDeinit()
{
	LSymCleanup(gProcess);

	FreeLibrary(gDebugHelp);
	gDebugHelp = NULL;

	if (gCallstackCount != 0) {
		gCallstackCount = 0;
	}

}

//------------------------------------------------------------------------


//------------------------------------------------------------------------
CallStack* CallstackFetch(unsigned int skip_frames)
{
	void * frameData[MAX_DEPTH];
	unsigned int frameCount = CaptureStackBackTrace(1 + skip_frames, MAX_DEPTH, frameData, NULL);

	size_t size = sizeof(CallStack) + sizeof(void*) * frameCount;
	void * buffer = malloc(size);
	CallStack * cs = (CallStack*)buffer;
	cs->frames = (void**)(cs + 1);
	cs->frame_count = frameCount;
	memcpy(cs->frames, frameData, sizeof(void*) * frameCount);

	return cs;

}

//------------------------------------------------------------------------
// Should only be called from the debug trace thread.  
CallStackLine* CallstackGetLines(CallStack *cs)
{
	IMAGEHLP_LINE64 LineInfo;
	DWORD LineDisplacement = 0; // Displacement from the beginning of the line 
	LineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

	unsigned int count = cs->frame_count;
	for (unsigned int i = 0; i < count; ++i) {
		CallStackLine *line = &(gCallstackBuffer[i]);
		DWORD64 ptr = (DWORD64)(cs->frames[i]);
		LSymFromAddr(gProcess, ptr, 0, gSymbol);

		//StringCopy(line->function_name, 128, gSymbol->Name);
		strncpy_s(line->function_name, gSymbol->Name, 127);
		

		BOOL bRet = LSymGetLineFromAddr64(
			GetCurrentProcess(), // Process handle of the current process 
			ptr, // Address 
			&LineDisplacement, // Displacement will be stored here by the function 
			&LineInfo);         // File name / line information will be stored here 

		if (bRet) {
			line->line = LineInfo.LineNumber;

			//char const *filename = StringFindLast(LineInfo.FileName, "\\src");
			char const *filename = LineInfo.FileName;
			if (filename == NULL) {
				filename = LineInfo.FileName;
			}
			else {
				//filename += 5; // skip to the important bit - so that it can be double clicked in Output
			}


			//StringCopy(line->filename, 128, filename);
			strncpy_s(line->filename, filename, 127);
			line->offset = LineDisplacement;
		}
		else {
			line->line = 0;
			line->offset = 0;
			//StringCopy(line->filename, 128, "N/A");
			strncpy_s(line->filename, "N/A", 127);
		}
	}

	return gCallstackBuffer;
}

//------------------------------------------------------------------------
static void OnFreeCallstack( void *ptr)
{
	ptr;
	//AtomicDecrement(&gCallstackCount);
}

CallStackLine* CallstackGetLine(CallStack *cs)
{
	IMAGEHLP_LINE64 LineInfo;
	DWORD LineDisplacement = 0; // Displacement from the beginning of the line 
	LineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

	//unsigned int count = cs->frame_count;
	
		CallStackLine *line = &(gCallstackBuffer[0]);
		DWORD64 ptr = (DWORD64)(cs->frames[0]);
		LSymFromAddr(gProcess, ptr, 0, gSymbol);

		//StringCopy(line->function_name, 128, gSymbol->Name);
		strncpy_s(line->function_name, gSymbol->Name, 127);


		BOOL bRet = LSymGetLineFromAddr64(
			GetCurrentProcess(), // Process handle of the current process 
			ptr, // Address 
			&LineDisplacement, // Displacement will be stored here by the function 
			&LineInfo);         // File name / line information will be stored here 

		if (bRet) {
			line->line = LineInfo.LineNumber;

			//char const *filename = StringFindLast(LineInfo.FileName, "\\src");
			char const *filename = LineInfo.FileName;
			if (filename == NULL) {
				filename = LineInfo.FileName;
			}
			else {
				//filename += 5; // skip to the important bit - so that it can be double clicked in Output
			}


			//StringCopy(line->filename, 128, filename);
			strncpy_s(line->filename, filename, 127);
			line->offset = LineDisplacement;
		}
		else {
			line->line = 0;
			line->offset = 0;
			//StringCopy(line->filename, 128, "N/A");
			strncpy_s(line->filename, "N/A", 127);
		}
	

	return gCallstackBuffer;
}
