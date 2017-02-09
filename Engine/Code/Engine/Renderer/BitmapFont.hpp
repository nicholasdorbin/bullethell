#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include <string>

class BitmapFont
{
public:
	static BitmapFont* CreateOrGetFont(const std::string& bitmapFontName);
	AABB2 GetTexCoordsForGlyph(int glyphUnicode) const;
	Texture* BitmapFont::GetTexture() const;
	

private:
	BitmapFont(const std::string& bitmapFontName);

private:
	SpriteSheet* m_spriteSheet;
	static std::map< std::string, BitmapFont* > s_fontRegistry;
};

