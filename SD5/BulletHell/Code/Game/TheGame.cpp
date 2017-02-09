#include "Game/TheGame.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheApp.hpp"
//#include "Game/GameCommon.hpp"
#include "Engine/Math/AABB3.hpp"

#include <time.h>
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Renderer/Shaders/GPUProgram.hpp"
#include "Engine/Renderer/Shaders/Mesh.hpp"
#include "Engine/Renderer/Shaders/MeshBuilder.hpp"
#include "Engine/Renderer/Shaders/Material.hpp"
#include "Engine/Core/Camera3D.hpp"
#include "Engine/Renderer/Shaders/Light.hpp"
#include "Engine/Tools/fbx.hpp"
#include "Engine/Renderer/Skeleton.hpp"
#include "Engine/Animation/Motion.hpp"
#include "Engine/Core/EngineBuildConfig.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"
#include "Engine/Renderer/ParticleRenderer/ParticleSystem.hpp"





#ifdef MEMORY_TRACKER
#include "Engine/Core/Memory/MemoryAnalytics.hpp"
#endif

TheGame* g_theGame = nullptr;


const int MAX_MEM_VERBOSE_LINES = 5;
const float XHAIR_LENGTH = 15.f;
const std::string FONT_NAME = "Data/Fonts/ArialFont.fnt";
//const IntVector2 snapBackPos = IntVector2(800, 450);

const std::string TEST_FILE = "Data/Images/Test_StbiAndDirectX.png";

const std::string READY_MUSIC = "Data/Sounds/Music/06 - 3DRealms - Aliens Of Gold (Blake Stone Theme).wav";
const std::string PLAYING_MUSIC = "Data/Sounds/Music/03 - 3DRealms - Major Stryker Medley.wav";
const std::string BOSS_MUSIC = "Data/Sounds/Music/07 - 3DRealms - Alien Carnage (Halloween Harry).wav";


CONSOLE_COMMAND(fbx_list)
{
    if (args.m_argList.size() < 1) //Because I'm lazy
    {
        std::string filename = "Data/Models/SampleBox.fbx";
        FbxListScene(filename);
    }
    else
    {
        std::string filename = args.m_argList[0];
        FbxListScene(filename);
    }
        //return;
    

}

CONSOLE_COMMAND(fbx_load)
{
    args;
    /*
    if (args.m_argList.size() < 1) //Because I'm lazy
    {
        g_theGame->ClearBuilderPointers(g_theGame->m_modelMeshBuilder, g_theGame->m_modelRenderer, g_theGame->m_modelMesh);
    
        
        std::string filename = "Data/Models/samplebox.fbx";
        std::vector<MeshBuilder*> importMeshes;
        SceneImport* thisScene = FbxLoadSceneFromFile(filename, g_engineBasis, true, mat44_fl::identity);
        importMeshes = thisScene->meshes;
        g_theGame->m_modelScale = 1.f;
        //#TODO Make this NOT new off a MeshBuilder. Or at least deconstruct it.
        std::vector< Skeleton* > importSkeletons;
        std::vector<Motion*> importMotions;
        importSkeletons = thisScene->skeletons;
        importMotions = thisScene->motions;
        if (importSkeletons.size() > 0)
        {
            g_theGame->m_skeleton = importSkeletons[0];
            g_theGame->PopulateJointMeshRenderer(importSkeletons[0]);
        }
        if (importMotions.size() > 0)
        {
            g_theGame->m_motions = importMotions;
        }
        g_theGame->m_modelMeshBuilder = new MeshBuilder(importMeshes);
    }
    else if (args.m_argList.size() == 1)
    {
        g_theGame->ClearBuilderPointers(g_theGame->m_modelMeshBuilder, g_theGame->m_modelRenderer, g_theGame->m_modelMesh);

        std::string filename = "Data/Models/" + args.m_argList[0];
        std::vector<MeshBuilder*> importMeshes;
        SceneImport* thisScene = FbxLoadSceneFromFile(filename, g_engineBasis, true, mat44_fl::identity);
        importMeshes = thisScene->meshes;
        g_theGame->m_modelScale = 1.f;
        //#TODO Make this NOT new off a MeshBuilder. Or at least deconstruct it.
        std::vector< Skeleton* > importSkeletons;
        std::vector<Motion*> importMotions;
        importSkeletons = thisScene->skeletons;
        importMotions = thisScene->motions;
        g_theGame->m_modelMeshBuilder = new MeshBuilder(importMeshes);
        if (importSkeletons.size() == 1)
        {
            g_theGame->m_skeleton = importSkeletons[0];
            g_theGame->PopulateJointMeshRenderer(importSkeletons[0]);
        }
        if (importMotions.size() > 0)
        {
            g_theGame->m_motions = importMotions;
        }
        
    }
    else if (args.m_argList.size() == 2)
    {
        g_theGame->ClearBuilderPointers(g_theGame->m_modelMeshBuilder, g_theGame->m_modelRenderer, g_theGame->m_modelMesh);

        std::string filename = "Data/Models/" + args.m_argList[0];
        g_theGame->m_modelScale = stof(args.m_argList[1]);
        mat44_fl transform = mat44_fl::identity;
        MatrixMakeScale(&transform, g_theGame->m_modelScale);
        std::vector<MeshBuilder*> importMeshes;
        SceneImport* thisScene = FbxLoadSceneFromFile(filename, g_engineBasis, true, transform);
        importMeshes = thisScene->meshes;
        std::vector<Skeleton*> importSkeletons = thisScene->skeletons;
        std::vector<Motion*> importMotions;
        importMotions = thisScene->motions;

        if (importSkeletons.size() == 1)
        {
            g_theGame->m_skeleton = importSkeletons[0];
            g_theGame->PopulateJointMeshRenderer(importSkeletons[0]);
        }

        if (importMotions.size() > 0)
        {
            g_theGame->m_motions = importMotions;
        }
        g_theGame->m_modelMeshBuilder = new MeshBuilder(importMeshes);
    }
    //return;

    */
}

