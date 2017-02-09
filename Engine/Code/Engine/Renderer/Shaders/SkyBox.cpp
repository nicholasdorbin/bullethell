#include "Engine/Renderer/Shaders/SkyBox.hpp"
#include "Engine/Renderer/OpenGLExtensions.hpp"
#include "Engine/Math/IntVector2.hpp"


CubeMapDirs::CubeMapDirs(const std::string& directory /*= ""*/, const std::string& right /*= ""*/, const std::string& left /*= ""*/, const std::string& top /*= ""*/, 
	const std::string& bottom /*= ""*/, const std::string& front /*= ""*/, const std::string& back /*= ""*/)
{
	m_fileNames[0] = directory + right;
	m_fileNames[1] = directory + left;
	m_fileNames[2] = directory + top;
	m_fileNames[3] = directory + bottom;
	m_fileNames[4] = directory + front;
	m_fileNames[5] = directory + back;
}



CubeMapDirs::CubeMapDirs(const CubeMapDirs& other)
{
	for (size_t index = 0; index < 6; index++)
	{
		m_fileNames[index] = other.m_fileNames[index];
	}
}

CubeMapDirs::~CubeMapDirs()
{

}

const bool CubeMapDirs::operator==(const CubeMapDirs& other) const
{
	for (size_t index = 0; index < 6; index++)
	{
		if (m_fileNames[index] != other.m_fileNames[index])
			return false;
	}
	return true;
}

const bool CubeMapDirs::operator<(const CubeMapDirs& other) const
{
	for (size_t index = 0; index < 6; index++)
	{
		if (m_fileNames[index] >= other.m_fileNames[index])
			return false;
	}
	return true;
}


STATIC std::map<CubeMapDirs, CubeMap*> s_CubeMapRegistry = std::map<CubeMapDirs, CubeMap*>();


CubeMap::CubeMap()
	: m_textureObj(0),
	m_cubeFileNames("", "", "", "", "", "")
{
}

CubeMap::CubeMap(const CubeMapDirs& imageLocations)
	:m_textureObj(0),
	m_cubeFileNames(imageLocations)
{
	LoadCubeMap();
}

CubeMap::CubeMap(const std::string& directory, const std::string& right, const std::string& left, const std::string& top,
	const std::string& bottom, const std::string& front, const std::string& back)
	:m_textureObj(0),
	m_cubeFileNames(directory, right, left, top, bottom, front, back)
{
	LoadCubeMap();
}

CubeMap::~CubeMap()
{
	glDeleteTextures(1, &m_textureObj);
}

const unsigned int CubeMap::LoadCubeMap()
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	m_textureObj = textureID;
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int index = 0; index < 6; index++)
	{
		int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
		int numComponentsRequested = 0; // don't care; we support 3 (RGB) or 4 (RGBA)
		IntVector2 texelSize;
		unsigned char* imageData = stbi_load(m_cubeFileNames.m_fileNames[index].c_str(), &texelSize.x, &texelSize.y, &numComponents, numComponentsRequested);

		GLenum bufferFormat = GL_RGBA; // the format our source pixel data is currently in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
		GLenum internalFormat = GL_RGBA8; // the format we want the texture to me on the card; allows us to translate into a different texture format as we upload to OpenGL

		if (numComponents == 3)
		{
			bufferFormat = GL_RGB;
			internalFormat = GL_RGB8;
		}
		if (numComponents == 1)
			bufferFormat = GL_ALPHA;

		// #FIXME: What happens if numComponents is neither 3 nor 4?

		glTexImage2D(			// Upload this pixel data to our new OpenGL texture
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + index,		// Creating this as a 2d texture
			0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
			internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
			texelSize.x,		// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,10], and B is the border thickness [0,1]
			texelSize.y,		// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,10], and B is the border thickness [0,1]
			0,					// Border size, in texels (must be 0 or 1)
			bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
			GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color/alpha channel)
			imageData);		// Location of the actual pixel data bytes/buffer
		stbi_image_free(imageData);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return textureID;
}

CubeMap* CubeMap::CreateOrGetCubeMap(const CubeMapDirs& imageFilePath)
{
	
	std::map< CubeMapDirs, CubeMap* >::iterator iter = s_CubeMapRegistry.find(imageFilePath);
	if (iter != s_CubeMapRegistry.end())
	{
		return iter->second;
	}
	else
	{
		CubeMap* newCubeMap = new CubeMap(imageFilePath);
		s_CubeMapRegistry.insert(std::pair < CubeMapDirs, CubeMap* >(imageFilePath, newCubeMap));
		return newCubeMap;
	}
	
}

