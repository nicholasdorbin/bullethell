#pragma once
#include "Game/Entity.hpp"

class Vector2;

class Star : public Entity
{
public:

	bool m_isShooting;
	bool m_isPlayerVisible;
	float m_nextShootTick;
	int m_collisionDamage;
	float m_angularDisplacement;
	float m_moveMod;


	Star();
	Star(Vector2 pos);
	void Star::Update(const float deltaSeconds);

private:

};
