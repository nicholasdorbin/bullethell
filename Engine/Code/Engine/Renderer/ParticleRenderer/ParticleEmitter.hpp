#pragma once
#include "Engine/Core/Memory/ObjectPool.hpp"
#include "Engine/Renderer/ParticleRenderer/Particle2D.hpp"
#include "Engine/Renderer/Shaders/Material.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Rgba.hpp"
#include "Engine/Math/Range.hpp"
#include <string>

struct Particle2D;




class ParticleEmitterDefinition
{
private:
public:
	int m_initialSpawn;
	float m_spawnRate;
	Range<float> m_life;
	Range<float> m_scale;
	Rgba m_tint;
	bool m_isLooping;

	
	Range<Vector2> m_speed;
	std::string m_spriteName;

	ParticleEmitterDefinition(XMLNode topNode);
public:
	ParticleEmitterDefinition(std::string const & spriteName);
	~ParticleEmitterDefinition();
};


class ParticleEmitter
{
private:
public:
	ParticleEmitterDefinition const * m_particleEmitterDefinition;
	float m_emitTimer;
	int m_layerID;

	ParticleEmitter(ParticleEmitterDefinition const * definition);
	~ParticleEmitter();

	Particle2D * SpawnParticle(Vector2 const & position) const;
};