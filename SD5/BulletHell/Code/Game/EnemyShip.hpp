#pragma once
#include "Game/Entity.hpp"

class Vector2;

class EnemyShip : public Entity
{
public:

	bool m_isShooting;
	bool m_isPlayerVisible;
	float m_nextShootTick;
	int m_collisionDamage;


	EnemyShip();
	EnemyShip(Vector2 pos);
	void EnemyShip::Update(const float deltaSeconds);

private:
	bool CanSeePlayer();
	float GetDistToPlayer();
	bool IsOutOfBounds();
};