#pragma once
#include "Engine/Core/Memory/Thread.hpp"
#include "Engine/Core/Memory/ThreadSafeQueue.hpp"

#include <string>
#include <fstream>
class Logger;
extern Logger* g_theLogger;
const char LOG_FILE_PATH[] = "Data/debug.log";
const char LOG_DEFAULT_TAG[] = "General";


enum eLog_TagMode
{
	LOG_WHITELIST = 0,
	LOG_BLACKLIST
};

enum eLog_Level
{
	LOG_NONE = 0, //Show no logs
	LOG_SEVERE = 1, 
	LOG_RECOVERABLE = 2, //Significant Warnings
	LOG_DEFAULT = 3, //Production Quality warnings
	LOG_ALL = 4 // Informational Warnings
};

struct LogMessage
{
	std::string m_message;
	int m_logLevel;
	std::string tag;
	int m_fileLine;
	char const* m_fileName;
};

class Logger
{
public:
	Logger();
	~Logger();

	void InitializeLogger();
	void HandleMessage(LogMessage message);
	void FlushRemainingMessages();
	bool OpenFile();

	ThreadSafe_Queue<LogMessage> m_messageBuffer;
	Thread m_thread;

	eLog_TagMode m_tagMode;
	std::vector<std::string> m_listTags;
private:
	
	FILE* m_file;

};

void LogPrintf(const char* messageFormat, ...);
void LogPrintf(const char* tag, const char* messageFormat, ...);
void LogPrintf(int logLevel, const char* messageFormat, ...);
void LogPrintf(int logLevel, const char* tag, const char* messageFormat, ...);

void LogEnable(std::string tag);
void LogDisable(std::string tag);
void LogDisableAll();

void LoggerThreadRun();