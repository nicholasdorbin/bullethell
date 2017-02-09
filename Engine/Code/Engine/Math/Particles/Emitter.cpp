#include "Engine/Math/Particles/Emitter.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <stdlib.h>
#include "Engine/MathUtils.hpp"

Emitter::Emitter()
{
	m_position = Vector3::ZERO;
	m_direction = Vector3::ZERO;
	m_emissionsPerSeconds = 1.f;
	m_secondsSinceLastEmission = 0;

	m_minDegrees = Vector3::ZERO;
	m_maxDegrees = Vector3::ZERO;

	m_maxParticles = 1;
	m_type = EMIT_EXPLOSION;
	m_usesCone = false;
	HandleTypeConstruction();
}

Emitter::Emitter(const Vector3& position, const Vector3& direction, const float emissionsPerSeconds, const int maxParticles,
	const EmitterType type, const Vector3 minDegrees, const Vector3 maxDegrees)
{
	m_position = position;
	m_direction = direction;
	m_emissionsPerSeconds = emissionsPerSeconds;

	m_minDegrees = minDegrees;
	m_maxDegrees = maxDegrees;
	m_secondsSinceLastEmission = 0;

	m_maxParticles = maxParticles;
	m_type = type;
	m_usesCone = false;
	HandleTypeConstruction();
}

void Emitter::Render()
{
	g_theRenderer->DrawPoint(m_position, Rgba::PINK);
	//TODO: Iterate thru the vector and draw the particles
	for (auto partIter = m_particles.begin(); partIter != m_particles.end(); ++partIter)
	{
		(*partIter)->Render();
	}
}

void Emitter::Update(const float deltaSeconds)
{
	for (auto partIter = m_particles.begin(); partIter != m_particles.end(); ++partIter)
	{
		(*partIter)->Update(deltaSeconds);
	}

	float emissions = m_secondsSinceLastEmission * m_emissionsPerSeconds;
	if (1.f < emissions)
	{
		FireEmitter();
// 		if (m_type == EMIT_CUSTOM)
// 		{
// 			
// 		}
	}
	m_secondsSinceLastEmission += deltaSeconds;
	RemoveDeadParticles();
}

void Emitter::FireEmitter()
{
	HandleTypeConstruction();
	
	m_secondsSinceLastEmission = 0.f;
}

void Emitter::RemoveDeadParticles()
{
	std::vector<Particle*>::iterator it = m_particles.begin();

	while (it != m_particles.end()) {
		if ((*it)->m_lifetime == -1.f) //Run forever if constant
		{
			++it;
		}
		else
		{
			if ((*it)->m_secondsLeft <= 0.f)
			{
				delete (*it);
				it = m_particles.erase(it);
			}
			else
			{
				++it;
			}
		}

	}
}

void Emitter::HandleTypeConstruction()
{
	switch (m_type)
	{
	case Emitter::EMIT_EXPLOSION:
		MakeExplosion();
		break;
	case Emitter::EMIT_FIREWORK:
		MakeFirework();
		break;
	case Emitter::EMIT_SMOKE:
		MakeSmoke();
		break;
	case Emitter::EMIT_FOUNTAIN:
		MakeFountain();
		break;
	case Emitter::EMIT_DEBRIS:
		MakeDebris();
		break;
	case Emitter::EMIT_CUSTOM:
		MakeCustom();
		break;
	default:
		MakeExplosion();
		break;
	}

}

void Emitter::MakeExplosion()
{
	for (int i = 0; i < m_maxParticles; i++)
	{
		Vector3 dir = GetRandParticleDirection();

		Particle* newPart = new Particle(m_position, Vector3::ZERO, 1.f);
		newPart->m_gravity = Vector3::ZERO;
		newPart->m_color = Rgba::RED;

		newPart->AddForce(dir, 20.f, 0.2f, false);

		m_particles.push_back(newPart);
	}
}

