#pragma once
#include "Engine/Math/Particles/Particle.hpp"
#include "Engine/Math/Vector3.hpp"
#include <vector>

class Emitter
{
public:
	enum EmitterType
	{
		EMIT_EXPLOSION = 0,
		EMIT_FIREWORK,
		EMIT_SMOKE,
		EMIT_FOUNTAIN,
		EMIT_DEBRIS,
		EMIT_CUSTOM
	};

	Emitter();
	Emitter(const Vector3& position, const Vector3& direction, const float emissionsPerSeconds,const int maxParticles = 1, 
		const EmitterType type = EMIT_EXPLOSION, const Vector3 minDegrees = Vector3::ZERO, const Vector3 maxDegrees = Vector3(360.f,360.f,360.f));

	Vector3 m_position;
	Vector3 m_direction;
	float m_emissionsPerSeconds;

	Vector3 m_minDegrees;
	Vector3 m_maxDegrees;
	EmitterType m_type;
	bool m_usesCone;

	std::vector<Particle*> m_particles;
	int m_maxParticles;
	float m_secondsSinceLastEmission;

	void Render();
	void Update(const float deltaSeconds);
	void FireEmitter();
	void RemoveDeadParticles();
	void HandleTypeConstruction();

	//Unique constructor functions
	void MakeExplosion();
	void MakeFirework();
	void MakeSmoke();
	void MakeFountain();
	void MakeDebris();
	void MakeCustom();

	Vector3 GetRandParticleDirection();
};