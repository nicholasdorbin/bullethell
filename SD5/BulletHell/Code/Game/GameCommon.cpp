#include "Game/GameCommon.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Rgba.hpp"
#include "Engine/Renderer/VertexPC.hpp"
#include "Engine/Renderer/Renderer.hpp"

const std::string ATLASFILE = "Data/Images/SimpleMinerAtlas.png";

SpriteSheet* g_Atlas = nullptr;
bool g_lockLastTrace = false;
bool g_showDirtyBlocks = true;
bool g_DebugWhiteBlocks = false;
bool g_HandleEdgeLights = true;
bool g_DrawWireFrame = true;
bool g_GravityOn = true;
bool g_enableBlockSounds = true;
bool g_DisplayMemoryDebug = false;



std::vector<Vertex3D_PC>  g_debugPoints;

void InitializeCommon()
{
	g_Atlas = new SpriteSheet(ATLASFILE, 16, 16);
}

void AddDebugPoint(const Vector3& position, const Rgba& color)
{
	Vertex3D_PC newVert;
	newVert.m_position = position;
	newVert.m_color = color;
	g_debugPoints.push_back(newVert);
}

void DrawDebugPoints(float pointSize, bool enableDepthTesting)
{
	//g_theRenderer->SetPointSize(pointSize);
	g_theRenderer->SetDepthTest(enableDepthTesting);
	//g_theRenderer->DrawVertexArrayPoints(g_debugPoints);
	for (auto chunkIter = g_debugPoints.begin(); chunkIter != g_debugPoints.end(); ++chunkIter)
	{
		Vector3 other;
		other.SetXYZ(chunkIter->m_position.x + 0.1f, chunkIter->m_position.y + 0.1f, chunkIter->m_position.z + 0.1f);
		g_theRenderer->DrawLine(chunkIter->m_position, other, chunkIter->m_color, pointSize);

	}
}

void ClearDebugPoints()
{
	g_debugPoints.clear();
}
