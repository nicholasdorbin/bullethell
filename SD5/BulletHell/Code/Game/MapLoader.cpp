#include "Game/MapLoader.hpp"

#include <vector>

#define STBI_HEADER_FILE_ONLY
#include "ThirdParty/stb/stb_image.c"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Rgba.hpp"
#include "Engine/MathUtils.hpp"


#define STATIC // Do-nothing indicator that method/member is static in class definition


//---------------------------------------------------------------------------
STATIC std::map< std::string, MapLoader* >	MapLoader::s_textureRegistry;

MapLoader::MapLoader(const std::string& imageFilePath)
{
	//Vector3 lightVectorToDot(lightVector.x, lightVector.y, lightVector.z);
	//Vector3 lightVectorToDot(lightVector.x*-1.f, lightVector.y*-1.f, lightVector.z*-1);

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
		for (int j = inputTexelSize.y - 1; j >= 0; j--)
		{
			for (int i = 0; i <= inputTexelSize.x - 1; i++)
			{
				int index = (j * inputTexelSize.x) + i;
				Rgba color = arrayOfColors.at(index);
				m_colorData.push_back(color);
			}
		}
	}
}

MapLoader::MapLoader()
{

}
