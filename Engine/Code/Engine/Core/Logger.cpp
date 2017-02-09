#include "Engine/Core/Logger.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineBuildConfig.hpp"
#include "Engine/Core/Memory/CallStack.hpp"
#include <stdarg.h>
#include <iostream>

Logger* g_theLogger = nullptr;

/*
LogPrintf(char const *tag, char const *format, …)
{
	If(IsFiltered(tag)) { return; }

	Std::string finalString = GetString(format, vargs);

	ConstructMessage(tag, finalString, options);
	SendMessage(msg);//This is the only bit in this func that is on a Critical Section
}
*/

void LogPrintf(const char* messageFormat, ...)
{
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	vsnprintf_s(messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);
	messageLiteral[MESSAGE_MAX_LENGTH - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)


	LogMessage message;
	message.m_message = messageLiteral;
	message.m_logLevel = LOG_DEFAULT;

	CallStack* cs = CallstackFetch(2);
	CallStackLine* line = CallstackGetLine(cs);

	message.m_fileLine = line->line;
	message.m_fileName = line->filename;

	message.tag = LOG_DEFAULT_TAG;

	g_theLogger->m_messageBuffer.PushBack(message);
	//std::cout << messageLiteral;
}

void LogPrintf(int logLevel, const char* messageFormat, ...)
{
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	vsnprintf_s(messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);
	messageLiteral[MESSAGE_MAX_LENGTH - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)


	LogMessage message;
	message.m_message = messageLiteral;
	message.m_logLevel = logLevel;

	CallStack* cs = CallstackFetch(2);
	CallStackLine* line = CallstackGetLine(cs);

	message.m_fileLine = line->line;
	message.m_fileName = line->filename;

	message.tag = LOG_DEFAULT_TAG;

	g_theLogger->m_messageBuffer.PushBack(message);
}

void LogPrintf(int logLevel, const char* tag, const char* messageFormat, ...)
{
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	vsnprintf_s(messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);
	messageLiteral[MESSAGE_MAX_LENGTH - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)


	LogMessage message;
	message.m_message = messageLiteral;
	message.m_logLevel = logLevel;

	CallStack* cs = CallstackFetch(2);
	CallStackLine* line = CallstackGetLine(cs);

	message.m_fileLine = line->line;
	message.m_fileName = line->filename;

	message.tag = tag;

	g_theLogger->m_messageBuffer.PushBack(message);
}

void LogPrintf(const char* tag, const char* messageFormat, ...)
{
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, messageFormat);
	vsnprintf_s(messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList);
	va_end(variableArgumentList);
	messageLiteral[MESSAGE_MAX_LENGTH - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)


	LogMessage message;
	message.m_message = messageLiteral;
	message.m_logLevel = LOG_DEFAULT;

	CallStack* cs = CallstackFetch(2);
	CallStackLine* line = CallstackGetLine(cs);

	message.m_fileLine = line->line;
	message.m_fileName = line->filename;

	message.tag = tag;

	g_theLogger->m_messageBuffer.PushBack(message);
}

void LogEnable(std::string tag)
{
	if (g_theLogger->m_tagMode == LOG_WHITELIST)
	{
		//Add to list
		g_theLogger->m_listTags.push_back(tag);
	}
	else
	{
		//Remove from the list
		for (auto tagIter = g_theLogger->m_listTags.begin(); tagIter != g_theLogger->m_listTags.end(); ++tagIter)
		{
			if ((*tagIter) == tag)
			{
				g_theLogger->m_listTags.erase(tagIter);
				break;
			}
		}
	}
}

void LogDisable(std::string tag)
{
	if (g_theLogger->m_tagMode == LOG_BLACKLIST)
	{
		//Add to list
		g_theLogger->m_listTags.push_back(tag);
	}
	else
	{
		//Remove from the list
		for (auto tagIter = g_theLogger->m_listTags.begin(); tagIter != g_theLogger->m_listTags.end(); ++tagIter)
		{
			if ((*tagIter) == tag)
			{
				g_theLogger->m_listTags.erase(tagIter);
				break;
			}
		}
	}
}

void LogDisableAll()
{
	g_theLogger->m_listTags.clear();
	g_theLogger->m_tagMode = LOG_WHITELIST;
}

void LoggerThreadRun()
{
	if (!g_theLogger->OpenFile())
		return;

	while (!g_isQuitting)
	{
		LogMessage message;
		while (g_theLogger->m_messageBuffer.PopFront(&message))
		{
			g_theLogger->HandleMessage(message);
		}
		g_theLogger->m_thread.yield();
	}

	g_theLogger->FlushRemainingMessages();
}

Logger::Logger()
{
	
}

Logger::~Logger()
{
	m_thread.Join();
}

void Logger::InitializeLogger()
{
	m_file = nullptr;
	m_thread = Thread(&LoggerThreadRun);
	m_tagMode = LOG_BLACKLIST;
}

void Logger::HandleMessage(LogMessage message)
{
	if (message.m_logLevel > LOG_LEVEL)
		return;

	bool showTaggedMessage = false;
	if (m_tagMode == LOG_WHITELIST)
	{
		for (unsigned int i = 0; i < m_listTags.size(); i++)
		{
			if (m_listTags[i] == message.tag)
			{
				showTaggedMessage = true;
				break;
			}
		}
	}
	else
	{
		bool foundBlackListTag = false;
		for (unsigned int i = 0; i < m_listTags.size(); i++)
		{
			if (m_listTags[i] == message.tag)
			{
				foundBlackListTag = true;
				break;
			}
		}
		showTaggedMessage = !foundBlackListTag;
	}

	if (!showTaggedMessage)
		return;


	std::string outString;
	outString = Stringf("LOG [%i] ", message.m_logLevel);

	outString += "[" + message.tag + "] ";

	outString += "\"" + message.m_message + "\"";

	outString += Stringf("\n\t@ Line[%i], %s", message.m_fileLine, message.m_fileName);

	outString += "\n";


	fwrite(&outString.c_str()[0], sizeof(unsigned char), outString.size(), m_file);
	
	
}

void Logger::FlushRemainingMessages()
{
	LogMessage msg;
	while (g_theLogger->m_messageBuffer.PopFront(&msg))
	{
		g_theLogger->HandleMessage(msg);
	}
	fflush(m_file);
	fclose(m_file);
}

bool Logger::OpenFile()
{
	
	//FILE* file = nullptr;
	errno_t didFileOpen = fopen_s(&m_file, LOG_FILE_PATH, "wb");
	if (didFileOpen != 0)
		return false;
	return true;
}
