#include "Game/Star.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/MathUtils.hpp"
#include "Engine/Renderer/SpriteRenderer/Sprite.hpp"
#include "Engine/Renderer/SpriteRenderer/SpriteGameRenderer.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"

const float MAX_DISTANCE_TO_PLAYER = 20.f;
const float FIRE_RATE = 0.5f;
const float MOVEMENT_RATE = 2.f;//2.5f;
const int MAX_HEALTH = 15;
const int ENEMY_TOUCH_DAMAGE = 1;


Star::Star()
	:Entity()
{
	m_health = MAX_HEALTH;
	m_pos = Vector2::ZERO;
	m_sprite = Sprite::Create("star1", LAYER_BACKGROUND);
	float scaleAmount = GetRandomFloatBetween(0.2f, 0.7f);
	m_sprite->m_scale = Vector2(scaleAmount, scaleAmount);
	m_sprite->m_position = m_pos;

	m_bounds = GetBounds();
	m_isShooting = false;
	m_isEnemy = false;
	m_canTakeDamage = false;
	m_nextShootTick = 0.f;
	m_age = 0.f;
	m_orientation = 270.f;
	m_angularDisplacement = GetRandomFloatBetween(30.f, 155.f);
	m_moveMod = GetRandomFloatBetween(0.8f, 5.f);
	if (GetRandomTrueOrFalseWithinProbability(0.5f))
	{
		m_angularDisplacement *= -1.f;
	}



	SetPhysicsRadius();
}

Star::Star(Vector2 pos)
	: Entity()
{
	m_health = MAX_HEALTH;
	m_pos = pos;
	m_sprite = Sprite::Create("star1", LAYER_BACKGROUND);
	float scaleAmount = GetRandomFloatBetween(0.3f, 0.8f);
	m_sprite->m_scale = Vector2(scaleAmount, scaleAmount);
	m_sprite->m_position = m_pos;

	m_bounds = GetBounds();
	m_isShooting = false;
	m_isEnemy = false;
	m_canTakeDamage = false;
	m_nextShootTick = 0.f;
	m_age = 0.f;
	m_orientation = 270.f;
	m_angularDisplacement = GetRandomFloatBetween(30.f, 155.f);
	m_moveMod = GetRandomFloatBetween(1.f, 5.f);
	if (GetRandomTrueOrFalseWithinProbability(0.5f))
	{
		m_angularDisplacement *= -1.f;
	}



	SetPhysicsRadius();
}

void Star::Update(const float deltaSeconds)
{
	g_theProfiler->StartProfilerSample("StarUpdate");
	m_age += deltaSeconds;

	m_sprite->m_rotationDegrees += (m_angularDisplacement * deltaSeconds);

	if (m_bounds.m_maxs.y <= g_spriteRenderSystem->GetVirtualBounds().m_mins.y)
	{
		m_isDead = true;
		// 		SoundID sound = g_theGame->m_audio->CreateOrGetSound(ENEMY_DIE_SOUND);
		// 		g_theGame->m_audio->PlaySound(sound, 1.f);
	}
	else
		m_isDead = false;




	//bool isPlayerAlive = g_theGame->m_world->m_player.m_health > 0 ? true : false;

	if (!m_isDead)
	{


		m_velocity.x = MOVEMENT_RATE * m_moveMod * cos(DegToRad(m_orientation));
		m_velocity.y = MOVEMENT_RATE * m_moveMod * sin(DegToRad(m_orientation));

		m_pos.x += m_velocity.x * deltaSeconds;
		m_pos.y += m_velocity.y * deltaSeconds;

		m_sprite->m_position = m_pos;
		m_bounds = GetBounds();
	}
	else
	{
		//Play Dead Effects
	}
	g_theProfiler->EndProfilerSample();
}
