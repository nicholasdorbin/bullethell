#include "Game/EnemyShip.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteGameRenderer.hpp"
#include "Game/Bullet.hpp"

const float MAX_DISTANCE_TO_PLAYER = 20.f;
const float FIRE_RATE = 5.f;
const float MOVEMENT_RATE = 5.f;
const int MAX_HEALTH = 1;
const int ENEMY_BULLET_DAMAGE = 1;
const int ENEMY_TOUCH_DAMAGE = 1;

EnemyShip::EnemyShip()
	:Entity()
{
	m_health = MAX_HEALTH;
	m_pos = Vector2::ZERO;
	m_sprite = Sprite::Create("enemy-ship");
	m_sprite->m_scale = Vector2(0.1f, 0.1f);

	m_bounds = GetBounds();
	m_isShooting = false;
	m_isEnemy = true;
	m_canTakeDamage = true;
	m_nextShootTick = 0.f;
	m_age = 0.f;
	m_orientation = 270.f;

	m_bulletDamage = ENEMY_BULLET_DAMAGE;
	m_collisionDamage = ENEMY_TOUCH_DAMAGE;
	SetPhysicsRadius();
}

EnemyShip::EnemyShip(Vector2 pos)
	:Entity()
{
	m_health = MAX_HEALTH;
	m_pos = pos;
	m_sprite = Sprite::Create("enemy-ship");
	m_sprite->m_scale = Vector2(0.1f, 0.1f);
	m_sprite->m_position = m_pos;

	m_bounds = GetBounds();
	m_isShooting = false;
	m_canTakeDamage = true;
	m_isEnemy = true;
	m_nextShootTick = 0.f;
	m_age = 0.f;
	m_orientation = 270.f;

	m_bulletDamage = ENEMY_BULLET_DAMAGE;
	m_collisionDamage = ENEMY_TOUCH_DAMAGE;
	SetPhysicsRadius();
}

bool EnemyShip::CanSeePlayer()
{
	//Get tile distance to player
	Vector2 start = m_pos;
	Vector2 end = g_theGame->m_world->m_player.m_pos;
	float distToPlayer = GetDistToPlayer();

	if (distToPlayer <= MAX_DISTANCE_TO_PLAYER)
	{
		return true;
	}
	return false;
}

float EnemyShip::GetDistToPlayer()
{
	Vector2 start = m_pos;
	Vector2 end = g_theGame->m_world->m_player.m_pos;
	Vector2 vecToPlayer = end - start;
	float distToPlayer = vecToPlayer.GetMagnitude();
	return distToPlayer;
}

bool EnemyShip::IsOutOfBounds()
{
	AABB2 screenBounds = g_spriteRenderSystem->GetVirtualBounds();
	Vector2 diff = Vector2::ZERO;
	


	if (m_bounds.m_maxs.y < screenBounds.m_mins.y)
	{
		diff.y = screenBounds.m_maxs.y - m_bounds.m_mins.y;
		return true;
	}
	
	return false;
}

void EnemyShip::Update(const float deltaSeconds)
{
	
	m_age += deltaSeconds;
	m_isPlayerVisible = CanSeePlayer();
	float distToPlayer = GetDistToPlayer();

	bool isPlayerAlive = g_theGame->m_world->m_player.m_health > 0 ? true : false;

	if (m_health <= 0)
	{
		m_isDead = true;
// 		SoundID sound = g_theGame->m_audio->CreateOrGetSound(ENEMY_DIE_SOUND);
// 		g_theGame->m_audio->PlaySound(sound, 1.f);
	}
	else
		m_isDead = false;

	if (IsOutOfBounds())
	{
		m_isDead = true;
	}

	//Handle touching collision
	Vector2 playerPos = g_theGame->m_world->m_player.m_pos;
	float playerRadius = g_theGame->m_world->m_player.m_physicsRadius;
	if (DoDiscsOverlap(m_pos, m_physicsRadius, playerPos, playerRadius))
	{
		m_isDead = true;
		g_theGame->m_world->m_player.m_health -= m_collisionDamage;
	}

	if (distToPlayer <= MAX_DISTANCE_TO_PLAYER)
	{
		if (!m_isDead)
		{
			if (m_isPlayerVisible && isPlayerAlive)
			{
				m_isShooting = true;
			}
			else
			{
				m_isShooting = false;
			}

			m_velocity.x = MOVEMENT_RATE * cos(DegToRad(m_orientation));
			m_velocity.y = MOVEMENT_RATE * sin(DegToRad(m_orientation));

			m_pos.x += m_velocity.x * deltaSeconds;
			m_pos.y += m_velocity.y * deltaSeconds;

			m_sprite->m_position = m_pos;
			m_bounds = GetBounds();

			if (m_isShooting && m_nextShootTick < m_age)
			{
				m_nextShootTick = m_age + FIRE_RATE;
				Vector2 downDisp = Vector2(m_pos.x, m_bounds.m_mins.y);
				Bullet* newBullet = new Bullet(downDisp, 270.f, true, m_bulletDamage);

				g_theGame->m_world->m_entitiesToAdd.push_back(newBullet);
				
			}
		}
		else
		{
			//Play Dead Effects
		}
	}


}
