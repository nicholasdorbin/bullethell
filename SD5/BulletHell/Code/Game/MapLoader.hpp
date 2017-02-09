#pragma once
#include <map>
#include <string>
#include <vector>
#include "Engine/Math/Rgba.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define STATIC // Do-nothing indicator that method/member is static in class definition

class MapLoader
{
public:
	MapLoader();
	MapLoader(const std::string& imageFilePath);

	int openglTextureID;
	IntVector2 m_texelSize;
	bool m_isValidFile;

	static std::map< std::string, MapLoader* >	MapLoader::s_textureRegistry;

	std::vector<Rgba> m_colorData;
	//static void BuildMapData();
};