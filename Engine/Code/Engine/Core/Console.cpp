#include "Engine/Core/Console.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <sstream>

Console* g_theConsole = nullptr;

const std::string FONT_NAME = "Data/Fonts/ArialFont.fnt";
const int NUM_CONSOLE_LINES = 20;
const float TEXT_SCALE = 0.36f;
const float CURSOR_BLINK_RATE = 0.5f;
const float CURSOR_PADDING = 0.2f;

static std::map< std::string, console_command_cb > *g_commandRegistry = nullptr;

RegisterCommandHelper::RegisterCommandHelper(std::string name, console_command_cb cb)
{
	Console::RegisterCommand(name, cb);
}
Console::Console()
{
	m_screenHeight = 1;
	m_screenWidth = 1;
	m_isOpen = false;
	m_canExit = false;
	m_curCommand = "";
	m_lastCommand = "";
	m_bitmapFont = BitmapFontMeta::CreateOrGetFont(FONT_NAME);
	m_showCursor = true;
	m_timeSinceLastCursorBlink = 0.f;
	m_cursorIndex = 0;
	m_linesToScrollUp = 0;
}

Console::Console(int screenHeight, int screenWidth)
{
	m_screenHeight = screenHeight;
	m_screenWidth = screenWidth;
	m_isOpen = false;
	m_canExit = false;
	m_curCommand = "";
	m_lastCommand = "";
	m_bitmapFont = BitmapFontMeta::CreateOrGetFont(FONT_NAME);
	m_showCursor = true;
	m_timeSinceLastCursorBlink = 0.f;
	m_cursorIndex = 0;
	m_linesToScrollUp = 0;
}

Console::~Console()
{
	//delete m_bitmapFont;
}

void Console::Update(const float deltaSeconds)
{
	m_timeSinceLastCursorBlink += deltaSeconds;

	if (m_timeSinceLastCursorBlink > CURSOR_BLINK_RATE)
	{
		m_showCursor = !m_showCursor;
		m_timeSinceLastCursorBlink = 0.f;
	}

	//Handle input system non-char stuff
	if (m_inputSystem.IsKeyDown(VK_LEFT) && m_inputSystem.IsKeyJustPressed(VK_LEFT))
	{
		if (m_cursorIndex > 0)
			m_cursorIndex--;
	}
	
	else if (m_inputSystem.IsKeyDown(VK_RIGHT) && m_inputSystem.IsKeyJustPressed(VK_RIGHT))
	{
		if ((unsigned int)m_cursorIndex < m_curCommand.length())
			m_cursorIndex++;
	}
	else if (m_inputSystem.IsKeyDown(VK_HOME) && m_inputSystem.IsKeyJustPressed(VK_HOME))
	{
		m_cursorIndex = 0;
	}
	else if (m_inputSystem.IsKeyDown(VK_END) && m_inputSystem.IsKeyJustPressed(VK_END))
	{
		m_cursorIndex = m_curCommand.length();
	}
	else if (m_inputSystem.IsKeyDown(VK_DELETE) && m_inputSystem.IsKeyJustPressed(VK_DELETE))
	{
		if ((unsigned int)m_cursorIndex < m_curCommand.length())
		{
			m_curCommand.erase(m_cursorIndex, 1);
			//m_cursorIndex--;
		}
	}
	else if (m_inputSystem.IsKeyPressed(VK_UP))
	{
		if (m_lastCommand != "")
		{
			m_curCommand = m_lastCommand;
			m_cursorIndex = m_curCommand.length() - 1;
		}
	}
	else if (m_inputSystem.IsKeyDown(VK_PRIOR) && m_inputSystem.IsKeyJustPressed(VK_PRIOR)) // PGUP
	{
		if ((unsigned int)m_linesToScrollUp < m_log.size()-1)
			m_linesToScrollUp++;
	}
	else if (m_inputSystem.IsKeyDown(VK_NEXT) && m_inputSystem.IsKeyJustPressed(VK_NEXT)) // PGDOWN
	{
		if (m_linesToScrollUp > 0)
			m_linesToScrollUp--;
	}
}

void Console::HandleInput(unsigned char key)
{
	if (key == VK_BACK)
	{
		//Backspace
		if (m_curCommand.length() > 0)
		{
			m_curCommand = m_curCommand.erase(m_curCommand.size() - 1);
			m_cursorIndex--;
		}
	}
	
	else if (key == VK_RETURN)
	{
		//Enter
		if (m_curCommand != "")
		{
			ExecuteCommand(m_curCommand);
			m_lastCommand = m_curCommand;
			m_curCommand = "";
			m_cursorIndex = 0;
			m_linesToScrollUp = 0;
		}
		else
		{
			m_isOpen = false;
		}
	}
	else if (key == VK_ESCAPE)
	{
		if (m_curCommand != "")
		{
			m_curCommand = "";
			m_cursorIndex = 0;
		}
		else
		{
			m_isOpen = false;
		}
	}
	
	else if (key == 0xc0 || key == '`' || key == '~')
	{
		//Do nothing
	}
	else
	{
		std::map<int, Glyph*>::iterator it;
		it = m_bitmapFont->m_glyphRegistry.find(key);

		if (it != m_bitmapFont->m_glyphRegistry.end())
		{
			if ((unsigned int)m_cursorIndex == m_curCommand.length())
			{
				m_curCommand += (unsigned char)it->second->m_charId;
			}
			else
			{
				//Cursor is not at end of str
				std::string beforeCursorSubstr = m_curCommand.substr(0, m_cursorIndex);
				std::string afterCursorSubstr = m_curCommand.substr(m_cursorIndex, m_curCommand.length());
				m_curCommand = "";
				m_curCommand = beforeCursorSubstr; 
				m_curCommand += (unsigned char)it->second->m_charId;
				m_curCommand += afterCursorSubstr;

			}
			
			m_cursorIndex++;
		}
	}
}

