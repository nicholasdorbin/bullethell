#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Rgba.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"

/*
• Position
• Velocity
• Acceleration
• Age
• Max Age
• Tint
• alpha

*/

struct Particle2D
{
	Vector2 m_position;
	Vector2 m_velocity;
	Vector2 m_acceleration;
	float m_age;
	float m_maxAge;
	Rgba m_tint;
	float m_alpha;

	//Sprite
	Sprite* m_sprite;

	Particle2D();
	~Particle2D();

	void SetPosition(Vector2 const & position);
};