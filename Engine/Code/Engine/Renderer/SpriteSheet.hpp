#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include <string>

class SpriteSheet
{
public:
	SpriteSheet(const std::string& imageFilePath, int tilesWide, int tilesHigh);
	AABB2 GetTexCoordsForSpriteCoords(int spriteX, int spriteY) const; // mostly for atlases
	AABB2 GetTexCoordsForSpriteIndex(int spriteIndex) const; // mostly for sprite animations
	int GetNumSprites() const;
	//... and other member functions as needed
	Texture* 	m_spriteSheetTexture;	// Image with grid-based layout of sub-images


	SpriteSheet& SpriteSheet::operator=(const SpriteSheet &rhs);
private:
	
	IntVector2	m_spriteLayout;		// # of sprites across, and down, on the sheet
	float spriteCutFactorX;
	float spriteCutFactorY;
	
	//... and other data member variables as needed
};
