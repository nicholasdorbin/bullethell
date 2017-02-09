#include "Engine/Renderer/SpriteSheet.hpp"

SpriteSheet::SpriteSheet(const std::string& imageFilePath, int tilesWide, int tilesHigh)
{
	m_spriteSheetTexture = Texture::CreateOrGetTexture(imageFilePath);
	m_spriteLayout.SetXY(tilesWide, tilesHigh);
	spriteCutFactorX = 1.f / m_spriteLayout.x;
	spriteCutFactorY = 1.f / m_spriteLayout.y;
}

AABB2 SpriteSheet::GetTexCoordsForSpriteCoords(int spriteX, int spriteY) const
{
	// Texture coords for the first tile of a 16*16 sprite sheet
	//	ins = 0,0, Maxs = (1f/16f, 1f/16f)
	
	//First tile is 1st row, 1st collumn. Location via spriteLayout (1,1)
	AABB2 result;
	result.m_maxs.SetXY(spriteCutFactorX * spriteX, spriteCutFactorY * spriteY);
	result.m_mins.SetXY(spriteCutFactorX * (float)(spriteX - 1), spriteCutFactorY * (float)(spriteY - 1));

	return result;
}

AABB2 SpriteSheet::GetTexCoordsForSpriteIndex(int spriteIndex) const
{
// 	int yCount = spriteIndex % m_spriteLayout.m_x;
// 	int xCount = spriteIndex - (m_spriteLayout.m_x * yCount);

	int xCount = spriteIndex % m_spriteLayout.x;
	int yCount = spriteIndex / m_spriteLayout.x;

	//First tile is 1st row, 1st collumn, index 0. Location via spriteLayout (1,1)
// 	xCount += 1;
// 	yCount += 1; //Needs to match row, not index.
	AABB2 result;
	result.m_mins.SetXY(spriteCutFactorX * xCount, spriteCutFactorY * yCount);
	result.m_maxs.SetXY(spriteCutFactorX * (float)(xCount + 1), spriteCutFactorY * (float)(yCount + 1));
	return result;
}

int SpriteSheet::GetNumSprites() const
{
	return (m_spriteLayout.x * m_spriteLayout.y);
}

SpriteSheet& SpriteSheet::operator=(const SpriteSheet &rhs)
{
	m_spriteLayout = rhs.m_spriteLayout;
	m_spriteSheetTexture = rhs.m_spriteSheetTexture;
	spriteCutFactorX = 1.f / m_spriteLayout.x;
	spriteCutFactorY = 1.f / m_spriteLayout.y;
	return *this;
}