void Emitter::MakeFirework()
{
	for (int i = 0; i < m_maxParticles; i++)
	{
		Vector3 dir = GetRandParticleDirection();

		Particle* newPart = new Particle(m_position, Vector3(0.f,0.f, 10.f), 1.f);
		newPart->m_gravity = Vector3(0.f,0.f,-9.8f);
		newPart->m_color = Rgba::RED;

		newPart->AddForce(dir, 20.f, 5.f, false);

		m_particles.push_back(newPart);
	}
}

void Emitter::MakeSmoke()
{
	for (int i = 0; i < m_maxParticles; i++)
	{
		Vector3 partDir = GetRandParticleDirection();
		Vector3 windDir = Vector3(1.f,0.f,0.f);

		Particle* newPart = new Particle(m_position, Vector3(0.f, 0.f, 5.f), 15.f);
		newPart->m_gravity = Vector3::ZERO;
		newPart->m_color = Rgba::YELLOW;

		newPart->AddForce(partDir, 10.f, 0.2f, false);
		newPart->AddForce(windDir, 1.f, 5.f, true);

		m_particles.push_back(newPart);
	}
}

void Emitter::MakeFountain()
{
	for (int i = 0; i < m_maxParticles; i++)
	{
		Vector3 partDir = GetRandParticleDirection();
		

		Particle* newPart = new Particle(m_position, Vector3(0.f, 0.f, 20.f), 15.f);
		newPart->m_gravity = Vector3(0.f, 0.f, -9.8f);
		newPart->m_color = Rgba::BLUE;

		newPart->AddForce(partDir, 10.f, 0.2f, false);

		m_particles.push_back(newPart);
	}
}

void Emitter::MakeDebris()
{
	for (int i = 0; i < m_maxParticles; i++)
	{
		Vector3 partDir = GetRandParticleDirection();


		Particle* newPart = new Particle(m_position, Vector3(0.f, 0.f, 8.f), 8.f);
		newPart->m_gravity = Vector3(0.f, 0.f, -9.8f);
		newPart->m_color = Rgba::GREEN;
		newPart->m_canBounce = true;

		newPart->AddForce(partDir, 10.f, 0.2f, false);

		m_particles.push_back(newPart);
	}
}

void Emitter::MakeCustom()
{
	for (int i = 0; i < m_maxParticles; i++)
	{
		Vector3 windDir = Vector3(0.f, 1.f, 0.f);
		Vector3 partDir = GetRandParticleDirection();
		partDir += m_direction;
		partDir.SetNormalized();


		Particle* newPart = new Particle(m_position, Vector3(5.f, 0.f, 8.f), 8.f);
		newPart->m_gravity = Vector3(0.f, 0.f, -9.8f);
		newPart->m_color = Rgba::BLUE;
		newPart->m_canBounce = true;

		newPart->AddForce(partDir, 10.f, 0.2f, false);
		newPart->AddForce(windDir, 8.f, 5.f, true);

		m_particles.push_back(newPart);
	}
}

Vector3 Emitter::GetRandParticleDirection()
{
	float LO = m_minDegrees.x;
	float HI = m_maxDegrees.x;
	//float degX = static_cast <float>(rand()) / static_cast <float>(m_maxDegrees.x + m_minDegrees.x);
	float degX = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));

	LO = m_minDegrees.y;
	HI = m_maxDegrees.y;
	float degY = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));

	LO = m_minDegrees.z;
	HI = m_maxDegrees.z;
	float degZ = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));

	degX = RangeMap(degX, m_minDegrees.x, m_maxDegrees.x, -1.f, 1.f);
	degY = RangeMap(degY, m_minDegrees.y, m_maxDegrees.y, -1.f, 1.f);
	degZ = RangeMap(degZ, m_minDegrees.z, m_maxDegrees.z, -1.f, 1.f);

	Vector3 dir = Vector3(degX, degY, degZ);

	dir.SetNormalized();

	return dir;
}