CONSOLE_COMMAND(mesh_save)
{
    if (g_theGame->m_modelMeshBuilder == nullptr)
    {
        return;
    }

    if (args.m_argList.size() < 1) //Because I'm lazy
    {
        std::string filename = "Data/Models/basicbox.mesh";
        g_theGame->m_modelMeshBuilder->WriteToFile(filename);
    }
    else
    {
        std::string filename = "Data/Models/" + args.m_argList[0];
        g_theGame->m_modelMeshBuilder->WriteToFile(filename);
    }
    //return;
}

CONSOLE_COMMAND(mesh_load)
{
  
    if (args.m_argList.size() < 1) //Because I'm lazy
    {
        g_theGame->ClearBuilderPointers(g_theGame->m_modelMeshBuilder, g_theGame->m_modelRenderer, g_theGame->m_modelMesh);
        g_theGame->m_modelMeshBuilder = new MeshBuilder();
        std::string filename = "Data/Models/basicbox.mesh";
        g_theGame->m_modelMeshBuilder->LoadFromFile(filename);
    }
    else
    {
        g_theGame->ClearBuilderPointers(g_theGame->m_modelMeshBuilder, g_theGame->m_modelRenderer, g_theGame->m_modelMesh);
        g_theGame->m_modelMeshBuilder = new MeshBuilder();
        std::string filename = "Data/Models/" + args.m_argList[0];
        g_theGame->m_modelMeshBuilder->LoadFromFile(filename);
    }
    //return;
}

CONSOLE_COMMAND(skel_save)
{
    if (g_theGame->m_skeleton == nullptr)
    {
        return;
    }

    if (args.m_argList.size() < 1) //Because I'm lazy
    {
        std::string filename = "Data/Models/basicbox.skel";
        g_theGame->m_skeleton->WriteToFile(filename);
    }
    else
    {
        std::string filename = "Data/Models/" + args.m_argList[0];
        g_theGame->m_skeleton->WriteToFile(filename);
    }
    //return;
}

