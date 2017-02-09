#pragma once


#pragma once

#include <vector>
#include <string>
#include <map>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Matrix4.hpp"
//#include "Engine/Renderer/Shaders/MeshRenderer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Rgba.hpp"


//-------------------------------------------------------------------------------------------------
class SpriteGameRenderer;
class SpriteResource;
class SpriteLayer;
class Sprite;
class AnimationSequence;
class Framebuffer;
class Mesh;
class MeshBuilder;
class Material;
class MeshRenderer;
class Framebuffer;
class ParticleSystem;
struct ParticleSystemDefinition;
class ParticleEmitter;
class ParticleEmitterDefinition;

//-------------------------------------------------------------------------------------------------
extern SpriteGameRenderer * g_spriteRenderSystem;


//-------------------------------------------------------------------------------------------------
class SpriteGameRenderer
{
private:
	Vector2 m_virtualScreen;
	Vector2 m_camera = Vector2::ZERO;
	AABB2 m_virtualBounds;
	float m_virtualSize;
	float m_importSize;
	float m_aspectRatio;
	Rgba m_clearColor;
	Matrix4 m_projectionMatrix;
	Mesh * m_spriteMesh;
	MeshBuilder* m_meshBuilder;
	MeshRenderer * m_spriteMeshRenderer;

	std::map<std::string, SpriteResource*> m_spriteResourceDatabase;
	std::map<std::string, AnimationSequence*> m_animationSequenceDatabase;
	std::map<int, SpriteLayer*> m_spriteLayers;

// 	Framebuffer * m_fboCurrent;
// 	Framebuffer * m_fboEffect;
	Mesh const * m_screenMesh;
	Material * m_screenMaterial;
	


	
public:
	Sprite* m_testSprite;
	Sprite* m_testSprite2;
	Sprite* m_testSprite3;

	MeshRenderer * m_screenEffect;
	Framebuffer * m_fboCurrent;
	Framebuffer * m_fboEffect;

	Material * m_defaultScreenMaterial;

	SpriteGameRenderer();
	~SpriteGameRenderer();

	void Render();
	void RenderLayer(SpriteLayer const * layer);
	void RenderSprite(Sprite const * sprite);

	void LoadAllSpriteResources(); //Loads Sprites from an XML
	void LoadAllAnimationSequences(); //Loads Sprites from an XML
	void LoadAllEmitterDefinitions();
	void LoadAllParticleSystems();
	void AddSpriteResource(std::string const & id, std::string const & filename);
	SpriteLayer * CreateOrGetLayer(int layer);
	void RemoveSpriteFromLayer(Sprite* sprite);
	void AddEffectToLayer(Material* effect, int layer);
	void RemoveEffectFromLayer(int layer);
	void ConstructMesh(Mesh * out_updateMesh, Sprite const * sprite);

	void EnableLayer(int layer);
	void DisableLayer(int layer);
	void EnableAllButThisLayer(int layer);
	void DisableAllButThisLayer(int layer);
	void EnableAllLayers();
	void DisableAllLayers();

	SpriteResource const * GetSpriteResource(std::string const & spriteID) const;
	AnimationSequence* GetAnimationSequence(std::string const & animID) const;
	float GetVirtualSize() const;
	float GetImportSize() const;
	float GetAspectRatio() const;

	void ResetVirtualScreen();
	void SetVirtualSize(float size);
	void SetImportSize(float size);
	void SetAspectRatio(float width, float height);

	AABB2 GetVirtualBounds();
	Vector2 GetActualCoordsFromVirtual(const Vector2 inVec);
	void SetCameraPosition(const Vector2& newCameraPos);
};

