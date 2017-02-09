#pragma once
#include <string>
#include <map>

#define STBI_HEADER_FILE_ONLY
#include "ThirdParty/stb/stb_image.c"

#define STATIC

struct CubeMapDirs
{
public:
	CubeMapDirs(const std::string& directory = "", const std::string& right = "", const std::string& left = "", 
		const std::string& top = "", const std::string& bottom = "", const std::string& front = "", const std::string& back = "");
	CubeMapDirs(const CubeMapDirs& other);
	~CubeMapDirs();

	const bool operator==(const CubeMapDirs& other) const;
	const bool operator<(const CubeMapDirs& other) const;
	std::string m_fileNames[6];
};

class CubeMap;
extern STATIC std::map<CubeMapDirs, CubeMap*> s_CubeMapRegistry;

class CubeMap
{
public:
	CubeMap();
	CubeMap(const CubeMapDirs& imageLocations);
	CubeMap(const std::string& directory, const std::string& right, const std::string& left,
		const std::string& top, const std::string& bottom, const std::string& front, const std::string& back);

	~CubeMap();

	const unsigned int LoadCubeMap();
	const unsigned int GetTextureObj() { return m_textureObj; };
	static CubeMap* CreateOrGetCubeMap(const CubeMapDirs& imageFilePath);

private:
	CubeMapDirs m_cubeFileNames;
	unsigned int m_textureObj;
};