CONSOLE_COMMAND(skel_load)
{

    if (args.m_argList.size() < 1) //Because I'm lazy
    {
        delete g_theGame->m_skeleton;
        g_theGame->m_skeleton = nullptr;
        g_theGame->m_skeleton = new Skeleton();
        std::string filename = "Data/Models/basicbox.skel";
        g_theGame->m_skeleton->LoadFromFile(filename);
        g_theGame->PopulateJointMeshRenderer(g_theGame->m_skeleton);
    }
    else
    {
        if (g_theGame->m_skeleton)
        {
            delete g_theGame->m_skeleton;
            g_theGame->m_skeleton = nullptr;
        }
        
        g_theGame->m_skeleton = new Skeleton();
        std::string filename = "Data/Models/" + args.m_argList[0];
        g_theGame->m_skeleton->LoadFromFile(filename);
        g_theGame->PopulateJointMeshRenderer(g_theGame->m_skeleton);
    }
    //return;
}

CONSOLE_COMMAND(motion_save)
{
    if (g_theGame->m_motions.size() == 0 || g_theGame->m_motions.size() <= (unsigned int) g_theGame->m_motionIndex)
    {
        return;
    }

    if (args.m_argList.size() < 1) //Because I'm lazy
    {
        std::string filename = "Data/Models/basicmotion.anim";
        g_theGame->m_motions[g_theGame->m_motionIndex]->WriteToFile(filename);
    }
    else
    {
        std::string filename = "Data/Models/" + args.m_argList[0];
        g_theGame->m_motions[g_theGame->m_motionIndex]->WriteToFile(filename);
    }

}

CONSOLE_COMMAND(motion_load)
{
    if (args.m_argList.size() < 1) //Because I'm lazy
    {
        Motion* newMotion = new Motion();
        std::string filename = "Data/Models/basicmotion.anim";
        newMotion->LoadFromFile(filename);
        g_theGame->m_motions.push_back(newMotion);
        g_theGame->m_motionIndex = g_theGame->m_motions.size() - 1;
    }
    else
    {

        Motion* newMotion = new Motion();
        std::string filename = "Data/Models/" + args.m_argList[0];
        newMotion->LoadFromFile(filename);
        g_theGame->m_motions.push_back(newMotion);
        g_theGame->m_motionIndex = g_theGame->m_motions.size() - 1;
    }
}


CONSOLE_COMMAND(toggle_skeleton)
{
    args;
    g_theGame->m_showSkeleton = !g_theGame->m_showSkeleton;
}

CONSOLE_COMMAND(toggle_model)
{
    args;
    g_theGame->m_showModelMesh = !g_theGame->m_showModelMesh;
}

CONSOLE_COMMAND(set_motion_index)
{
    if(args.m_argList.size() < 2)
        g_theGame->m_motionIndex = stoi(args.m_argList[0]);
}

CONSOLE_COMMAND(quat_test)
{
    g_theGame->m_enableCameraSystem = !g_theGame->m_enableCameraSystem;
}


#ifdef MEMORY_TRACKER
CONSOLE_COMMAND(memory_debug)
{
    args;
    g_DisplayMemoryDebug = !g_DisplayMemoryDebug;
}
#endif

TheGame::TheGame()
    : m_lightNum(2)
    , m_maxLights(16)
    , m_modelRenderState(0)
    , m_showSkeleton(true)
    , m_showModelMesh(true)
    , m_motionTime(0.f)
    , m_motionIndex(0)
    , m_playAnimation(true)
    , m_modelScale(1.0f)
    , m_quatSlerpTime(0.f)
    , m_negateSlerpTime(false)
    , m_enableCameraSystem(false)
	, m_state(STATE_ATTRACT_SCREEN)
{
	g_spriteRenderSystem = new SpriteGameRenderer();
	m_audio = new AudioSystem();
	m_playingMusic = m_audio->CreateOrGetSound(PLAYING_MUSIC);
	m_readyMusic = m_audio->CreateOrGetSound(READY_MUSIC);
	m_bossMusic = m_audio->CreateOrGetSound(BOSS_MUSIC);


    m_startQuat = Quaternion(0.f, Vector3::FORWARD);
    m_endQuat = Quaternion(90.f, Vector3::UP);

#ifdef MEMORY_TRACKER
    m_memoryListLastIndexToShow = MAX_MEM_VERBOSE_LINES - 1;
#endif // MEMORY_TRACKER

    //InitializeCommon();
	g_camera = Camera3D();
	g_camera.m_position = Vector3::ZERO;
	g_spriteRenderSystem->LoadAllSpriteResources();
	g_spriteRenderSystem->LoadAllAnimationSequences();
	g_ParticleEngine->LoadAllParticleSystemDefinitions();

	m_particleTest = g_ParticleEngine->Create("smoke", LAYER_ATTRACT);
	m_particleTest->Initialize();




    m_modelMeshBuilder = nullptr;
    m_modelRenderer = nullptr;
    m_modelMesh = nullptr;
    m_skeleton = nullptr;
    
    m_world = new World();

    Console::RegisterCommand("draw", DrawDebugArrow );

	m_audio->PlaySound(m_readyMusic, 0.2, true);



	

	Job* job = g_theJobSystem->JobCreate(GENERIC, TestJobFunction);

	job->JobWrite<float>(1.f);
	job->JobWrite<int>(42);


	float testFloat = job->JobRead<float>();
	int testInt = job->JobRead<int>();

}

