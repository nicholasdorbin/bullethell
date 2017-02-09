#include "Engine/Renderer/BitmapFontMeta.hpp"
#include "Engine/Core/FileUtils.hpp"
#include <map>
#include <string>
#include <sstream>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vector3.hpp"


BitmapFontMeta* BitmapFontMeta::CreateOrGetFont(const std::string& BitmapFontMetaName)
{
	size_t nameHash = std::hash<std::string>{}(BitmapFontMetaName);
	std::map< size_t, BitmapFontMeta* >::iterator iter = BitmapFontMeta::s_fontRegistry.find(nameHash);
	if (iter != s_fontRegistry.end())
	{
		return iter->second;
	}
	else
	{
		BitmapFontMeta* newFont = new BitmapFontMeta(BitmapFontMetaName);
		s_fontRegistry.insert(std::pair < size_t, BitmapFontMeta* >(nameHash, newFont));
		return newFont;

		//TODO return nullptr if FNF
	}
}

// AABB2 BitmapFontMeta::GetTexCoordsForGlyph(int glyphUnicode) const
// {
// 	return m_spriteSheet->GetTexCoordsForSpriteIndex(glyphUnicode);
// }

Texture* BitmapFontMeta::GetTexture() const
{
	return m_fontSheet;

}

BitmapFontMeta::BitmapFontMeta(const std::string& BitmapFontMetaName)
{
	ReadMetaFile(BitmapFontMetaName);
	m_name = BitmapFontMetaName;
	m_name.erase(m_name.size() - 4);
	m_name += "_0.png";
	m_fontSheet = Texture::CreateOrGetTexture(m_name);
	
	//m_spriteSheet = new SpriteSheet(fontName, 16, 16);
}

BitmapFontMeta::~BitmapFontMeta()
{
	std::map< int, Glyph* >::iterator it = m_glyphRegistry.begin();
	while (it != m_glyphRegistry.end()) {

		delete(it->second);
		it = m_glyphRegistry.erase(it);
	}
}

bool BitmapFontMeta::ReadMetaFile(const std::string& metaFileName)
{
	std::string fontFileData;
	bool fileFound = LoadBufferFromBinaryFile(fontFileData, metaFileName);
	if (fileFound)
	{
		std::stringstream metaFileStream(fontFileData);
		std::vector<std::string> metaFileLines;

		std::string line;
		while (std::getline(metaFileStream, line))
		{
			metaFileLines.push_back(line);
		}

		//Get width and height of the texture
		std::string scaleLine = metaFileLines[1];
		std::string val = ExtractToken(scaleLine, "scaleW=", " ");
		m_textureWidth = std::stoi(val);
		val = ExtractToken(scaleLine, "scaleH=", " ");
		m_textureHeight = std::stoi(val);

		std::string charCountLine = metaFileLines[3];
		val = ExtractToken(charCountLine, "chars count=", "\r");
		int charCount = std::stoi(val);

		

		for (int i = 4; i <= charCount + 3; i++)
		{
			std::string glyphLine = metaFileLines[i];
			Glyph* thisGlyph = new Glyph();

			val = ExtractToken(glyphLine, "char id=", " ");
			thisGlyph->m_charId = std::stoi(val);

			val = ExtractToken(glyphLine, "x=", " ");
			thisGlyph->m_xPos = std::stoi(val);

			val = ExtractToken(glyphLine, "y=", " ");
			thisGlyph->m_yPos = std::stoi(val);

			val = ExtractToken(glyphLine, "width=", " ");
			thisGlyph->m_width = std::stoi(val);

			val = ExtractToken(glyphLine, "height=", " ");
			thisGlyph->m_height = std::stoi(val);

			val = ExtractToken(glyphLine, "xoffset=", " ");
			thisGlyph->m_xOffset = std::stoi(val);

			val = ExtractToken(glyphLine, "yoffset=", " ");
			thisGlyph->m_yOffset = std::stoi(val);

			val = ExtractToken(glyphLine, "xadvance=", " ");
			thisGlyph->m_xAdvance = std::stoi(val);

			m_glyphRegistry.insert(std::pair < int, Glyph* >(thisGlyph->m_charId, thisGlyph));


		}

		int kerningLine = charCount + 3 + 1;

		if (metaFileLines.size() < (unsigned int)(kerningLine + 1))
		{
			// This font file does not have kerning information
			m_hasKerning = false;
		}

		else
		{
			m_hasKerning = true;

			std::string kerningCountLine = metaFileLines[kerningLine];
			val = ExtractToken(kerningCountLine, "kernings count=", "\r");
			int kerningCount = std::stoi(val);

			//Add kerning pairs to the kerning registry
			for (int i = kerningLine + 1; i <= kerningLine + kerningCount; i++)
			{
				std::string kerningLineText = metaFileLines[i];

				val = ExtractToken(kerningLineText, "kerning first=", " ");
				int firstKerning = std::stoi(val);

				val = ExtractToken(kerningLineText, "second=", " ");
				int secondKerning = std::stoi(val);

				val = ExtractToken(kerningLineText, "amount=", " ");
				int value = std::stoi(val);

				std::pair<int, int> kerningPair = std::pair<int, int>(firstKerning, secondKerning);

				m_kerningRegistry.insert(std::pair < std::pair<int, int>, int >(kerningPair, value));
			}
		}
		
	}
	return fileFound;
}

