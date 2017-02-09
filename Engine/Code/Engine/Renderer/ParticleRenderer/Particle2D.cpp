#include "Engine/Renderer/ParticleRenderer/Particle2D.hpp"

Particle2D::Particle2D()
	: m_position(Vector2::ZERO)
	, m_velocity(Vector2::ZERO)
	, m_acceleration(Vector2::ZERO)
	, m_age(0.f)
	, m_maxAge(1.f)
	, m_tint(Rgba::WHITE)
	, m_alpha(1.f)
	, m_sprite(nullptr)
{

}

Particle2D::~Particle2D()
{
	delete m_sprite;
}

void Particle2D::SetPosition(Vector2 const & position)
{
	m_position = position;
	if (m_sprite != nullptr)
	{
		m_sprite->m_position = position;
	}
}

