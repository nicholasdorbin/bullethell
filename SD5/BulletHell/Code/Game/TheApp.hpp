#pragma once
#include "Engine/Core/InputSystem.hpp"
#include <vector>
class TheApp;

extern TheApp* g_theApp;
class Framebuffer;
class Mesh;
class MeshRenderer;
class Material;


class TheApp
{
public:
	TheApp(int screenWidth, int screenHeight, void* windowHandle);
	void Update();
	void Render(const float deltaSeconds) const;

	bool m_canCloseGame;
	InputSystem m_inputSystem;
	int WINDOW_PHYSICAL_WIDTH;
	int WINDOW_PHYSICAL_HEIGHT;
	

	Framebuffer *m_fbo;
	Mesh *m_screenMesh;
	Material *m_screenMaterial;
	MeshRenderer *m_screenEffect;
	XboxController m_controllers[4];
private:

	std::vector<MeshRenderer*> m_screenEffects;
	std::vector<Material*> m_screenMaterials;

};

