#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Rgba.hpp"

class Sprite;

class Entity
{
public:

	Sprite* m_sprite;
	Vector2 m_pos;
	Vector2 m_velocity;
	AABB2 m_bounds;
	Rgba m_tint;
	float m_physicsRadius;
	float m_orientation;
	float m_age;
	bool m_isDead;
	bool m_isEnemy;
	bool m_canTakeDamage;
	int m_health;
	int m_bulletDamage;


	Entity();
	Entity(Vector2 pos);
	~Entity();

	virtual void Update(const float deltaSeconds);
	virtual void Render() const;
	virtual void TakeDamage(int damage);


	void SetPhysicsRadius();
	AABB2 GetBounds();

};