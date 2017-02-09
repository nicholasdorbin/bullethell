#pragma once
#include <vector>
#include <string>
#include <map>
#include "Engine/Math/Rgba.hpp"
#include "Engine/Core/InputSystem.hpp"

class Console;
class BitmapFontMeta;

extern Console* g_theConsole;

struct ConsoleString
{
	ConsoleString() {};
	ConsoleString(const Rgba& color, const std::string& text) :
		m_tint(color),
		m_text(text) {};
	Rgba m_tint;
	std::string m_text;
};

class Command
{
public:
	Command(std::string fullCommandString); // split name and args into two buffers

	std::string m_fullString;
	std::string m_commandName;
	std::vector< std::string > m_argList;
	
};

typedef void(*console_command_cb)(Command&);

class RegisterCommandHelper
{
public:
	RegisterCommandHelper(std::string name, console_command_cb cb);
};

class Console
{
public:
	Console();
	Console(int screenHeight, int screenWidth);
	~Console();
	void Update(const float deltaSeconds);
	void HandleInput(unsigned char key);
	void Render();
	void ConsolePrint(const std::string& text, const Rgba& color = Rgba::WHITE);
	static void RegisterCommand(std::string name, console_command_cb cb); // Put name and console_command_cb into a map
	void ExecuteCommand(std::string commandStr);

	bool m_showCursor;
	bool m_isOpen;
	bool m_canExit;
	int m_screenHeight;
	int m_screenWidth;
	float m_timeSinceLastCursorBlink;
	int m_cursorIndex;
	int m_linesToScrollUp;

	InputSystem m_inputSystem;

	BitmapFontMeta* m_bitmapFont;

	std::vector<ConsoleString> m_log;
	std::string m_curCommand;
	std::string m_lastCommand;
};


#define CONSOLE_COMMAND(name) void ConsoleCommand_ ## name ## ( Command & args ); \
	  static RegisterCommandHelper RegistrationHelper_## name ##( #name, ConsoleCommand_ ## name ## ); \
	  void ConsoleCommand_ ## name ##(Command &args)