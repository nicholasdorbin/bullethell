#pragma once
#include <string>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Rgba.hpp"


//-------------------------------------------------------------------------------------------------
class SpriteResource;
class Material;
class Texture;


//-------------------------------------------------------------------------------------------------
class Sprite
{
protected:
	bool m_enabled;
	bool m_usesCamera = true;
	int m_layerID;
	mutable SpriteResource const * m_spriteResource;
	Material* m_material;

public:
	Vector2 m_position;
	Vector2 m_pivot;
	float m_rotationDegrees;
	Vector2 m_scale;
	Rgba m_color;

protected:
	Sprite();
	Sprite(std::string const & spriteID, int layer = 0);

public:
	static Sprite * Create(std::string const & spriteID, int layer = 0);
	~Sprite();

	void SwapSprite(std::string const & spriteID);
	Material * GetMaterial() const;
	void SetMaterial(Material* mat);
	Texture const * GetTexture() const;
	float GetWidth() const;
	float GetHeight() const;

	Vector2 GetSpriteBottomLeft();
	Vector2 GetSpriteTopRight();

	Vector2 GetSpriteBottomLeftNoRotation();
	Vector2 GetSpriteTopRightNoRotation();

	int GetLayerID();

	void ChangeSpriteLayer(int newLayer);

	void SetEnabled(bool isEnabled);
	bool IsEnabled() const;

	void SetUseCamera(bool usesCamera); //Toggles if the SpriteRenderer will offset by the current camera position. Default true.
	bool IsUsingCamera() const;
};