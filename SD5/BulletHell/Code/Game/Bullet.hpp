#pragma once
#include "Game/Entity.hpp"

class Bullet : public Entity
{
public:
	Bullet::Bullet();
	Bullet::Bullet(Vector2 pos, float dir, bool enemyBullet, int damage);
	void Bullet::Render();
	void Bullet::Update(const float deltaSeconds);

	float m_speed;
protected:
private:
	void HandleBulletCollisions();
	bool IsOutOfBounds();
};