void TheGame::SetUpPerspectiveProjection() const
{
    float aspect = (16.f / 9.f);
    //float fovDegreesHorizontal = 90.f;
    float fovDegreesVertical = 70.f;
    float zNear = 0.1f;
    float zFar = 100.f;

    g_theRenderer->SetPerspective(fovDegreesVertical, aspect, zNear, zFar);
}

void TheGame::Update(const float deltaSeconds)
{
	g_theProfiler->StartProfilerSample("GameUpdate");
    g_engineTotalTime += deltaSeconds;
    if (g_theApp->m_inputSystem.IsKeyJustPressed(0xc0) && g_theApp->m_inputSystem.IsKeyDown(0xc0)) //why tho ~
    {
        g_theConsole->m_isOpen = !g_theConsole->m_isOpen;
    }
    if (!g_theConsole->m_isOpen)
    {

		m_particleTest->Update(deltaSeconds);

		Rgba testColor = Rgba("0.5,0.5,1,1");
       // UpdateCameraSystem(deltaSeconds);
		if (m_state.GetState() == STATE_ATTRACT_SCREEN)
		{
			g_spriteRenderSystem->DisableAllButThisLayer(eGame_SpriteLayers::LAYER_ATTRACT);
			g_spriteRenderSystem->EnableLayer(eGame_SpriteLayers::LAYER_BACKGROUND);

			if (g_theApp->m_inputSystem.IsKeyJustPressed(VK_RETURN) || g_theApp->m_controllers[0].IsButtonPressedOnce(XBOX_START))
			{
				m_state.SetState(STATE_READY);
				m_world->m_worldTimer = 0.f;
				//g_spriteRenderSystem->EnableAllButThisLayer(eGame_SpriteLayers::LAYER_ATTRACT);
			}
		}

		if (m_state.GetState() == STATE_READY)
		{
			if (m_world->m_worldTimer > 1.5f)
			{
				m_state.SetState(STATE_PLAYING);
				m_world->m_worldTimer = 0.f;
				m_world->EnableWaveSpriteForCurrentMap();
				g_spriteRenderSystem->EnableAllButThisLayer(eGame_SpriteLayers::LAYER_ATTRACT);
				g_theGame->m_audio->StopSound(m_readyMusic);
				g_theGame->m_audio->PlaySound(m_bossMusic, 0.2f);
				
			}
		}

    
		if (g_theApp->m_controllers[0].IsButtonDown(XBOX_BACK))
		{
			g_isQuitting = true;
		}

    }
    else
    {
        g_theConsole->Update(deltaSeconds);
		
    }
    
	m_world->Update(deltaSeconds);
	m_audio->Update(deltaSeconds);
    static float m_age;
    m_age += deltaSeconds;

    
    UpdateMemoryDebug(deltaSeconds);


    g_theApp->m_inputSystem.UpdateKeyJustPressedState();
    g_theConsole->m_inputSystem.UpdateKeyJustPressedState();
    g_theRenderer->UpdateRenderCommands(deltaSeconds);
	g_theProfiler->EndProfilerSample();
}

