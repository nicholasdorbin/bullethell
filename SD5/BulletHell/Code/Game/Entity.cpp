#include "Game/Entity.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"

Entity::Entity()
{
	m_pos = Vector2::ZERO;
	m_velocity = Vector2::ZERO;
	m_sprite = nullptr;
	m_age = 0.f;
	m_tint = Rgba::WHITE;
	m_isDead = false;
	m_isEnemy = false;
	m_canTakeDamage = false;
	m_health = 1;
	m_bulletDamage = 1;
	m_physicsRadius = 1.f;
}

Entity::Entity(Vector2 pos)
{
	m_pos = pos;
	m_velocity = Vector2::ZERO;
	m_sprite = nullptr;
	m_age = 0.f;
	m_tint = Rgba::WHITE;
	m_isDead = false;
	m_isEnemy = false;
	m_canTakeDamage = false;
	m_health = 1;
	m_bulletDamage = 1;
	m_physicsRadius = 1.f;
}

Entity::~Entity()
{
	delete m_sprite;
}

void Entity::Update(const float deltaSeconds)
{
	m_age += deltaSeconds;
}

void Entity::Render() const
{

}

void Entity::TakeDamage(int damage)
{
	if (m_canTakeDamage)
	{
		m_health -= damage;

		if (m_health <= 0)
		{
			m_isDead = true;
		}
	}
}

void Entity::SetPhysicsRadius()
{
	float highestDimension = m_sprite->GetHeight() * m_sprite->m_scale.y;
	float spriteWidth = m_sprite->GetWidth() * m_sprite->m_scale.x;
	if (highestDimension < spriteWidth)
		highestDimension = spriteWidth;

	m_physicsRadius = highestDimension / 2.f;
}

AABB2 Entity::GetBounds()
{
	//Vector2 mins = Vector2(m_pos.x - (PLAYER_WIDTH / 2.f), m_pos.y - (PLAYER_WIDTH / 2.f));
	//Vector2 maxs = Vector2(m_pos.x + (PLAYER_WIDTH / 2.f), m_pos.y + (PLAYER_WIDTH / 2.f));
	Vector2 mins = m_sprite->GetSpriteBottomLeft();
	Vector2 maxs = m_sprite->GetSpriteTopRight();
	return AABB2(mins, maxs);
}
