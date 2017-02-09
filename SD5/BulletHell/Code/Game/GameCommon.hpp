#pragma once
#include <vector>
class Vector3;
class Rgba;
struct Vertex3D_PC;



#define BIT(x) (1<<(x))
const int BITS_WIDE_X = 4;
const int BITS_WIDE_Y = 4;
const int BITS_WIDE_Z = 7;







extern bool g_lockLastTrace;
extern bool g_showDirtyBlocks;
extern bool g_DebugWhiteBlocks;
extern bool g_HandleEdgeLights;
extern bool g_DrawWireFrame;
extern bool g_GravityOn;
extern bool g_enableBlockSounds;
extern bool g_DisplayMemoryDebug;


enum eGame_SpriteLayers
{
	LAYER_TOP = 200,
	LAYER_ATTRACT = 100,
	LAYER_UI = 70,
	LAYER_PLAYER = 50,
	LAYER_PLAYER_BULLET = 49,
	LAYER_ENEMY = 20,
	LAYER_ENEMY_BULLET = 10,
	LAYER_BACKGROUND = -50
};

extern std::vector<Vertex3D_PC>  g_debugPoints;

void InitializeCommon();

void AddDebugPoint(const Vector3& position, const Rgba& color); // push_back in vector
void DrawDebugPoints(float pointSize, bool enableDepthTesting); // fastest using VAs!
void ClearDebugPoints(); // clear the (global) vector