void TheGame::UpdateMemoryDebug(const float deltaSeconds)
{
    #ifdef MEMORY_TRACKER
    if (!g_theConsole->m_isOpen)
    {
        if (g_theApp->m_inputSystem.IsKeyDown(VK_NEXT) && g_theApp->m_inputSystem.IsKeyJustPressed(VK_NEXT)) // PGDOWN
        {
            if ((unsigned int)m_memoryListLastIndexToShow < m_verboseDataStrings.size())
                m_memoryListLastIndexToShow++;
        }
        else if (g_theApp->m_inputSystem.IsKeyDown(VK_PRIOR) && g_theApp->m_inputSystem.IsKeyJustPressed(VK_PRIOR)) // PGUP
        {
            if (m_memoryListLastIndexToShow >= MAX_MEM_VERBOSE_LINES)
                m_memoryListLastIndexToShow--;
        }
    }
    

    if (g_engineTotalTime - g_timestampMemory > 1)
    {
        g_timestampMemory = g_engineTotalTime;
        g_BytesAllocedLastTick = g_BytesAllocedPerSec;
        g_BytesFreedLastTick = g_BytesFreedPerSec;

        g_BytesAllocedPerSec = 0;
        g_BytesFreedPerSec = 0;
    }

    if (MEMORY_TRACKER == 1)
    {


        UpdateCallstackMemoryHashMap();

        m_verboseDataStrings.clear();

        //Iterate thru the callstacks
        for (auto callstackItem : g_callstackHashMap)
        {
            //Show the file line, Alloc Count, Total Size of the alloc
            unsigned char* fileName = callstackItem.second.filename;
            uint32_t fileLine =  callstackItem.second.fileline;

            unsigned int numAllocs = callstackItem.second.alloc_count;
            unsigned int totalSize = callstackItem.second.total_bytes;

            std::string thisDebugStr = Stringf("File: %s, Line: #%u ___ Amount: %u, Size: %u ", fileName, fileLine, numAllocs, totalSize);

            m_verboseDataStrings.push_back(thisDebugStr);
        }

        if (m_memoryListLastIndexToShow > m_verboseDataStrings.size() - 1)
            m_memoryListLastIndexToShow = m_verboseDataStrings.size() - 1;
    }
    #endif
}

void TheGame::UpdateCameraSystem(const float deltaSeconds)
{
    if (m_negateSlerpTime)
    {
        m_quatSlerpTime -= deltaSeconds;
    }
    else
    {
        m_quatSlerpTime += deltaSeconds;
    }
    

    if (m_quatSlerpTime > 1.f) {
        m_negateSlerpTime = true;
        //m_dt *= -1.f;
        m_quatSlerpTime = 1.f - (m_quatSlerpTime - 1.f);
    }
    else if (m_quatSlerpTime < 0.f) {
        m_negateSlerpTime = false;
        //m_dt *= -1.f;
        m_quatSlerpTime = -m_quatSlerpTime;
    }

    Quaternion intermidiateQuat = SlerpQuats(m_startQuat, m_endQuat, m_quatSlerpTime);

    if (m_enableCameraSystem)
    {
        intermidiateQuat.ToEulerAngles(g_camera.m_orientation);
        g_camera.FixAndClampAngles();
    }
}

void TheGame::Render(const float deltaSeconds) const
{



	//g_theRenderer->DrawAABB(m_world->m_player.m_bounds, Rgba::RED);


    g_spriteRenderSystem->Render();
// 	static BitmapFontMeta* bitmapFont = BitmapFontMeta::CreateOrGetFont(FONT_NAME);
// 	Rgba playerHealthColor = Rgba::WHITE;
// 
// 	std::string clockStr = "Current: " + Stringf("%f", (float)g_systemClock->m_currentTime);
// 	std::string clockStr2 = "Delta: " + Stringf("%f", (float)g_systemClock->GetDelta());
// 	g_theRenderer->DrawTextMeta2D(Vector3(20.f, 80.f, 0.f), 0.8f,
// 		clockStr,
// 		playerHealthColor, bitmapFont);
// 	g_theRenderer->DrawTextMeta2D(Vector3(20.f, 50.f, 0.f), 0.8f,
// 		clockStr2,
// 		playerHealthColor, bitmapFont);
	if (m_state.GetState() == STATE_PLAYING)
	{
		DrawHUD();
	}
	//g_theRenderer->ApplyRenderState(RenderState::BASIC_2D);



	
    g_theRenderer->SetDepthTest(false);
    g_theRenderer->SetOrtho(Vector2(0.f, 1600.f), Vector2(0.f, 900.f));
    DrawMemoryDebug();
    g_theConsole->Render();

	
	//g_theRenderer->DrawTexturedAABB(m_world->m_player.m_bounds , 0, Vector2(0.f, 0.f), Vector2(1.f, 1.f), Rgba::RED);
    


    //g_theGame->DrawCrosshair();
}

