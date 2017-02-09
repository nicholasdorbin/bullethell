#include "Game/Bullet.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteGameRenderer.hpp"
#include "Engine/MathUtils.hpp"
#include "Game/TheGame.hpp"
#include "Game/Player.hpp"
#include <vector>

const float BULLET_SPEED_PLAYER = 11.f;
const float BULLET_SPEED_ENEMY = 6.f;
Bullet::Bullet()
	: Entity()
{
	
}

Bullet::Bullet(Vector2 pos, float dir, bool enemyBullet, int damage)
	:Entity(pos)
{
	m_pos = pos;
	m_orientation = dir;

	m_isEnemy = enemyBullet;
	m_bulletDamage = damage;

	if (m_isEnemy)
	{
		m_sprite = Sprite::Create("enemy-bullet", -2);
		//m_sprite->m_scale = Vector2(0.1f, 0.1f);
		m_velocity = Vector2(0.f, -1.f);
		m_speed = BULLET_SPEED_ENEMY;
	}
	else
	{
		m_sprite = Sprite::Create("player-bullet", -1);
		//m_sprite->m_scale = Vector2(0.1f, 0.1f);
		m_velocity = Vector2(0.f, 1.f);
		m_speed = BULLET_SPEED_PLAYER;
	}
	m_sprite->m_position = m_pos;
	SetPhysicsRadius();

}

void Bullet::Update(const float deltaSeconds)
{

	HandleBulletCollisions();
	m_velocity.x = m_speed * CosDegrees(m_orientation);
	m_velocity.y = m_speed * SinDegrees(m_orientation);

	m_pos.x += m_velocity.x * deltaSeconds;
	m_pos.y += m_velocity.y * deltaSeconds;

	m_sprite->m_position = m_pos;
	m_bounds = GetBounds();

	if (IsOutOfBounds())
		m_isDead = true;

	if (m_isDead)
	{
// 		Explosion* exp = new Explosion(m_pos, m_cosmeticRadius, 0.5f, m_orientation);
// 		g_theGame->m_map->m_explosions.push_back(exp);
// 		SoundID sound = g_theGame->m_audio->CreateOrGetSound(BULLET_SOUND);
// 		g_theGame->m_audio->PlaySound(sound, 0.5f);
	}
}

void Bullet::HandleBulletCollisions()
{
	for (std::vector<Entity*>::iterator it = g_theGame->m_world->m_entities.begin(); it != g_theGame->m_world->m_entities.end(); ++it)
	{
		Entity* ent = *it;
		if (ent == this)
		{
			continue;
		}
		if (IsPointInDisc(ent->m_pos, ent->m_physicsRadius, m_pos))
		{
			if (m_isEnemy != ent->m_isEnemy  && !(ent->m_isDead || ent->m_health <= 0))
			{
   				m_isDead = true;
				ent->TakeDamage(m_bulletDamage);
				//ent->m_health -= m_bulletDamage;
				break;
			}
		}
	}

	//Do check for player because player isn't an ent;

	if (m_isEnemy)
	{
		Player& player = g_theGame->m_world->m_player;
		if (IsPointInDisc(player.m_pos, player.m_physicsRadius, m_pos))
		{
			if (!(player.m_isDead || (player.m_health <= 0)))
			{
				m_isDead = true;
				player.m_health -= m_bulletDamage;
			}
		}
	}
}

bool Bullet::IsOutOfBounds()
{
	AABB2 screenBounds = g_spriteRenderSystem->GetVirtualBounds();
	Vector2 diff = Vector2::ZERO;
	if (m_bounds.m_maxs.x < screenBounds.m_mins.x)
	{
		diff.x = screenBounds.m_maxs.x - m_bounds.m_mins.x;
		return true;
	}
	else if (m_bounds.m_mins.x > screenBounds.m_maxs.x)
	{
		diff.x = screenBounds.m_mins.x - m_bounds.m_maxs.x;
		return true;
	}


	if (m_bounds.m_maxs.y < screenBounds.m_mins.y)
	{
		diff.y = screenBounds.m_maxs.y - m_bounds.m_mins.y;
		return true;
	}
	else if (m_bounds.m_mins.y > screenBounds.m_maxs.y)
	{
		diff.y = screenBounds.m_mins.y - m_bounds.m_maxs.y;
		return true;
	}
	return false;
}


