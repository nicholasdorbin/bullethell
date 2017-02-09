#include "Game/Meteor.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/MathUtils.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteGameRenderer.hpp"
#include "Game/GameCommon.hpp"

const float MAX_DISTANCE_TO_PLAYER = 20.f;
const float FIRE_RATE = 0.5f;
const float MOVEMENT_RATE = 8.f;
const int MAX_HEALTH = 15;
const int ENEMY_TOUCH_DAMAGE = 1;


Meteor::Meteor()
	:Entity()
{
	m_health = MAX_HEALTH;
	m_pos = Vector2::ZERO;
	m_sprite = Sprite::Create("meteor", LAYER_ENEMY_BULLET - 1);
	m_sprite->m_scale = Vector2(0.1f, 0.1f);
	m_sprite->m_position = m_pos;
	m_sprite->m_color = Rgba::RED;

	m_bounds = GetBounds();
	m_isShooting = false;
	m_isEnemy = true;
	m_canTakeDamage = false;
	m_nextShootTick = 0.f;
	m_age = 0.f;
	m_orientation = 270.f;

	angularDisplacement = GetRandomFloatBetween(-90.f, 90.f);

	m_collisionDamage = ENEMY_TOUCH_DAMAGE;
	SetPhysicsRadius();
}

Meteor::Meteor(Vector2 pos)
	: Entity()
{
	m_health = MAX_HEALTH;
	m_pos = pos;
	m_sprite = Sprite::Create("meteor", LAYER_ENEMY_BULLET - 1);
	m_sprite->m_scale = Vector2(0.1f, 0.1f);
	m_sprite->m_position = m_pos;
	m_sprite->m_color = Rgba::LTBLUE;

	m_bounds = GetBounds();
	m_isShooting = false;
	m_isEnemy = true;
	m_canTakeDamage = false;
	m_nextShootTick = 0.f;
	m_age = 0.f;
	m_orientation = 270.f;
	angularDisplacement = GetRandomFloatBetween(20.f, 135.f);
	if (GetRandomTrueOrFalseWithinProbability(0.5f))
	{
		angularDisplacement *= -1.f;
	}


	m_collisionDamage = ENEMY_TOUCH_DAMAGE;
	SetPhysicsRadius();
}

void Meteor::Update(const float deltaSeconds)
{
	m_age += deltaSeconds;

	m_sprite->m_rotationDegrees += (angularDisplacement * deltaSeconds);

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

	Vector2 playerPos = g_theGame->m_world->m_player.m_pos;
	float playerRadius = g_theGame->m_world->m_player.m_physicsRadius;
	if (DoDiscsOverlap(m_pos, m_physicsRadius, playerPos, playerRadius))
	{
		m_isDead = true;
		g_theGame->m_world->m_player.m_health -= m_collisionDamage;
	}


	//bool isPlayerAlive = g_theGame->m_world->m_player.m_health > 0 ? true : false;

	if (!m_isDead)
	{
		

		m_velocity.x = MOVEMENT_RATE * cos(DegToRad(m_orientation));
		m_velocity.y = MOVEMENT_RATE * sin(DegToRad(m_orientation));

		m_pos.x += m_velocity.x * deltaSeconds;
		m_pos.y += m_velocity.y * deltaSeconds;

		m_sprite->m_position = m_pos;
		m_bounds = GetBounds();
	}
	else
	{
		//Play Dead Effects
	}

}


bool Meteor::IsOutOfBounds()
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