void TheGame::DrawHUD() const
{
	static BitmapFontMeta* bitmapFont = BitmapFontMeta::CreateOrGetFont(FONT_NAME);
	Rgba playerHealthColor = Rgba::GREEN;
	float percentHealth = m_world->m_player.GetPercentHealthRemaining();
	if (percentHealth <= 50.f)
	{
		playerHealthColor = Rgba::YELLOW;
	}
	if (percentHealth <= 25.f)
	{
		playerHealthColor = Rgba::RED;
	}
	std::string playerHealthStr = "Shields: " + Stringf("%.0f", percentHealth) + "%";
	g_theRenderer->DrawTextMeta2D(Vector3(20.f, 50.f, 0.f), 0.8f,
		playerHealthStr,
		playerHealthColor, bitmapFont);
}

void TheGame::DebugText(std::string text)
{
    //g_theRenderer->DrawText2D(Vector2(20.f, 800.f), text, 15.f, Rgba::WHITE, m_font);
}

void TheGame::DrawCrosshair()
{
    Rgba color = Rgba::WHITE;
    Vector2 xhairBottom = Vector2((float)snapBackPos.x, (float)snapBackPos.y - XHAIR_LENGTH);
    Vector2 xhairTop = Vector2((float)snapBackPos.x, (float)snapBackPos.y + XHAIR_LENGTH);
    Vector2 xhairLeft = Vector2((float)snapBackPos.x - XHAIR_LENGTH, (float)snapBackPos.y);
    Vector2 xhairRight = Vector2((float)snapBackPos.x + XHAIR_LENGTH, (float)snapBackPos.y);

    g_theRenderer->SetInverseDestBlend();
    g_theRenderer->DrawLine(xhairBottom, xhairTop, color.m_red, color.m_green, color.m_blue, color.m_alpha, 2.f);
    g_theRenderer->DrawLine(xhairLeft, xhairRight, color.m_red, color.m_green, color.m_blue, color.m_alpha, 2.f);
    g_theRenderer->SetAlphaBlend();
}

