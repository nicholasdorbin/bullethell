#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteResource.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteGameRenderer.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteLayer.hpp"
#include "Engine/Renderer/Texture.hpp"

Sprite::Sprite(std::string const & spriteID, int layer /*= 0*/)
	: m_color(Rgba::WHITE)
	, m_enabled(true)
	, m_layerID(layer)
	, m_rotationDegrees(0.f)
	, m_scale(1.0f, 1.0f)
	, m_pivot(Vector2(0.5f,0.5f))
{
	spriteID;//Used in the SpriteMap
}

Sprite::Sprite()
	: m_color(Rgba::WHITE)
	, m_enabled(true)
	, m_layerID(0)
	, m_rotationDegrees(0.f)
	, m_scale(1.0f, 1.0f)
	, m_pivot(Vector2(0.5f, 0.5f))
{
}

Sprite * Sprite::Create(std::string const & spriteID, int layer /*= 0*/)
{
	Sprite* newSprite = new Sprite(spriteID, layer);

	newSprite->m_spriteResource =  g_spriteRenderSystem->GetSpriteResource(spriteID);
	if (newSprite->m_spriteResource == nullptr)
		ERROR_AND_DIE("Could not load sprite: " + spriteID);
	newSprite->m_material = newSprite->m_spriteResource->GetMaterial();
	//Add it to Game Renderer's list
	g_spriteRenderSystem->CreateOrGetLayer(layer)->AddSprite(newSprite);

	return newSprite;
}

Sprite::~Sprite()
{
 //#TODO Unregister this sprite
	g_spriteRenderSystem->RemoveSpriteFromLayer(this);
}

void Sprite::SwapSprite(std::string const & spriteID)
{
	const SpriteResource* newResource = g_spriteRenderSystem->GetSpriteResource(spriteID);
	if (newResource != nullptr)
	{
		m_spriteResource = newResource;
		m_material = m_spriteResource->GetMaterial();
	}
}

Material * Sprite::GetMaterial() const
{
	//return m_spriteResource->GetMaterial();
	return m_material;
}

void Sprite::SetMaterial(Material* mat)
{
	m_material = mat;
}

Texture const * Sprite::GetTexture() const
{
	return m_spriteResource->GetTexture();
}

float Sprite::GetWidth() const
{
	float rawWidth = (float)GetTexture()->m_texelSize.x;
	float importSize = g_spriteRenderSystem->GetImportSize();
	float virtualSize = g_spriteRenderSystem->GetVirtualSize();
	return ((rawWidth / importSize) * virtualSize);
}

float Sprite::GetHeight() const
{
	float rawHeight = (float)GetTexture()->m_texelSize.y;
	float importSize = g_spriteRenderSystem->GetImportSize();
	float virtualSize = g_spriteRenderSystem->GetVirtualSize();
	return ((rawHeight / importSize) * virtualSize);
}

Vector2 Sprite::GetSpriteBottomLeft()
{
	float w = GetWidth();
	float h = GetHeight();

	//Position
	Vector2 pivot = Vector2(m_pivot.x * w, m_pivot.y * h);
	Vector2 v0 = pivot * -1.f;
	float sinRotation = SinDegrees(m_rotationDegrees);
	float cosRotation = CosDegrees(m_rotationDegrees);

	Vector2 V0Rot = v0;
	V0Rot.x = v0.x * cosRotation + v0.y * -sinRotation;
	V0Rot.y = v0.x * sinRotation + v0.y * cosRotation;

	V0Rot = (V0Rot * m_scale) + m_position;


	return V0Rot;
}

Vector2 Sprite::GetSpriteTopRight()
{
	float w = GetWidth();
	float h = GetHeight();

	//Position
	Vector2 pivot = Vector2(m_pivot.x * w, m_pivot.y * h);
	Vector2 v2 = Vector2(w - pivot.x, h - pivot.y);
	float sinRotation = SinDegrees(m_rotationDegrees);
	float cosRotation = CosDegrees(m_rotationDegrees);

	Vector2 V2Rot = v2;
	V2Rot.x = v2.x * cosRotation + v2.y * -sinRotation;
	V2Rot.y = v2.x * sinRotation + v2.y * cosRotation;


	V2Rot = (V2Rot * m_scale) + m_position;

	return V2Rot;
}

Vector2 Sprite::GetSpriteBottomLeftNoRotation()
{
	float w = GetWidth();
	float h = GetHeight();

	//Position
	Vector2 pivot = Vector2(m_pivot.x * w, m_pivot.y * h);
	Vector2 v0 = pivot * -1.f;


	Vector2 V0Rot = v0;


	V0Rot = (V0Rot * m_scale) + m_position;


	return V0Rot;
}

Vector2 Sprite::GetSpriteTopRightNoRotation()
{
	float w = GetWidth();
	float h = GetHeight();

	//Position
	Vector2 pivot = Vector2(m_pivot.x * w, m_pivot.y * h);
	Vector2 v2 = Vector2(w - pivot.x, h - pivot.y);


	Vector2 V2Rot = v2;

	V2Rot = (V2Rot * m_scale) + m_position;

	return V2Rot;
}

int Sprite::GetLayerID()
{
	return m_layerID;
}

void Sprite::ChangeSpriteLayer(int newLayer)
{
	g_spriteRenderSystem->RemoveSpriteFromLayer(this);
	g_spriteRenderSystem->CreateOrGetLayer(newLayer)->AddSprite(this);
	m_layerID = newLayer;
}

void Sprite::SetEnabled(bool isEnabled)
{
	if (m_enabled != isEnabled)
	{
		m_enabled = isEnabled;

		if (m_enabled)
		{
			g_spriteRenderSystem->CreateOrGetLayer(m_layerID)->AddSprite(this);
		}
		else
		{
			g_spriteRenderSystem->RemoveSpriteFromLayer(this);
		}
	}
}

bool Sprite::IsEnabled() const
{
	return m_enabled;
}

void Sprite::SetUseCamera(bool usesCamera)
{
	m_usesCamera = usesCamera;
}

bool Sprite::IsUsingCamera() const
{
	return m_usesCamera;
}
