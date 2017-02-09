#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Camera3D.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFontMeta.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineBuildConfig.hpp"
#include "Engine/Core/Events/EventSystem.hpp"
#include "Engine/Net/NetworkSystem.hpp"
#include "Engine/Net/RemoteCommandService.hpp"
#include "Engine/Net/Session/NetSession.hpp"




Camera3D g_camera;
float g_engineDeltaSeconds;
float g_engineTotalTime;
int g_effectState;
bool g_debugDrawing;
int g_lightState;
Matrix4 g_engineBasis;
bool g_isQuitting = false;
Clock* g_systemClock;
NetworkSystem* g_networkSystem;
RemoteCommandService* g_networkService;
NetSession* g_netSession;

void InitializeEngineCommon()
{
	g_engineDeltaSeconds = 0.f;
	g_engineTotalTime = 0.f;
	g_effectState = false;
	g_debugDrawing = false;
	#ifdef CINEMATIC_CAMERA
	g_cinematicCameraController = new CinematicCameraController();
	#endif	
	
	g_camera.m_position = Vector3::ZERO;
	g_camera.m_orientation = EulerAngles::ZERO;
	g_lightState = 0;
	//MatrixGenerateBasis(&g_engineBasis, Vector3(0.f, -1.f, 0.f));
	g_engineBasis.SetBasis(Vector3::RIGHT, Vector3::UP, Vector3(0.f, -1.f, 0.f));
 	g_theLogger = new Logger();
 	g_theLogger->InitializeLogger();
	
	g_theProfiler = new Profiler();
	g_ParticleEngine = new ParticleEngine();

	//g_theJobSystem = new JobSystem();
	//g_theJobSystem->JobSystemStartUp(-2);

	EventSystem::GetInstance()->Init();

	g_networkSystem = new NetworkSystem();
	g_networkSystem->NetworkStartup();

	g_networkService = new RemoteCommandService();
	g_networkService->InitEvents();

	g_netSession = new NetSession();

	g_theEventSystem = new NamedEventSystem();
	

	TimeInit();
}

void DestroyEngineCommon()
{
 	delete g_theLogger;
 	g_theLogger = nullptr;

	#ifdef CINEMATIC_CAMERA
	delete g_cinematicCameraController;
	g_cinematicCameraController = nullptr;
	#endif
	delete g_spriteRenderSystem;
	g_spriteRenderSystem = nullptr;

	//Deleting profiler in the Main's Shutdown

	g_ParticleEngine->DestroyAllParticleSystemDefinitions();
	delete g_ParticleEngine;
	g_ParticleEngine = nullptr;

	EventSystem::GetInstance()->Shutdown();

	delete g_networkService;
	g_networkService = nullptr;

	g_networkSystem->NetworkShutdown();
	delete g_networkSystem;
	g_networkSystem = nullptr;

	g_netSession->Stop();
	delete g_netSession;
	g_netSession = nullptr;

	delete g_theEventSystem;
	
	
	Texture::DestroyTextures();
	BitmapFontMeta::DestroyFonts();


	//#TODO figure out why this errors on delete
// 	delete g_theJobSystem;
// 	g_theJobSystem = nullptr;
	delete g_systemClock;
	g_systemClock = nullptr;
}

void TimeInit()
{
	g_systemClock = new Clock(GetCurrentTimeSeconds());
}

void TimeUpdate()
{
	double currentTime = GetCurrentTimeSeconds();
	double deltaTime = currentTime - g_systemClock->m_currentTime;
	//ASSERT_RECOVERABLE(deltaTime < .5, "Ohshit");
	ASSERT_OR_DIE(g_systemClock->m_scale == 1.f, "SystemClock should not be scaled!");
	g_systemClock->Update(deltaTime);
}

eEndianMode GetSystemEndianness()
{
	return GetLocalEndianess();
}