Glyph* BitmapFontMeta::GetGlyph(const char c)
{
	int val = (int)c;
	return m_glyphRegistry.at(val);
}

const bool BitmapFontMeta::GetKerning(const int first, const int second, int& out_val) const
{
	std::pair<int, int> kerningPair = std::pair<int, int>(first, second);
	std::map< std::pair<int, int>, int >::const_iterator it;
	it = m_kerningRegistry.find(kerningPair);
	if (it != m_kerningRegistry.end())
	{
		out_val = it->second;
		return true;
	}
	out_val = 0;
	return false;
}

float BitmapFontMeta::CalcTextWidth(const std::string& text, const float scale)
{
	Vector3 cursor = Vector3::ZERO;
	Vector3 right = Vector3(1.f, 0.f, 0.f);
	Vector3 up = Vector3(0.f, 1.f, 0.f);

	Glyph* prev_glyph = nullptr;

	for (std::string::const_iterator it = text.begin(); it != text.end(); ++it)
	{
		Glyph* thisGlyph = m_glyphRegistry.at(*it);

		if (thisGlyph == nullptr)
			continue;
		int kerningVal = 0;
		if (prev_glyph != nullptr)
		{
			GetKerning(prev_glyph->m_charId, thisGlyph->m_charId, kerningVal);
		}


		cursor += kerningVal * scale * right;

		cursor += thisGlyph->m_xAdvance * scale * right;
		prev_glyph = thisGlyph;

	}
	return cursor.x;
}

float BitmapFontMeta::CalcTextHeight(const std::string& inputText, float scale)
{


	float maxHeightPx = -1.f;

	Vector2 cursor = Vector2::ZERO; //Assuming its the lower-left.

	for (unsigned int charIndex = 0; charIndex < inputText.size(); charIndex++)
	{
		char c = inputText[charIndex];
		const Glyph* currentGlyph = GetGlyph(c); //Use m_fontGlyphs.

		if (currentGlyph == nullptr)
			continue; //Skips unsupported chars.

		float quadHeightPx = (currentGlyph->m_yOffset + currentGlyph->m_height) * scale;
		if (maxHeightPx < quadHeightPx)
			maxHeightPx = quadHeightPx;
	}

	return maxHeightPx;
}




std::map< size_t, BitmapFontMeta* > BitmapFontMeta::s_fontRegistry;

void BitmapFontMeta::DestroyFonts()
{
	std::map< size_t, BitmapFontMeta* >::iterator it = s_fontRegistry.begin();
	while (it != s_fontRegistry.end()) {
		if (it->second->m_textureHeight > 0)
			delete(it->second);
		it = s_fontRegistry.erase(it);
	}
}