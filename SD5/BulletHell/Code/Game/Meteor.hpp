#pragma once
#pragma once
#include "Game/Entity.hpp"

class Vector2;

class Meteor : public Entity
{
public:

	bool m_isShooting;
	bool m_isPlayerVisible;
	float m_nextShootTick;
	int m_collisionDamage;
	float angularDisplacement;


	Meteor();
	Meteor(Vector2 pos);
	bool IsOutOfBounds();
	void Meteor::Update(const float deltaSeconds);

private:

};