#pragma once


#include <string>
#include <vector>
#include "Engine/Renderer/ParticleRenderer/Particle2D.hpp"
#include "Engine/Renderer/ParticleRenderer/ParticleEmitter.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"


//-------------------------------------------------------------------------------------------------
class Vector2;


//-------------------------------------------------------------------------------------------------
struct ParticleSystemDefinition
{
public:
	std::string id;
	std::vector<ParticleEmitterDefinition const *> m_emitterResources;

public:
	ParticleSystemDefinition(XMLNode systemNode);
	ParticleSystemDefinition(std::string const & resourceID);
	~ParticleSystemDefinition();

	void AddEmitter(ParticleEmitterDefinition const * emitter);
};


//-------------------------------------------------------------------------------------------------
class ParticleSystem
{
private:
	ParticleSystemDefinition const * m_particleSystemResource;
	bool m_isDead;
	Vector2 m_position;
	std::vector<ParticleEmitter*> m_emitters;
	std::vector<Particle2D*> m_particles;

public:
	ParticleSystem(ParticleSystemDefinition const * resource, int layer, Vector2 const & position);
	~ParticleSystem();

	void Update(float deltaSeconds);

	void UpdateParticles(float deltaSeconds);
	void CleanUpParticles();
	void SpawnParticles(float deltaSeconds);

	void Initialize();
	void SetSpawnPosition(Vector2 const & position);
	void Destroy();
	bool IsFinished();
};
