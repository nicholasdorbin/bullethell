#pragma once

#include <map>
#include <vector>
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Rgba.hpp"

class Entity;

enum e_MapType 
{
	MAPTYPE_SPEED = 0,
	MAPTYPE_SLAYER,
	MAPTYPE_STRIKER
};

struct MapRow
{
	std::vector<Entity*> m_rowEntities;
	int m_rowIndex;
};
class Map
{
public:
	Map();
	Map(const std::string& imageFilePath);

	int openglTextureID;
	IntVector2 m_texelSize;
	bool m_isValidFile;
	e_MapType m_mapType;

	Entity* GetEntForColor(Rgba color, Vector2 pos);

	static std::map< std::string, Map* >	Map::s_textureRegistry;

	std::vector<Rgba> m_colorData;
	std::vector<MapRow> m_rowData;
};


