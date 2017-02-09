#include "Game/TheApp.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Renderer/Shaders/Mesh.hpp"
#include "Engine/Renderer/Shaders/Framebuffer.hpp"
#include "Engine/Renderer/Shaders/GPUProgram.hpp"
#include "Engine/Renderer/Shaders/Material.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shaders/MeshRenderer.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteGameRenderer.hpp"


TheApp* g_theApp = nullptr;



TheApp::TheApp(int screenWidth, int screenHeight, void* windowHandle)
	: m_canCloseGame(false)
	, m_inputSystem(windowHandle)
	, WINDOW_PHYSICAL_WIDTH(screenWidth)
	, WINDOW_PHYSICAL_HEIGHT(screenHeight)
{

	//Create FBO

	std::vector<eTextureFormat> textureFormats; //#TODO: Why exactly is this an array again?
	textureFormats.push_back(eTextureFormat_RGBA8);

	//Create Screen Mesh
	m_screenMesh =  Mesh::GetMeshShape(MeshShape_QUAD_XY);

	//Create Nothing Shader PostProcessFX
	m_fbo = new Framebuffer(WINDOW_PHYSICAL_WIDTH, WINDOW_PHYSICAL_HEIGHT, textureFormats, eTextureFormat_D24S8);
	//m_screenMaterial = new Material("Data/Shaders/post.vert", "Data/Shaders/post_red.frag");
	m_screenMaterial = new Material("Data/Shaders/post.vert", "Data/Shaders/post_passthrough.frag");
	m_screenEffect = new MeshRenderer(m_screenMesh, m_screenMaterial, Transform(Vector3::ZERO, Matrix4::IDENTITY, 2.f));
	m_screenEffect->SetRenderState(RenderState::BASIC_2D);
	m_screenEffect->SetUniform("gDiffuseTex", m_fbo->GetColorTexture(0));
	m_screenEffect->SetUniform("gDepthTex", m_fbo->GetDepthTexture());


	m_controllers[0].SetControllerNumber(0);
	m_controllers[1].SetControllerNumber(1);
	m_controllers[2].SetControllerNumber(2);
	m_controllers[3].SetControllerNumber(3);
	for (int i = 0; i < 4; i++)
	{
		m_controllers[i].Update();
	}
}

void TheApp::Update()
{
	m_inputSystem.Update();
	if (g_theConsole->m_canExit)
	{
		m_canCloseGame = true;
		g_isQuitting = true;
	}
	for (int i = 0; i < 4; i++)
	{
		m_controllers[i].Update();
	}
	//GET_WHEEL_DELTA_WPARAM(wParam);
}

void TheApp::Render(const float deltaSeconds) const
{
 	//g_theRenderer->ClearScreenDepth(0.5f, 0.85f, 0.9f);
 	//g_theRenderer->FramebufferBind(m_fbo);
 	//g_theRenderer->ClearScreen(Rgba(50, 50, 50, 255));
	//g_theRenderer->ClearScreenDepth(0.5f, 0.85f, 0.9f);

	//Draw Game
	g_theGame->Render(deltaSeconds);
	

//  	g_theRenderer->FramebufferBind(nullptr);
//  	g_theRenderer->MeshRenderPPFX(g_spriteRenderSystem->m_screenEffect);
}