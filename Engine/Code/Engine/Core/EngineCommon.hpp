#pragma once
#include "Engine/Core/IO/EndianUtils.hpp"
#include "Engine/Core/Camera3D.hpp"
#include "Engine/CinematicCamera/CinematicCameraController.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteGameRenderer.hpp"
#include "Engine/Time/Clock.hpp"
#include "Engine/Core/Memory/Profiler.hpp"
#include "Engine/Renderer/ParticleRenderer/ParticleEngine.hpp"
#include "Engine/Core/Memory/JobSystem.hpp"
#include "Engine/Core/Events/NamedEventSystem.hpp"




class Matrix4;
class NetworkSystem;
class RemoteCommandService;
class NetSession;


extern  Camera3D g_camera;
extern float g_engineDeltaSeconds;
extern float g_engineTotalTime;
extern int g_effectState;
extern bool g_debugDrawing;
extern bool g_isQuitting;
extern int g_lightState;
extern Matrix4 g_engineBasis;
extern Clock* g_systemClock;
extern NetworkSystem* g_networkSystem;
extern RemoteCommandService* g_networkService;
extern NetSession* g_netSession;


//-------------------------------------------------------------------------------------------------
// Engine Enumerators
//-------------------------------------------------------------------------------------------------
enum Blending
{
	Blending_NORMAL,
	Blending_SUBTRACTIVE,
	Blending_ADDITIVE,
	Blending_INVERTED,
};


//-------------------------------------------------------------------------------------------------
enum DrawMode
{
	DrawMode_FULL,
	DrawMode_LINE,
	DrawMode_POINT,
};

void InitializeEngineCommon();
void DestroyEngineCommon();
void TimeInit();
void TimeUpdate();

eEndianMode GetSystemEndianness();