void Console::Render()
{
	if (m_isOpen)
	{
		
		AABB2 overlay = AABB2(Vector2(0.f,0.f),Vector2((float)m_screenWidth, (float)m_screenHeight / 2.f));
		AABB2 prompt = overlay;
		prompt.m_maxs.y = prompt.m_maxs.y / (float)NUM_CONSOLE_LINES;

		Rgba overlayColor = Rgba(0, 0, 0, 127);
		Rgba promptColor = Rgba(190, 190, 190, 127);
		g_theRenderer->DrawTexturedAABB(overlay, 0, Vector2(0.f, 0.f), Vector2(1.f, 1.f), overlayColor);
		g_theRenderer->DrawTexturedAABB(prompt, 0, Vector2(0.f, 0.f), Vector2(1.f, 1.f), promptColor);
		g_theRenderer->SetDepthTest(false);

		
		float lineYPos = prompt.m_maxs.y;
		
		g_theRenderer->DrawTextMeta2D(Vector3(0.f, lineYPos, 0.f), TEXT_SCALE, (m_curCommand), Rgba::WHITE, m_bitmapFont);//BUG: Crashes if trying to draw empty str

		if (m_showCursor)
		{
			std::string substr = m_curCommand.substr(0, m_cursorIndex);
			float cursorPosX = m_bitmapFont->CalcTextWidth(substr, TEXT_SCALE);
			g_theRenderer->DrawLine(Vector3(cursorPosX + CURSOR_PADDING, 0.f, 0.f), Vector3(cursorPosX + CURSOR_PADDING, prompt.m_maxs.y, 0.f), Rgba::WHITE, 1.2f);
		}
		
		int index = 2;
		std::vector<ConsoleString>::reverse_iterator logIter = m_log.rbegin();

		for (int i = 0; i < m_linesToScrollUp; i++)
		{
			logIter++;
			if (logIter == m_log.rend())
			{
				break;
			}
		}
		
		for (logIter; logIter != m_log.rend(); ++logIter )
		{
			if (index > NUM_CONSOLE_LINES)
				break;
			lineYPos = prompt.m_maxs.y * index;
			std::string text = logIter->m_text;
			Rgba color = logIter->m_tint;
			g_theRenderer->DrawTextMeta2D(Vector3(0.f, lineYPos, 0.f), TEXT_SCALE, text, color, m_bitmapFont);
			index++;
		}
		
	}
}

void Console::ConsolePrint(const std::string& text, const Rgba& color)
{
	ConsoleString cs(color, text);
	m_log.push_back(cs);
}

void Console::RegisterCommand(std::string name, console_command_cb cb)
{
	if (g_commandRegistry == nullptr)
	{
		//Make the command registry
		g_commandRegistry = new std::map< std::string, console_command_cb >;
	}
	g_commandRegistry->insert(std::pair< std::string, console_command_cb>(name, cb));
}

void Console::ExecuteCommand(std::string commandStr)
{
	ConsolePrint(commandStr, Rgba::LTGRAY);
	Command command(commandStr);

	std::map< std::string, console_command_cb >::iterator it;
	it = g_commandRegistry->find(command.m_commandName);

	if (it != g_commandRegistry->end())
	{
		it->second(command);
	}
	else
	{
		ConsolePrint("Invalid Command.", Rgba::YELLOW);
	}
}

Command::Command(std::string fullCommandString)
{
	//Break apart string into args
	//Save off name
	//Save off args
	m_fullString = fullCommandString;
	std::stringstream fullCommandStringStream(fullCommandString);
	std::vector< std::string > commandContents;
	std::string lineArg;
	while (fullCommandStringStream >> lineArg)
	{
		commandContents.push_back(lineArg);
	}

	if (commandContents.size() > 0)
	{
		m_commandName = commandContents[0];
	}

	if (commandContents.size() > 1)
	{
		for (std::vector< std::string >::iterator argIter = commandContents.begin() + 1; argIter != commandContents.end(); ++argIter)
		{
			m_argList.push_back(*argIter);
		}
	}
}
CONSOLE_COMMAND(help)
{
	if (args.m_argList.empty())
	{
		g_theConsole->ConsolePrint("Commands Available:", Rgba::WHITE);
		//std::map< std::string, console_command_cb >::iterator it;
		for (std::map< std::string, console_command_cb >::iterator argIter = g_commandRegistry->begin(); argIter != g_commandRegistry->end(); ++argIter)
		{
			g_theConsole->ConsolePrint(argIter->first, Rgba::YELLOW);
		}
	}
	else
	{
		if (args.m_argList[0] == "help")
		{
			g_theConsole->ConsolePrint("help <commandName>: Prints a description of the command", Rgba::YELLOW);
		}
	}
}

CONSOLE_COMMAND(clear)
{
	args;
	g_theConsole->m_log.clear();
}


CONSOLE_COMMAND(exit)
{
	args;
	g_theConsole->m_canExit = true;
}