void TheGame::DrawMemoryDebug() const
{
	static BitmapFontMeta* bitmapFont = BitmapFontMeta::CreateOrGetFont(FONT_NAME);
#ifdef MEMORY_TRACKER



    if (g_DisplayMemoryDebug)
    {
        
        std::string currentAllocationText = "Current Allocs: " + std::to_string(g_NumberOfAllocations);
        std::string totalAllocationText = " Total Bytes: " + std::to_string(g_TotalAllocated);
        std::string maxAllocationText = " Max Bytes: " + std::to_string(g_MaxAllocated);
        std::string allocationLastTickText = " Bytes Alloc Last Tick: " + std::to_string(g_BytesAllocedLastTick);
        std::string freedLastTickText = " Bytes Freed Last Tick: " + std::to_string(g_BytesFreedLastTick);
        int memDiff = g_BytesAllocedLastTick - g_BytesFreedLastTick;
        std::string avgMemChangeText = " Mem Change Last Tick: " + std::to_string(memDiff);



        g_theRenderer->DrawTextMeta2D(Vector3(20.f, g_theApp->WINDOW_PHYSICAL_HEIGHT - 20.f, 0.f), 0.3f,
            currentAllocationText + totalAllocationText + maxAllocationText + allocationLastTickText + freedLastTickText + avgMemChangeText,
            Rgba::WHITE, bitmapFont);

        if (MEMORY_TRACKER == 1)
        {
            std::vector<std::string> linesToShow;
            linesToShow.resize(5);
            for each (std::string str in linesToShow)
            {
                str = "";
            }
            

            int indexToStartAt = 0;
            
            int indexToEndAt = m_verboseDataStrings.size() - 1;
            if (m_memoryListLastIndexToShow >= MAX_MEM_VERBOSE_LINES)
            {
                indexToEndAt = m_memoryListLastIndexToShow;
                indexToStartAt = indexToEndAt - (MAX_MEM_VERBOSE_LINES - 1);
            }



            float offsetY = g_theApp->WINDOW_PHYSICAL_HEIGHT - 60.f;
            std::string amountOfLinesStr = Stringf("Showing CallStack Lines %i - %i out of %i", indexToStartAt + 1, indexToStartAt + MAX_MEM_VERBOSE_LINES, g_callstackHashMap.size());
            g_theRenderer->DrawTextMeta2D(Vector3(20.f, offsetY + 20.f, 0.f), 0.3f,
                amountOfLinesStr,
                Rgba::WHITE, bitmapFont);
            for (int i = 0; i < MAX_MEM_VERBOSE_LINES; i++)
            {
                if ((indexToStartAt + i) >= m_verboseDataStrings.size())
                    break;

                linesToShow[i] = m_verboseDataStrings[indexToStartAt + i];
                g_theRenderer->DrawTextMeta2D(Vector3(20.f, offsetY - (i * 20.f), 0.f), 0.3f,
                    linesToShow[i],
                    Rgba::WHITE, bitmapFont);
            }

            
        }
    }
#endif // MEMORY_TRACKER


#ifdef PROFILER
	if (g_theProfiler->IsEnabled())
	{
		double frameTime = g_theProfiler->GetTimeOfPrevFrame();
		int framesPerSec = (int)floor(1 / frameTime);
		std::string frameTimeStr = Stringf("Frame Time: %f", frameTime);
		std::string fpsStr = Stringf("FPS: %i", framesPerSec);
		g_theRenderer->DrawTextMeta2D(Vector3(20.f, 80.f, 0.f), 0.5f,
			frameTimeStr,
			Rgba::WHITE, bitmapFont);
		g_theRenderer->DrawTextMeta2D(Vector3(20.f, 120.f, 0.f), 0.5f,
			fpsStr,
			Rgba::WHITE, bitmapFont);


		float offsetY = g_theApp->WINDOW_PHYSICAL_HEIGHT - 60.f;
		if (g_theProfiler->m_showLiveUpdate)
		{
			std::vector<std::string> sampleLines = g_theProfiler->GetLiveFrameStrings();
			for (int i = 0; i < sampleLines.size()-1; i++)
			{
	

				g_theRenderer->DrawTextMeta2D(Vector3(20.f, offsetY - (i * 20.f), 0.f), 0.3f,
					sampleLines[i],
					Rgba::WHITE, bitmapFont);
			}
		}
		
	}
#endif
}

void TheGame::ClearBuilderPointers(MeshBuilder* &meshBuilder, MeshRenderer* &modelRender, Mesh* &mesh)
{
    delete meshBuilder;
    meshBuilder = nullptr;

    delete modelRender;
    modelRender = nullptr;

    delete mesh;
    mesh = nullptr;
}

void TheGame::PopulateJointMeshRenderer(Skeleton* skeleton)
{
    if (m_jointMeshRenderer.size() > 0)
    {
        m_jointMeshRenderer.clear();
    }

    for (unsigned int jointIndex = 0; jointIndex < skeleton->m_jointNames.size(); ++jointIndex)
    {
        //#TODO Change to new matrix
// 		m_jointMeshRenderer.push_back(new MeshRenderer(m_sphereMesh, m_materialStone,
// 			Transform(skeleton->GetJointPosition(jointIndex), mat44_fl::identity,
// 				Vector3(0.1f, 0.1f, 0.1f))));
    }
}


void DrawDebugArrow(Command& args)
{
    
    if (args.m_argList.empty())
    {
        g_theRenderer->AddDebugArrow(Vector3(1.f, 1.f, 1.f), Vector3(10.f, 50.f, 10.f), Rgba::YELLOW, 2.f, RenderCommand::XRAY);
    }
}

void TestJobFunction(Job* job)
{

}
