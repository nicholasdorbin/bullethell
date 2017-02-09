#include "Engine/Renderer/BitmapFont.hpp"
#include <map>
#include <string>


BitmapFont* BitmapFont::CreateOrGetFont(const std::string& bitmapFontName)
{
	std::map< std::string, BitmapFont* >::iterator iter = BitmapFont::s_fontRegistry.find(bitmapFontName);
	if (iter != s_fontRegistry.end())
	{
		return iter->second;
	}
	else
	{
		BitmapFont* newFont = new BitmapFont(bitmapFontName);
		s_fontRegistry.insert(std::pair < std::string, BitmapFont* >(bitmapFontName, newFont));
		return newFont;

		//TODO return nullptr if FNF
	}
}

AABB2 BitmapFont::GetTexCoordsForGlyph(int glyphUnicode) const
{
	return m_spriteSheet->GetTexCoordsForSpriteIndex(glyphUnicode);
}

Texture* BitmapFont::GetTexture() const
{
	return m_spriteSheet->m_spriteSheetTexture;

}

BitmapFont::BitmapFont(const std::string& bitmapFontName)
{
	std::string fontName = "Data/Fonts/" + bitmapFontName;
	m_spriteSheet = new SpriteSheet(fontName, 16, 16);
}

std::map< std::string, BitmapFont* > BitmapFont::s_fontRegistry;




