#pragma once
#include "Engine/Math/Vector3.hpp"
#include <vector>
#include "Engine/Math/Rgba.hpp"

struct Force
{
	Vector3 m_direction;
	float m_secondsLeft;
	float m_magnitude;
	bool m_isConstant;
};

class Particle
{
public:
	Particle();
	Particle(const Vector3& position = Vector3::ZERO)
		:Particle(position,
			Vector3::ZERO,
			5.f,
			1.f) {};
	Particle(const Vector3& position = Vector3::ZERO, 
		const Vector3& velocity = Vector3::ZERO,  
		const float lifetime = 5.f, 
		const float mass = 1.f ,
		const Rgba color = Rgba::WHITE,
		const bool canBounce = false);

	Vector3 m_position;
	Vector3 m_gravity;
	Vector3 m_velocity;
	float m_mass;
	float m_lifetime;
	float m_secondsLeft;
	bool m_canBounce;
	Rgba m_color;

	std::vector<Force> m_force;

	void Render();
	void Update(const float deltaSeconds);

	void AddForce(const Vector3& direction, const float magnitude, const float secondsleft, const bool isConstant);
};