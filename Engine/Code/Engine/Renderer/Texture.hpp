#pragma  once
#include <map>
#include <string>
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define STATIC // Do-nothing indicator that method/member is static in class definition
enum eTextureFormat
{
	eTextureFormat_RGBA8,
	eTextureFormat_D24S8 // Depth24 stencil 8
};

class Texture
{
public:
	Texture(const std::string& imageFilePath);
	Texture::Texture(uint32_t width, uint32_t height, eTextureFormat format);

	int m_openglTextureID;
	IntVector2 m_texelSize;

	static std::map< std::string, Texture* >	Texture::s_textureRegistry;

	static Texture* GetTextureByName(const std::string& imageFilePath);
	static Texture* CreateOrGetTexture(const std::string& imageFilePath);
	static void DestroyTextures();
};