#include "Game/Map.hpp"

#define STBI_HEADER_FILE_ONLY
#include "ThirdParty/stb/stb_image.c"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Rgba.hpp"
#include "Engine/MathUtils.hpp"

#include "Engine/Renderer/SpriteRenderer/SpriteGameRenderer.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"
#include "Game/Entity.hpp"
#include "Game/EnemyShip.hpp"
#include "Game/Meteor.hpp"
#include "Game/Star.hpp"

Map::Map()
{

}

Map::Map(const std::string& imageFilePath)
{
	IntVector2 inputTexelSize;
	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
	int numComponentsRequested = 3; // don't care; we support 3 (RGB) or 4 (RGBA)
	unsigned char* inputTexture = stbi_load(imageFilePath.c_str(), &inputTexelSize.x, &inputTexelSize.y, &numComponents, numComponentsRequested);

	if (inputTexture == nullptr)
	{
		m_isValidFile = false;
	}
	else
	{
		m_isValidFile = true;
		m_texelSize.SetXY(inputTexelSize.x, inputTexelSize.y);
		int numOfTexels = inputTexelSize.x * inputTexelSize.y;
		//int numOfInputComponents = numOfTexels * numComponents;

		std::vector<Rgba> arrayOfColors;


		for (int texelIndex = 0; texelIndex < numOfTexels; texelIndex++)
		{
			int rIndex = texelIndex*numComponents;
			float r = inputTexture[rIndex];
			float g = inputTexture[rIndex + 1];
			float b = inputTexture[rIndex + 2];
			float a = 255.f; // Hard coding alpha to be 1
			Rgba texel(r, g, b, a);
			arrayOfColors.push_back(texel);
		}

		//Now store the colors so that the bottom left pixel is first in the list
		
		float leftBounds = g_spriteRenderSystem->GetVirtualBounds().m_mins.x;
		float rightBounds = g_spriteRenderSystem->GetVirtualBounds().m_maxs.x;

		float bottomBounds = g_spriteRenderSystem->GetVirtualBounds().m_mins.y;
		float topBounds = g_spriteRenderSystem->GetVirtualBounds().m_maxs.y;

		for (int j = inputTexelSize.y - 1; j >= 0; j--)
		{
			MapRow newRow;
			for (int i = 0; i <= inputTexelSize.x - 1; i++)
			{
				int index = (j * inputTexelSize.x) + i;
				Rgba color = arrayOfColors.at(index);


				int x = i;
				x = RangeMap(i, 0, inputTexelSize.x, leftBounds, rightBounds);

				int y = j;
				y = RangeMap(j, 0, inputTexelSize.y, bottomBounds, topBounds);

				Vector2 pos = Vector2(x, topBounds + 0.01f);
				Entity* ent = GetEntForColor(color, pos);
				if (ent != nullptr)
				{
					ent->m_sprite->SetEnabled(false);
					newRow.m_rowEntities.push_back(ent);
				}
			}
			m_rowData.push_back(newRow);
		}
	}
}

Entity* Map::GetEntForColor(Rgba color, Vector2 pos)
{
	if (color == Rgba::BLACK)
	{
		//Empty tile, return nullptr
		return nullptr;
	}


	if (color == Rgba::RED)
	{
		//Enemy Ship
		EnemyShip* newShip = new EnemyShip(pos);
		return newShip;
	}

	if (color == Rgba::BLUE)
	{
		//Meteor
		Meteor* newMeteor = new Meteor(pos);
		return newMeteor;
	}
	if (color == Rgba::GREEN)
	{
		m_mapType = MAPTYPE_STRIKER;
	}

	if (color == Rgba::YELLOW)
	{
		m_mapType = MAPTYPE_SPEED;
	}

	if (color == Rgba::PINK)
	{
		m_mapType = MAPTYPE_SLAYER;
	}


	return nullptr;
}
