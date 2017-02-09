#include "Engine/Core/Events/NamedEventSystem.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

NamedEventSystem* g_theEventSystem = nullptr;

CONSOLE_COMMAND(fire_event)
{
	//Unused
	args;
	if (g_theEventSystem == nullptr)
		return;
	if (args.m_argList.size() == 0)
		return;

	FireEvent(args.m_argList[0], true);
}

void FireEventForEachFileFound(const std::string& eventToFire, const std::string& baseFolder, const std::string& filePattern, bool recurseSubfolders /*= false*/)
{
	{
		std::vector<std::string> foundFiles = EnumerateFilesInFolder(baseFolder, filePattern, recurseSubfolders);


		for each (std::string fileString in foundFiles)
		{
			/*
			“FileName”				“ExplosionSprites.png”
			“FileExtension”			“.png”
			“FileNameWithoutExtension”		“ExplosionSprites”
			“FileRelativePath”			“Images/ExplosionSprites.png”
			“FileAbsolutePath”			“C:/Guildhall/Projects/Tanks/Data/Images/ExplosionSprites.png”

			*/
			std::vector<std::string> slashTokens = TokenizeStringOnDelimiter(fileString, "/");
			std::string fileName = slashTokens.back();

			std::vector<std::string> dotTokens = TokenizeStringOnDelimiter(fileName, ".");
			std::string fileExtension = "." + dotTokens.back();

			std::string fileNameNoExtension = "";
			for (size_t i = 0; i < dotTokens.size() - 1; i++)
			{
				if (i > 0)
				{
					fileNameNoExtension += ".";
				}
				fileNameNoExtension += dotTokens[i];
			}

			std::string relativePath = fileString;

			char buffer[_MAX_PATH];
			_fullpath(buffer, relativePath.c_str(), _MAX_PATH);

			

			std::string fullPath = Stringf("%s", buffer);
			ReplaceCharsInString(fullPath, '\\', '/');


			NamedProperties params;
			params.Set("FileName", fileName);
			params.Set("FileExtension", fileExtension);
			params.Set("FileNameWithoutExtension", fileNameNoExtension);
			params.Set("FileRelativePath", relativePath);
			params.Set("FileAbsolutePath", fullPath);
			
			FireEvent(eventToFire, params);
		}
	}
}

NamedEventSystem::~NamedEventSystem()
{
	auto iter = m_registeredObjs.begin();

	while (iter != m_registeredObjs.end())
	{
		for (size_t i = 0; i < iter->second.size(); i++)
		{
			delete iter->second[i];
		}
		iter++;
	}
}
