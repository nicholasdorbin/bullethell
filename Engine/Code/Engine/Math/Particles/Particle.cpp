#include "Engine/Math/Particles/Particle.hpp"
#include "Engine/Renderer/Renderer.hpp"

Particle::Particle()
{
	m_position = Vector3::ZERO;
	m_gravity = Vector3(0.f,0.f,-9.8f);
	m_velocity = Vector3::ZERO;
	m_mass = 1.f;
	m_lifetime = 0.f;
	m_secondsLeft = 0.f;
	m_color = Rgba::WHITE;
	m_canBounce = false;
}

Particle::Particle(const Vector3& position /*= Vector3::ZERO*/, 
	const Vector3& velocity /*= Vector3::ZERO*/,
	const float lifetime /*= 5.f*/, 
	const float mass /*= 1.f */,
	const Rgba color,
	const bool canBounce)
{
	m_position = position;
	m_gravity = Vector3(0.f, 0.f, -9.8f);
	m_velocity = velocity;
	m_mass = mass;
	m_lifetime = lifetime;
	m_secondsLeft = lifetime;
	m_color = color;
	m_canBounce = canBounce;
}

void Particle::Render()
{
	g_theRenderer->DrawPoint(m_position, m_color);
}

void Particle::Update(const float deltaSeconds)
{
	Vector3 totalAcceleration = m_gravity;
	for (auto forceIter = m_force.begin(); forceIter != m_force.end(); ++forceIter)
	{
		Vector3 forceVec = forceIter->m_direction * forceIter->m_magnitude;
		Vector3 acceleration = forceVec / m_mass;
		totalAcceleration += acceleration;
		forceIter->m_secondsLeft -= deltaSeconds;
	}
	m_velocity += totalAcceleration * deltaSeconds;
	m_position += m_velocity * deltaSeconds;
	m_secondsLeft -= deltaSeconds;


	if (m_canBounce && m_position.z < 0.f)
	{
		m_velocity = m_velocity * 0.9f;
		if (m_velocity.z < 0)
			m_velocity.z *= -1;
	}
	//Check to see if any forces need to be removed
	std::vector<Force>::iterator it = m_force.begin();

	while (it != m_force.end()) {
		if (it->m_isConstant) //Run forever if constant
		{
			++it;
		}
		else
		{
			if (it->m_secondsLeft <= 0.f)
			{
				it = m_force.erase(it);
			}
			else
			{
				++it;
			}
		}

	}
}

void Particle::AddForce(const Vector3& direction, const float magnitude, const float secondsleft, const bool isConstant)
{
	Force f = Force();
	f.m_direction = direction;
	f.m_magnitude = magnitude;
	f.m_secondsLeft = secondsleft;
	f.m_isConstant = isConstant;

	m_force.push_back(f);
}

